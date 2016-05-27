#include <common.h>
#include <stdio_dev.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <asm/global_data.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <exports.h>
#include "loader_common.h"
#include "sprd_cpcmdline.h"

extern void *lcd_get_base_addr(void *lcd_base);
extern uint32_t load_lcd_id_to_kernel();
extern int get_dram_cs_number(void);
extern int get_dram_cs0_size(void);

#ifdef CONFIG_SECURE_BOOT
#define CMDLINE_BUF_SIZE	(1024)
unsigned spl_data_buf[0x2000] __attribute__ ((align(4))) = {
0};

unsigned harsh_data_buf[8] __attribute__ ((align(4))) = {
0};

void *spl_data = spl_data_buf;
void *harsh_data = harsh_data_buf;
#endif

int fdt_initrd_norsvmem(void *fdt, ulong initrd_start, ulong initrd_end, int force)
{
	int nodeoffset;
	int err;
	u32 tmp;
	const char *path;
	uint64_t addr, size;

	/* Find the "chosen" node.  */
	nodeoffset = fdt_path_offset(fdt, "/chosen");

	/* If there is no "chosen" node in the blob return */
	if (nodeoffset < 0) {
		printf("fdt_initrd: %s\n", fdt_strerror(nodeoffset));
		return nodeoffset;
	}

	/* just return if initrd_start/end aren't valid */
	if ((initrd_start == 0) || (initrd_end == 0))
		return 0;

	path = fdt_getprop(fdt, nodeoffset, "linux,initrd-start", NULL);
	if ((path == NULL) || force) {
		tmp = __cpu_to_be32(initrd_start);
		err = fdt_setprop(fdt, nodeoffset, "linux,initrd-start", &tmp, sizeof(tmp));
		if (err < 0) {
			printf("WARNING: " "could not set linux,initrd-start %s.\n", fdt_strerror(err));
			return err;
		}
		tmp = __cpu_to_be32(initrd_end);
		err = fdt_setprop(fdt, nodeoffset, "linux,initrd-end", &tmp, sizeof(tmp));
		if (err < 0) {
			printf("WARNING: could not set linux,initrd-end %s.\n", fdt_strerror(err));

			return err;
		}
	}

	return 0;
}

int fdt_chosen_bootargs_append(void *fdt, char *append_args, int force)
{
	int nodeoffset;
	int err;
	const char *path;
	char *strargs;

	if (!append_args)
		return -1;

	err = fdt_check_header(fdt);
	if (err < 0) {
		printf("fdt_chosen_bootargs_append: %s\n", fdt_strerror(err));
		return err;
	}

	/*
	 * Find the "chosen" node.
	 */
	nodeoffset = fdt_path_offset(fdt, "/chosen");

	/*
	 * If there is no "chosen" node in the blob, leave.
	 */
	if (nodeoffset < 0) {
		printf("fdt_chosen_bootargs_append: cann't find chosen");
		return -1;
	}

	/*
	 * If the property exists, update it only if the "force" parameter
	 * is true.
	 */
	path = fdt_getprop(fdt, nodeoffset, "bootargs", NULL);
	if ((path == NULL) || force) {
		strargs = malloc(1024);
		if (!strargs)
			return -1;
		memset(strargs, 0, 1024);
		if(path != NULL)
		{
		    sprintf(strargs, "%s %s", path, append_args);
		}
		else
		{
		    sprintf(strargs, "%s", append_args);
		}
		err = fdt_setprop(fdt, nodeoffset, "bootargs", strargs, strlen(strargs) + 1);
		if (err < 0)
			printf("WARNING: could not set bootargs %s.\n", fdt_strerror(err));
		free(strargs);
	}

	return err;
}

int fdt_chosen_bootargs_replace(void *fdt, char *old_args, char *new_args)
{
	int nodeoffset;
	int err, i;
	char *str, *src, *dst;
	const char *path;
	char *strargs;

	if (!old_args || !new_args)
		return -1;
	printf("fdt_chosen_bootargs_replace start!");

	err = fdt_check_header(fdt);
	if (err < 0) {
		printf("fdt_chosen_bootargs_replace: %s\n", fdt_strerror(err));
		return err;
	}

	/*
	 * Find the "chosen" node.
	 */
	nodeoffset = fdt_path_offset(fdt, "/chosen");

	/*
	 * If there is no "chosen" node in the blob, leave.
	 */
	if (nodeoffset < 0) {
		printf("fdt_chosen_bootargs_replace: cann't find chosen");
		return -1;
	}

	/*
	 * If the property exists, update it only if the "force" parameter
	 * is true.
	 */
	path = fdt_getprop(fdt, nodeoffset, "bootargs", NULL);
	if (path != NULL ) {

        printf("fdt_chosen_bootargs_replace: old bootargs %s!", path);
        str = strstr(path, old_args);
        if(!str)
        {
            printf("fdt_chosen_bootargs_replace: cann't find str %s!", old_args);
            return -1;
        }
		strargs = malloc(1024);
		if (!strargs)
			return -1;
        src = path;
        dst = strargs;
        i = 0;
        /* copy the front str */
        while(src != str && i < 1023){
            *dst++ = *src++;
            i++;
        }

        /* copy the new str */
        src = new_args;
        while(*src && i < 1023){
            *dst++ = *src++;
            i++;
        }

        /* copy the back str */
        src = str + strlen(old_args);
        while(*src && i < 1023){
            *dst++ = *src++;
            i++;
        }
        *dst = 0;

        printf("fdt_chosen_bootargs_replace: new bootargs %s!", strargs);
		err = fdt_setprop(fdt, nodeoffset, "bootargs", strargs, strlen(strargs) + 1);
		if (err < 0)
			printf("WARNING: could not set bootargs %s.\n", fdt_strerror(err));
		free(strargs);
	}

	return err;
}

#if defined( CONFIG_KERNEL_BOOT_CP )
int fdt_fixup_cp_boot(void *fdt)
{
	char buf[64];
	int str_len;
	int ret = 0;
	memset(buf, 0, 64);
	sprintf(buf,"modem=shutdown");
	str_len = strlen(buf);
	buf[str_len] = '\0';
	ret = fdt_chosen_bootargs_append(fdt, buf, 1);

	ret += fdt_chosen_bootargs_append(fdt, cp_getcmdline(), 1);

	/* if is uart calibraton, remove ttys1 console */
	if(is_calibration_by_uart())
	{
		//__raw_writel(0x285480, CTL_PIN_BASE + REG_PIN_CTRL2);
		//ret = fdt_chosen_bootargs_replace(fdt,"console=ttyS1", "console=ttyS3");
		ret = fdt_chosen_bootargs_replace(fdt,"console=ttyS1", "console=null");
	}

	return 0;
}

#else
int fdt_fixup_calibration_parameter(void *fdt)
{
	char *buf;
	int ret;

	buf = get_calibration_parameter();
	if (NULL == buf) {
		printf("warn:nothing transfer to kernel about calibration\n");
		return -1;
	} else {
		printf("in function fdt_fixup_calibration_parameter buf = %s", buf);
		ret = fdt_chosen_bootargs_append(fdt, buf, 1);
		if(0 ==ret)
		{
			/* if is uart calibraton, remove ttys1 console */
			if(is_calibration_by_uart())
			{
				//__raw_writel(0x285480, CTL_PIN_BASE + REG_PIN_CTRL2);
				//ret = fdt_chosen_bootargs_replace(fdt,"console=ttyS1", "console=ttyS3");
				ret = fdt_chosen_bootargs_replace(fdt,"console=ttyS1", "console=null");
			}
		}
		return ret;
	}
}

int fdt_fixup_boot_mode(void *fdt)
{
	char buf[128];
	int str_len;
	int ret;
	char *boot_mode;

	memset(buf, 0, 128);
	boot_mode = getenv("bootmode");
	if (NULL != boot_mode) {
		sprintf(buf, "androidboot.mode=%s", boot_mode);
		str_len = strlen(buf);
		buf[str_len] = '\0';
		debugf("Fixed bootmode:%s\n", buf);
	} else {
		debugf("Normal mode been set\n");
	}

	ret = fdt_chosen_bootargs_append(fdt, buf, 1);
	return ret;
}
#endif

int fdt_fixup_mtd(void *fdt)
{
	char buf[128];
	int str_len;
	int ret;

	memset(buf, 0, 128);

	sprintf(buf, MTDPARTS_DEFAULT);
	str_len = strlen(buf);
	buf[str_len] = '\0';

	ret = fdt_chosen_bootargs_append(fdt, buf, 1);
	return ret;
}

int fdt_fixup_lcdid(void *fdt)
{
	char buf[48];

	uint32_t lcd_id = 0;
	int str_len;
	int ret;

	lcd_id = load_lcd_id_to_kernel();
	memset(buf, 0, 48);

	sprintf(buf, "lcd_id=ID");
	str_len = strlen(buf);
	sprintf(&buf[str_len], "%x", lcd_id);
	str_len = strlen(buf);
	buf[str_len] = '\0';

	#ifdef CONFIG_LCD_COMPATIBLE_BY_READ_ID_VOLTAGE //GST huafeizhou140324 add-s	
	extern uint32_t load_lcd_vol_type_to_kernel();
	uint32_t lcd_vol_type;
	
	lcd_vol_type = load_lcd_vol_type_to_kernel();
	//add lcd vol type
	//if(lcd_vol_type) 
	{
		str_len = strlen(buf);
		sprintf(&buf[str_len], " lcd_vol_type=VT");
		str_len = strlen(buf);
		sprintf(&buf[str_len], "%x",lcd_vol_type);
		str_len = strlen(buf);
		buf[str_len] = '\0';
	}		
	#endif //GST huafeizhou140324 add-e

	ret = fdt_chosen_bootargs_append(fdt, buf, 1);
	return ret;
}

int fdt_fixup_lcdbase(void *fdt)
{
	char buf[32];
	int str_len;
	int ret;
	void *lcd_base;

	memset(buf, 0, 32);

	//add lcd frame buffer base, length should be lcd w*h*2(RGB565)
	lcd_base = lcd_get_base_addr(NULL);
	sprintf(buf, "lcd_base=");
	str_len = strlen(buf);
	sprintf(&buf[str_len], "%x", lcd_base);
	str_len = strlen(buf);
	buf[str_len] = '\0';

	ret = fdt_chosen_bootargs_append(fdt, buf, 1);
	return ret;
}

int fdt_fixup_serialno(void *fdt)
{
	char buf[64];
	int str_len;
	int ret;
	memset(buf, 0, 64);

	sprintf(buf, " androidboot.serialno=%s", get_product_sn());
	str_len = strlen(buf);
	buf[str_len] = '\0';
	ret = fdt_chosen_bootargs_append(fdt, buf, 1);
	return ret;
}

int fdt_fixup_adc_calibration_data(void *fdt)
{
	//extern unsigned int *adc_data_to_transfer;
	unsigned int *adc_data = malloc(64);
	int ret = read_adc_calibration_data(adc_data, 48);

	if (ret) {
		char buf[64];
		int str_len;
		int ret;

		memset(buf, 0, 64);
		if (((adc_data[2] & 0xffff) < 4500) && ((adc_data[2] & 0xffff) > 3000) && ((adc_data[3] & 0xffff) < 4500) && ((adc_data[3] & 0xffff) > 3000)) {
			str_len = strlen(buf);
			sprintf(&buf[str_len], " adc_cal=%d,%d", adc_data[2], adc_data[3]);
		}
		/*just after fgu adc calibration,and no aux adc calibration,need save fgu adc parameters */
		if ((0x00000002 == adc_data[10]) && (0x00000002 & adc_data[11])) {
			str_len = strlen(buf);
			sprintf(&buf[str_len], " fgu_cal=%d,%d,%d", adc_data[4], adc_data[5], adc_data[6]);
		}

		str_len = strlen(buf);
		buf[str_len] = '\0';
		ret = fdt_chosen_bootargs_append(fdt, buf, 1);
	} else {
		printf("read_adc_calibration_data failed\n");
	}
	free(adc_data);
	return ret;
}

int fdt_fixup_dram_training(void *fdt)
{
	char buf[64];
	int str_len;
	int ret;
	uint64_t addr;
	uint64_t size;
	size = 0x1000;
	memset(buf, 0, 64);

	sprintf(buf, " mem_cs=%d, mem_cs0_sz=%08x", get_dram_cs_number(), get_dram_cs0_size());
	str_len = strlen(buf);
	buf[str_len] = '\0';
	ret = fdt_chosen_bootargs_append(fdt, buf, 1);
	printf(" mem_cs=%d, mem_cs0_sz=%08x", get_dram_cs_number(), get_dram_cs0_size());

	/* gerenally, reserved memory should be configured in dts file. Add fixup here to avoid the wide
	 * range of change for various boards.
	 * reserved for ddr training data used
	 */
	if (ret < 0)
		printf("failed to append ddr training data args bootargs\n");
	else {
		addr = PHYS_SDRAM_1;
		ret = fdt_add_mem_rsv(fdt, addr, size);
		if(ret < 0) {
			printf("failed to reserved cs0 ddr training data space\n");
		}
		else {
			if(2 == get_dram_cs_number()) {
				addr = PHYS_SDRAM_1 + get_dram_cs0_size();
				ret = fdt_add_mem_rsv(fdt, addr, size);
				if(ret < 0) {
					printf("failed to reserved cs1 ddr training data space\n");
				}
			}
		}
	}

	return ret;
}

/**
 * You can re-define function void fdt_fixup_chosen_bootargs_board(char *buf, const char *boot_mode, int calibration_mode)
 * in your u-boot/board/spreadtrum/xxx/xxx.c to override this default function
 */
void __attribute__ ((weak)) fdt_fixup_chosen_bootargs_board(char *buf, int calibration_mode)
{
}

int fdt_fixup_chosen_bootargs_board_private(void *fdt)
{
	int ret = 0;
	char buf[512];
	memset(buf, 0, sizeof buf);
	fdt_fixup_chosen_bootargs_board(buf, poweron_by_calibration());
	if (buf[0])
		ret = fdt_chosen_bootargs_append(fdt, buf, 1);
	return ret;
}

#ifdef CONFIG_SECURE_BOOT
static void convertBinToHex(char *bin_ptr, int length, char *hex_ptr)
{
    int i;
    unsigned char tmp;

    if (bin_ptr == NULL || hex_ptr == NULL) {
        return;
    }
    for (i=0; i<length; i++) {
        tmp = (unsigned char)((bin_ptr[i] & 0xf0)>>4);
        if (tmp <= 9) {
            *hex_ptr = (unsigned char)(tmp + '0');
        } else {
            *hex_ptr = (unsigned char)(tmp + 'A' - 10);
        }
        hex_ptr++;
        tmp = (unsigned char)(bin_ptr[i] & 0x0f);
        if (tmp <= 9) {
            *hex_ptr = (unsigned char)(tmp + '0');
        } else {
            *hex_ptr = (unsigned char)(tmp + 'A' - 10);
        }
        hex_ptr++;
    }
}

void fdt_fixup_secureboot_param(void *fdt_blob)
{

	uint8_t *buf = NULL;
	unsigned int str_len = 0;
	buf = malloc(CMDLINE_BUF_SIZE);
	memset(buf, 0, CMDLINE_BUF_SIZE);
	uint8_t *pubk = NULL;
	pubk = malloc(512);
	int i = 0;
#ifdef PRIMPUKPATH
	str_len = strlen(buf);
	sprintf(&buf[str_len], " primpukpath=%s", PRIMPUKPATH);	//"/dev/block/mmcblk0boot0");
	str_len = strlen(buf);
	buf[str_len] = '\0';
#endif
#ifdef PRIMPUKSTART
	str_len = strlen(buf);
	sprintf(&buf[str_len], " primpukstart=%d", PRIMPUKSTART);	//512);
	str_len = strlen(buf);
	buf[str_len] = '\0';
#endif
#ifdef PRIMPUKLEN
	str_len = strlen(buf);
	sprintf(&buf[str_len], " primpuklen=%d", PRIMPUKLEN);	//260);
	str_len = strlen(buf);
	buf[str_len] = '\0';
#endif
	/*hash write by pc tool,but the hash value calculated by u-boot */
	/*if rom secure enable,do not need cal spl hash and pass to kernel */
	if (!secureboot_enabled()) {
		int ret = get_spl_hash(harsh_data);
		if (ret) {
			str_len = strlen(buf);
			sprintf(&buf[str_len], " securesha1=%08x%08x%08x%08x%08x", *(uint32_t *) harsh_data, *(uint32_t *) (harsh_data + 4),
				*(uint32_t *) (harsh_data + 8), *(uint32_t *) (harsh_data + 12), *(uint32_t *) (harsh_data + 16));
			str_len = strlen(buf);
			buf[str_len] = '\0';
		}
	}
#ifdef  CONFIG_EMMC_BOOT
	if (TRUE != Emmc_Read(2 /* PARTITION_BOOT2 */ , PUBKEY_VLR_BLOCK_INDEX,
					      PUBKEY_READ_BLOCK_NUMS, (uint8_t *) pubk)) {
				printf("PARTITION_BOOT2 read error \n");
				return 0;
			}
#endif

	str_len = strlen(buf);
	sprintf(&buf[str_len]," pubk=");
	str_len = strlen(buf);
	buf[str_len] = '\0';

	str_len = strlen(buf);

	convertBinToHex(pubk, 260, &buf[str_len]);

	fdt_chosen_bootargs_append(fdt_blob, buf, 1);
}

#endif

/**
 * Fix me:
 * fix_memory_size() interface which can be fixup memory size, but because of our
 * device tree is not suitable for the context, for simple, we add a new function. In arm64,
 * we should discard it. It should be ok now.
 */
int fdt_fixup_ddr_size(void *fdt)
{
#ifdef CONFIG_DDR_AUTO_DETECT
	int nodeoffset;
	int err;
	const char *path;
	u32 val = CONFIG_SYS_SDRAM_BASE;


	nodeoffset = fdt_path_offset(fdt, "/memory");
	printf("nodeoffset = %d\n", nodeoffset);
	if (nodeoffset < 0) {
	    printf("ERROR: device tree must have /memory node %s.\n", fdt_strerror(nodeoffset));
	    return nodeoffset;
	}

	fdt_delprop(fdt, nodeoffset, "reg");

	err = fdt_setprop_u32(fdt, nodeoffset, "reg", val);
	if (err < 0) {
	    printf("ERROR: cannot set /memory node's reg property(addr)!\n");
	    return err;
	}

	err = fdt_appendprop_u32(fdt, nodeoffset, "reg", (u32)get_real_ram_size());
	//err = fdt_appendprop_u32(fdt, nodeoffset, "reg", 0x40000000);
	if (err < 0) {
	    printf("ERROR: cannot set /memory node's reg property(size)!\n");
	}

	return err;
#else
	return 0;
#endif
}


