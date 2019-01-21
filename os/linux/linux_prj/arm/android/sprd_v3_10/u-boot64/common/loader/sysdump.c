/*
 * Copyright (C) 2013 Spreadtrum Communications Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

//#include "normal_mode.h"
//#include <mmc.h>
//#include <fat.h>
#include <rtc.h>
#include <asm/sizes.h>
#include <boot_mode.h>
#include <common.h>

#include <linux/input.h>
#include "sysdump.h"
#include <linux/mtd/mtd.h>
#include <asm/arch/check_reboot.h>
#include <mmc.h>
#include <fat.h>
#include <exfat.h>
#include "loader_common.h"

enum {
	FS_FAT32,
	FS_EXFAT,
	FS_INVALID
};

extern void MMU_DisableIDCM(void);
//extern lcd_display(void);
extern void set_backlight(uint32_t value);
//extern void lcd_setfgcolor(int color);
extern void lcd_printf(const char *fmt, ...);
extern  int sprd_host_init(int sdio_type);

void display_writing_sysdump(void)
{
	debugf("%s\n", __FUNCTION__);
#ifdef CONFIG_SPLASH_SCREEN
	vibrator_hw_init();
	set_vibrator(1);
	lcd_printf("   -------------------------------  \n"
		   "   Sysdumpping now, keep power on.  \n"
		   "   -------------------------------  \n");
	set_backlight(255);
	set_vibrator(0);
#endif
}
void display_special_mode(void)
{
	debugf("%s\n", __FUNCTION__);
#ifdef CONFIG_SPLASH_SCREEN
	vibrator_hw_init();
	set_vibrator(1);
	lcd_printf("   -------------------------------  \n"
		   "   Restart now, keep power on.  \n"
		   "   -------------------------------  \n");
	set_backlight(255);
	set_vibrator(0);
#endif
}
static void wait_for_keypress()
{
	int key_code;

	do {
		udelay(50 * 1000);
		key_code = board_key_scan();
		//printf("key_code: %d, (vd:%d,vu:%d,p:%d)\n", key_code, KEY_VOLUMEDOWN, KEY_VOLUMEUP, KEY_POWER);
		if (key_code == KEY_VOLUMEDOWN || key_code == KEY_VOLUMEUP || key_code == KEY_HOME)
			break;
	} while (1);
	debugf("Pressed key: %d\n", key_code);
	lcd_printf("Pressed key: %d\n", key_code);
//	lcd_display();
}



int init_mmc_fat(int *fs_type)
{
	struct mmc *mmc;
	block_dev_desc_t *dev_desc = NULL;
	int ret;
	char bufread[50];
	*fs_type = FS_INVALID;
	debugf("init_mmc_fat\n");
	sprd_host_init(1);
	mmc = find_mmc_device(1);// 1 is  sd, 0 is emmc
	if(mmc) {
		ret = mmc_init(mmc);
		if(ret < 0){
			debugf("mmc init failed %d\n", ret);
			return -1;
		}
	} else {
		debugf("no mmc card found\n");
		return -1;
	}

	dev_desc = &mmc->block_dev;
	if(dev_desc==NULL){
		debugf("no mmc block device found\n");
		return -1;
	}

	ret = fat_register_device(dev_desc, 1);
	if (ret == 0) {
		ret = file_fat_detectfs();
		if(ret) {
			debugf("detect fs fat failed\n");
			return -1;
		}
		*fs_type = FS_FAT32;
		return 0;
	}
	debugf("Fat register fail. Try exFat. \n");

	ret = exfat_register_device(dev_desc, 1);
	if(ret < 0) {
		debugf("exFat register fail %d.\n", ret);
		return -1;
	}

	ret = file_exfat_detectfs();
	if(ret) {
		debugf("Detect fs exfat failed\n");
		return -1;
	}
	*fs_type = FS_EXFAT;
	return 0;
}

void write_mem_to_mmc(char *path, char *filename, int fs_type,
	void *memaddr, unsigned long memsize)
{
	int ret;

	if (path) {
		do {} while (0); /* TODO: jianjun.he */
	}

	debugf("writing 0x%lx bytes to sd file %s\n",
		memsize, filename);
	lcd_printf("writing 0x%lx bytes to sd file %s\n", memsize, filename);
//	lcd_display();

	if (fs_type==FS_FAT32)
		ret = file_fat_write(filename, memaddr, memsize);
	else if (fs_type==FS_EXFAT)
		ret = file_exfat_write(filename, memaddr, memsize);
	else
		ret = -1;

	if (ret <= 0) {
		debugf("sd file write error %d\n", ret);
	}

	return;
}

extern unsigned check_reboot_mode(void);


static size_t get_elfhdr_size(int nphdr)
{
	size_t elfhdr_len;

	elfhdr_len = sizeof(struct elfhdr) +
		(nphdr + 1) * sizeof(struct elf_phdr);
#if SETUP_NOTE
	elfhdr_len += ((sizeof(struct elf_note)) +
		roundup(sizeof(CORE_STR), 4)) * 3 +
		roundup(sizeof(struct elf_prstatus), 4) +
		roundup(sizeof(struct elf_prpsinfo), 4) +
		roundup(sizeof(struct task_struct), 4);
#endif
	elfhdr_len = PAGE_ALIGN(elfhdr_len); //why?

	return elfhdr_len;
}

#if SETUP_NOTE
static int notesize(struct memelfnote *en)
{
	int sz;

	sz = sizeof(struct elf_note);
	sz += roundup((strlen(en->name) + 1), 4);
	sz += roundup(en->datasz, 4);

	return sz;
}

static char *storenote(struct memelfnote *men, char *bufp)
{
	struct elf_note en;

#define DUMP_WRITE(addr,nr) do { memcpy(bufp,addr,nr); bufp += nr; } while(0)

	en.n_namesz = strlen(men->name) + 1;
	en.n_descsz = men->datasz;
	en.n_type = men->type;

	DUMP_WRITE(&en, sizeof(en));
	DUMP_WRITE(men->name, en.n_namesz);

	/* XXX - cast from long long to long to avoid need for libgcc.a */
	bufp = (char*) roundup((unsigned long)bufp,4);
	DUMP_WRITE(men->data, men->datasz);
	bufp = (char*) roundup((unsigned long)bufp,4);

#undef DUMP_WRITE

	return bufp;
}

#endif

static void sysdump_fill_core_hdr(struct pt_regs *regs,
						struct sysdump_mem *sysmem, int mem_num,
						char *bufp, int nphdr, int dataoff)
{
#if 0
	struct elf_prstatus prstatus;	/* NT_PRSTATUS */
	struct elf_prpsinfo prpsinfo;	/* NT_PRPSINFO */
#endif
	struct elf_phdr *nhdr, *phdr;
	struct elfhdr *elf;
	struct memelfnote notes[3];
	off_t offset = 0;
	int i;

	/* setup ELF header */
	elf = (struct elfhdr *) bufp;
	bufp += sizeof(struct elfhdr); //printk("sizeof(struct elfhdr): %d\n");
	offset += sizeof(struct elfhdr); //printk("sizeof(struct elfhdr): %d\n");
	memcpy(elf->e_ident, ELFMAG, SELFMAG); //printk("ELFMAG: %s, SELFMAG:%d\n", ELFMAG, SELFMAG);
	elf->e_ident[EI_CLASS]	= ELF_CLASS;//printk("EI_CLASS:%d, ELF_CLASS: %d", EI_CLASS, ELF_CLASS);
	elf->e_ident[EI_DATA]	= ELF_DATA;//printk("EI_DATA:%");
	elf->e_ident[EI_VERSION]= EV_CURRENT;
	elf->e_ident[EI_OSABI] = ELF_OSABI;
	memset(elf->e_ident+EI_PAD, 0, EI_NIDENT-EI_PAD);
	elf->e_type	= ET_CORE;
	elf->e_machine	= ELF_ARCH;
	elf->e_version	= EV_CURRENT;
	elf->e_entry	= 0;
	elf->e_phoff	= sizeof(struct elfhdr);
	elf->e_shoff	= 0;
	elf->e_flags	= ELF_CORE_EFLAGS;
	elf->e_ehsize	= sizeof(struct elfhdr);
	elf->e_phentsize= sizeof(struct elf_phdr);
	elf->e_phnum	= nphdr;
	elf->e_shentsize= 0;
	elf->e_shnum	= 0;
	elf->e_shstrndx	= 0;

	/* setup ELF PT_NOTE program header */
	nhdr = (struct elf_phdr *) bufp;
	bufp += sizeof(struct elf_phdr);
	offset += sizeof(struct elf_phdr);
	nhdr->p_type	= PT_NOTE;
	nhdr->p_offset	= 0;
	nhdr->p_vaddr	= 0;
	nhdr->p_paddr	= 0;
	nhdr->p_filesz	= 0;
	nhdr->p_memsz	= 0;
	nhdr->p_flags	= 0;
	nhdr->p_align	= 0;

	/* setup ELF PT_LOAD program header for every area */
	for (i = 0; i < mem_num; i++) {
		phdr = (struct elf_phdr *) bufp;
		bufp += sizeof(struct elf_phdr);
		offset += sizeof(struct elf_phdr);

		phdr->p_type	= PT_LOAD;
		phdr->p_flags	= PF_R|PF_W|PF_X;
		phdr->p_offset	= dataoff;
		phdr->p_vaddr	= sysmem[i].vaddr;
		phdr->p_paddr	= sysmem[i].paddr;
		phdr->p_filesz	= phdr->p_memsz	= sysmem[i].size;
		phdr->p_align	= 0;//PAGE_SIZE;
		dataoff += sysmem[i].size;
	}
#if SETUP_NOTE
	/*
	 * Set up the notes in similar form to SVR4 core dumps made
	 * with info from their /proc.
	 */
	nhdr->p_offset	= offset;

	/* set up the process status */
	notes[0].name = CORE_STR;
	notes[0].type = NT_PRSTATUS;
	notes[0].datasz = sizeof(struct elf_prstatus);
	notes[0].data = &prstatus;

	memset(&prstatus, 0, sizeof(struct elf_prstatus));
	//fill_prstatus(&prstatus, current, 0);
	//if (regs)
	//	memcpy(&prstatus.pr_reg, regs, sizeof(*regs));
	//else
	//	crash_setup_regs((struct pt_regs *)&prstatus.pr_reg, NULL);

	nhdr->p_filesz	= notesize(&notes[0]);
	bufp = storenote(&notes[0], bufp);

	/* set up the process info */
	notes[1].name	= CORE_STR;
	notes[1].type	= NT_PRPSINFO;
	notes[1].datasz	= sizeof(struct elf_prpsinfo);
	notes[1].data	= &prpsinfo;

	memset(&prpsinfo, 0, sizeof(struct elf_prpsinfo));
	//fill_psinfo(&prpsinfo, current, current->mm);

	strcpy(prpsinfo.pr_fname, "vmlinux");
	//strncpy(prpsinfo.pr_psargs, saved_command_line, ELF_PRARGSZ);

	nhdr->p_filesz	+= notesize(&notes[1]);
	bufp = storenote(&notes[1], bufp);

	/* set up the task structure */
	notes[2].name	= CORE_STR;
	notes[2].type	= NT_TASKSTRUCT;
	notes[2].datasz	= sizeof(struct task_struct);
	notes[2].data	= current;

	printk("%s: data size is %d, data addr is %p",__func__,notes[2].datasz,notes[2].data);

	nhdr->p_filesz	+= notesize(&notes[2]);
	bufp = storenote(&notes[2], bufp);
#endif
	return;
} /* end elf_kcore_store_hdr() */

#define PROD_PART "prodnv"
int sysdump_flag_check(void)
{
	char sysdump_buf[200]={0};

	memset(sysdump_buf,0x0,200);
	if(!do_fs_file_read(PROD_PART, "sysdump_flag", sysdump_buf,200))
		debugf("file: sysdump_flag is exist\n");
	else {
		debugf("file: sysdump_flag is not exist\n");
#ifdef SYSDUMP_BYPASS
		return -1;
#else
		return 1;
#endif
	}

	if(!strncmp(sysdump_buf, "on", 2))
		return 1;

	return -1;
}
char *rstmode[] = {
	"undefind mode",		//CMD_UNDEFINED_MODE=0,
	"power down",			//CMD_POWER_DOWN_DEVICE,
	"normal",				//CMD_NORMAL_MODE,
	"recovery",				//CMD_RECOVERY_MODE,
	"fastboot",				//CMD_FASTBOOT_MODE,
	"alarm",				//CMD_ALARM_MODE,
	"charge",				//CMD_CHARGE_MODE,
	"engtest",				//CMD_ENGTEST_MODE,
	"watchdog timeout",		//CMD_WATCHDOG_REBOOT ,
	"framework crash",		//CMD_SPECIAL_MODE,
	"manual dump",			//CMD_UNKNOW_REBOOT_MODE,
	"kernel crash",			//CMD_PANIC_REBOOT,
	"calibration",			//CMD_CALIBRATION_MODE,
	"autodloader",			//CMD_AUTODLOADER_REBOOT,
	"ext rstn reboot",		//CMD_EXT_RSTN_REBOOT_MODE,
	"iq reboot",			//CMD_IQ_REBOOT_MODE,
	"sleep",				//CMD_SLEEP_MODE,
};

#define GET_RST_MODE(x) rstmode[(x) < 17 ? (x) : 0]

#ifndef CONFIG_EMMC_BOOT
/*Copy the data saved in nand flash to ram*/
int read_nand_to_ram( struct mtd_info *mtd, loff_t paddr, unsigned int size, unsigned char *buf)
{
        int ret = 0;
        unsigned int retlen = 0;
        loff_t read_addr = 0;
        unsigned char *read_buf = NULL;
        unsigned int readsize = 0;

        debugf("%s, read 0x%.8x:0x%.8x buf: 0x%.8x\n", __func__, (unsigned int)paddr, size, buf);
        for(read_addr = paddr, read_buf = buf; read_addr < (paddr + size); read_addr += readsize, read_buf += readsize) {
                readsize = (paddr + size - read_addr) > mtd->erasesize ? mtd->erasesize : (paddr + size - read_addr);
                if(mtd->block_isbad(mtd, read_addr) == 1) {//if met bad block, we just fill it with 0x5a
                        memset(read_buf, 0x5a, readsize);
                        continue;
                }

                ret = mtd->read(mtd, read_addr, readsize, &retlen, read_buf);
                if(ret != 0 && retlen != readsize) {
                        printf("%s, read addr: 0x%.8x len: 0x%.8x 's value err, ret: %d, retlen: 0x%.8x\n",\
			 __func__, (unsigned int)read_addr, readsize, ret, retlen);
			lcd_printf("\nRead nand flash 0x%.8x error, you can dump it use download tools again!\n", read_addr);
//			lcd_display();
			break;
                }
        }
	return ret;
}

/*dump the data saved in nand flash to sdcard when needed*/
void mtd_dump(void)
{
	int ret = 0;
	unsigned int write_len = 0, write_addr = 0;
	char *buf = NULL;
	unsigned int part_len = 0x8000000;//The size of each ubipac-part file
	int loop = 0;
	char fname[72];
	struct mtd_info *mtd = NULL;

	buf = CONFIG_SYS_SDRAM_BASE;//After dump memory to sdcard, we suppose the whole memory except u-boot used are avaliable.
	mtd = get_mtd_device_nm(UBIPAC_PART);
	if(mtd == NULL) {
		printf("Can't get the mtd part: %s\n", UBIPAC_PART);
		return;
	}

	debugf("Begin to dump 0x%.8x ubipac to sdcard!\n", mtd->size);
	for(write_addr = 0; write_addr < mtd->size; write_addr += write_len, loop++)
	{
		write_len = (mtd->size - write_addr) > part_len ? part_len : (mtd->size - write_addr);
		debugf("begin to read 0x%.8x value to ubipac%d\n", write_len, loop);
		memset(buf, 0, write_len);
		ret = read_nand_to_ram(mtd, (loff_t)(write_addr), write_len, buf);
		if(ret != 0) {
			printf("%s, read ubipac%d error, the ret is %d\n", __func__, loop, ret);
			break;
		}
		debugf("read ubipac%d end\n", loop);

		memset(fname, 0, 72);
		sprintf(fname, "ubipac%d", loop);
		write_mem_to_mmc(NULL, fname, FS_FAT32, buf, write_len);
		debugf("write ubipac%d end\n", loop);
	}
	put_mtd_device(mtd);
}
#endif

void write_sysdump_before_boot(int rst_mode)
{
	int i, j, len, fs_type;
	char fnbuf[72], *path;
	struct rtc_time rtc;
	char *waddr;
	struct sysdump_mem *mem;
	struct sysdump_info *infop = (struct sysdump_info *)SPRD_SYSDUMP_MAGIC;

	struct sysdump_mem sprd_dump_mem[] = {
		{
			.paddr = CONFIG_PHYS_OFFSET,
			.vaddr = PAGE_OFFSET,
			.soff = 0xffffffff,
			.size = get_real_ram_size(),
			.type = SYSDUMP_RAM,
		},
	};

	int sprd_dump_mem_num = 1;

	debugf("rst_mode:0x%x, Check if need to write sysdump info of 0x%08lx to file...\t", rst_mode,
		SPRD_SYSDUMP_MAGIC);

	if ((rst_mode == CMD_WATCHDOG_REBOOT) || (rst_mode == CMD_UNKNOW_REBOOT_MODE) || (rst_mode == CMD_EXT_RSTN_REBOOT_MODE) || \
		((rst_mode == CMD_PANIC_REBOOT) && !memcmp(infop->magic, SYSDUMP_MAGIC, sizeof(infop->magic))) || (rst_mode == CMD_SPECIAL_MODE) ) {
		debugf("\n");

		memset(infop->magic, 0, sizeof(infop->magic));

		if(-1 == sysdump_flag_check()) {
			debugf("skip sysdump because sysdump_flag is close.\n");
			goto FINISH;
		}
		//MMU_DisableIDCM();
		if (init_mmc_fat(&fs_type))
			goto FINISH;

		/* display on screen */
		display_writing_sysdump();
		#define CONSOLE_COLOR_RED (0x1f<<11)
		#define CONSOLE_COLOR_GREEN 0x07e0
	//	lcd_setfgcolor(CONSOLE_COLOR_GREEN);
		lcd_printf("\nReset mode: %s\n\n",GET_RST_MODE(rst_mode));
	//	lcd_setfgcolor(CONSOLE_COLOR_RED);
//		lcd_display();

		int mod, key_code;
		if (fs_type==FS_FAT32)
			mod = fat_checksys(sprd_dump_mem[0].size);
		else if (fs_type==FS_EXFAT)
			mod = exfat_checksys(sprd_dump_mem[0].size);
		if (mod & FSSMSIZE) {
			debugf("SD card volume size is smaller then ram size. Skip sysdump.\n");
			goto FINISH;
		}
		if (mod &(FSFATAL | FSNOSPACE)) {
			if (mod & FSFATAL)
				lcd_printf("\ndetect sd is demaged");
			else if (mod & FSNOSPACE)
				lcd_printf("\ndetect sd have not enough space");

			lcd_printf("\npress volumedown to format sd\notherwise press volumeup to skip sysdump\n");

			do {
				udelay(50 * 1000);
				key_code = board_key_scan();
				if (key_code == KEY_VOLUMEDOWN) {
					lcd_printf("\nvolumedown key is pressed.\nSD Card format starts........\n");
					int ret;
					if (fs_type==FS_FAT32)
						ret = fat_format();
					else if (fs_type==FS_EXFAT)
						ret = exfat_format();
					if (ret != 0) {
						lcd_printf("\nSD Card format failed. Skip sysdump.\n");
						goto FINISH;
					}
					debugf("SD Card formatting done.\n");
					break;
				} else if(key_code == KEY_VOLUMEUP) {
					lcd_printf("\nvolumedup key is pressed. sysdump will be skipped.\n");
					goto FINISH;
				}
			} while (1);
		}

		if ((rst_mode == CMD_WATCHDOG_REBOOT) || (rst_mode == CMD_UNKNOW_REBOOT_MODE) ||
			(rst_mode == CMD_EXT_RSTN_REBOOT_MODE) || (rst_mode == CMD_SPECIAL_MODE)) {
			infop->dump_path[0] = '\0';
			infop->mem_num = sprd_dump_mem_num;
			infop->dump_mem_paddr = (unsigned long)sprd_dump_mem;
			strcpy(infop->time, "hw_watchdog");
			infop->elfhdr_size = get_elfhdr_size(infop->mem_num);
			infop->crash_key = 0;

			sysdump_fill_core_hdr(NULL,
					sprd_dump_mem,
					sprd_dump_mem_num,
					(char *)infop + sizeof(*infop),
					infop->mem_num + 1,
					infop->elfhdr_size);
		}

		if (strlen(infop->dump_path))
			path = infop->dump_path;
		else
			path = NULL;

		sprintf(fnbuf, SYSDUMP_CORE_NAME_FMT, 0);
		write_mem_to_mmc(path, fnbuf, fs_type,
			(char *)infop + sizeof(*infop), infop->elfhdr_size);

#if 1 /* TODO: jianjun.he */
		mem = (struct sysdump_mem *)infop->dump_mem_paddr;
		for (i = 0; i < infop->mem_num; i++) {
			if (0xffffffff != mem[i].soff)
				waddr = (char *)infop + sizeof(*infop) + 
						infop->elfhdr_size + mem[i].soff;
			else
				waddr = mem[i].paddr;

		#ifdef CONFIG_RAMDUMP_NO_SPLIT
				sprintf(fnbuf, SYSDUMP_CORE_NAME_FMT"_0x%8.8x-0x%8.8x_dump.lst", i + 1, mem[i].paddr, mem[i].paddr + mem[i].size -1);
				write_mem_to_mmc(path, fnbuf, fs_type, waddr, mem[i].size);
		#else
			if (mem[i].size <= SZ_8M) {
				sprintf(fnbuf, SYSDUMP_CORE_NAME_FMT, /*infop->time,*/ i + 1);
				write_mem_to_mmc(path, fnbuf, fs_type, waddr, mem[i].size);
			} else {
				for (j = 0; j < mem[i].size / SZ_8M; j++) {
					sprintf(fnbuf, SYSDUMP_CORE_NAME_FMT"_%03d",
						/*infop->time,*/ i + 1, j);
					write_mem_to_mmc(path, fnbuf, fs_type, waddr + j * SZ_8M, SZ_8M);
				}

				if (mem[i].size % SZ_8M) {
					sprintf(fnbuf, SYSDUMP_CORE_NAME_FMT"_%03d",
						/*infop->time,*/ i + 1, j);
					write_mem_to_mmc(path, fnbuf, fs_type, waddr + j * SZ_8M,
									(mem[i].size % SZ_8M));
				}
			}
		#endif
		}
#else
		for (i = 0; i < infop->mem_num; i++) {
			sprintf(fnbuf, SYSDUMP_CORE_NAME_FMT, /*infop->time,*/ i + 1);
			write_mem_to_mmc(path, fnbuf, fs_type, mem[i].paddr, mem[i].size);
		}
#endif

#ifndef CONFIG_EMMC_BOOT
		lcd_printf("\nBegin to dump nand flash:\n");
//		lcd_display();
		mtd_dump();
#endif
		debugf("\nwriting done.\nPress any key to continue...");
		lcd_printf("\nWriting done.\nPress any key (Exp power key) to continue...");
//		lcd_display();
		wait_for_keypress();
	} else
		debugf("no need.\n");

FINISH:
	return;
}




