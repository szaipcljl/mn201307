#include <common.h>
#include "loader_common.h"
#include <malloc.h>
#include <mmc.h>
#include <ext_common.h>
#include <ext4fs.h>
#include <asm/sizes.h>

#ifdef CONFIG_ARM7_RAM_ACTIVE
    extern void pmic_arm7_RAM_active(void);
#endif

#ifdef CONFIG_MINI_TRUSTZONE
#include "trustzone_def.h"
#endif

#ifdef CONFIG_SECURE_BOOT
#include "secure_boot.h"
#include "secure_verify.h"
#include <linux/types.h>
#endif

#ifdef CONFIG_OF_LIBFDT
#include "dev_tree.h"
#endif
#ifdef CONFIG_SUPPORT_TDLTE
static boot_image_required_t const s_boot_image_tl_table[] = {
#if !defined( CONFIG_KERNEL_BOOT_CP )
	{"tl_fixnv1", "tl_fixnv2", LTE_FIXNV_SIZE, LTE_FIXNV_ADDR},
	{"tl_runtimenv1", "tl_runtimenv2", LTE_RUNNV_SIZE, LTE_RUNNV_ADDR},
	{"tl_modem", NULL, LTE_MODEM_SIZE, LTE_MODEM_ADDR},
	{"tl_ldsp", NULL, LTE_LDSP_SIZE, LTE_LDSP_ADDR},	//ltedsp
	{"tl_tgdsp", NULL, LTE_GDSP_SIZE, LTE_GDSP_ADDR},
#endif
	{NULL, NULL, 0, 0}
};
#endif

#ifdef CONFIG_SUPPORT_WLTE
static boot_image_required_t const s_boot_image_wl_table[] = {
#if !defined( CONFIG_KERNEL_BOOT_CP )
	{"wl_fixnv1", "wl_fixnv2", LTE_FIXNV_SIZE, LTE_FIXNV_ADDR},
	{"wl_runtimenv1", "wl_runtimenv2", LTE_RUNNV_SIZE, LTE_RUNNV_ADDR},
	{"wl_modem", NULL, LTE_MODEM_SIZE, LTE_MODEM_ADDR},
	{"wl_ldsp", NULL, LTE_LDSP_SIZE, LTE_LDSP_ADDR},
	{"wl_gdsp", NULL, LTE_GDSP_SIZE, LTE_GDSP_ADDR},
	{"wl_warm", NULL, WL_WARM_SIZE, WL_WARM_ADDR},
#endif
	{NULL, NULL, 0, 0}
};
#endif

#ifdef CONFIG_SUPPORT_GSM
static boot_image_required_t const s_boot_image_gsm_table[] = {
#if !defined( CONFIG_KERNEL_BOOT_CP )
	{"g_fixnv1", "g_fixnv2", GSM_FIXNV_SIZE, GSM_FIXNV_ADDR},
	{"g_runtimenv1", "g_runtimenv2", GSM_RUNNV_SIZE, GSM_RUNNV_ADDR},
	{"g_modem", NULL, GSM_MODEM_SIZE, GSM_MODEM_ADDR},
	{"g_dsp", NULL, GSM_DSP_SIZE, GSM_DSP_ADDR},
#endif
	{NULL, NULL, 0, 0}
};
#endif

#ifdef CONFIG_SUPPORT_LTE
static boot_image_required_t const s_boot_image_lte_table[] = {
#if !defined( CONFIG_KERNEL_BOOT_CP )
	{"l_fixnv1", "l_fixnv2", LTE_FIXNV_SIZE, LTE_FIXNV_ADDR},
	{"l_runtimenv1", "l_runtimenv2", LTE_RUNNV_SIZE, LTE_RUNNV_ADDR},
	{"l_modem", NULL, LTE_MODEM_SIZE, LTE_MODEM_ADDR},
	{"l_ldsp", NULL, LTE_LDSP_SIZE, LTE_LDSP_ADDR},
	{"l_gdsp", NULL, LTE_GDSP_SIZE, LTE_GDSP_ADDR},
	{"l_warm", NULL, WL_WARM_SIZE, WL_WARM_ADDR},
#endif
	{NULL, NULL, 0, 0}
};
#endif

#ifdef CONFIG_SUPPORT_TD
static boot_image_required_t const s_boot_image_TD_table[] = {
#if !defined( CONFIG_KERNEL_BOOT_CP )
	{"tdfixnv1", "tdfixnv2", FIXNV_SIZE, TDFIXNV_ADR},
	{"tdruntimenv1", "tdruntimenv2", RUNTIMENV_SIZE, TDRUNTIMENV_ADR},
	{"tdmodem", NULL, TDMODEM_SIZE, TDMODEM_ADR},
	{"tddsp", NULL, TDDSP_SIZE, TDDSP_ADR},
#endif
	{NULL, NULL, 0, 0}
};
#endif

#ifdef CONFIG_SUPPORT_W
static boot_image_required_t const s_boot_image_W_table[] = {
#if !defined( CONFIG_KERNEL_BOOT_CP )
	{"wfixnv1", "wfixnv2", FIXNV_SIZE, WFIXNV_ADR},
	{"wruntimenv1", "wruntimenv2", RUNTIMENV_SIZE, WRUNTIMENV_ADR},
	{"wmodem", NULL, WMODEM_SIZE, WMODEM_ADR},
	{"wdsp", NULL, WDSP_SIZE, WDSP_ADR},
#endif
	{NULL, NULL, 0, 0}
};
#endif

#ifdef CONFIG_SUPPORT_WIFI
static boot_image_required_t const s_boot_image_WIFI_table[] = {
	{"wcnfixnv1", "wcnfixnv2", FIXNV_SIZE, WCNFIXNV_ADR},
	{"wcnruntimenv1", "wcnruntimenv2", RUNTIMENV_SIZE, WCNRUNTIMENV_ADR},
	{"wcnmodem", NULL, WCNMODEM_SIZE, WCNMODEM_ADR},
	{NULL, NULL, 0, 0}
};
#endif

static boot_image_required_t const s_boot_image_COMMON_table[] = {
#ifdef CONFIG_SIMLOCK_ENABLE
	{"simlock", NULL, SIMLOCK_SIZE, SIMLOCK_ADR},
#endif
#ifdef CONFIG_DFS_ENABLE
#if !defined( CONFIG_KERNEL_BOOT_CP )
	{"pm_sys", NULL, DFS_SIZE, DFS_ADDR},
#endif
#endif
	{NULL, NULL, 0, 0}

};

#ifdef CONFIG_MINI_TRUSTZONE
static boot_image_required_t const s_boot_image_TZ_table[] = {
	{"sml", NULL, TRUSTRAM_SIZE, TRUSTRAM_ADR},
	{NULL, NULL, 0, 0}

};
#endif

static boot_image_required_t *const s_boot_image_table[] = {
#ifdef CONFIG_SUPPORT_TDLTE
	s_boot_image_tl_table,
#endif

#ifdef CONFIG_SUPPORT_WLTE
	s_boot_image_wl_table,
#endif

#ifdef CONFIG_SUPPORT_LTE
	s_boot_image_lte_table,
#endif

#ifdef CONFIG_SUPPORT_GSM
	s_boot_image_gsm_table,
#endif

#ifdef CONFIG_SUPPORT_TD
	s_boot_image_TD_table,
#endif

#ifdef CONFIG_SUPPORT_W
	s_boot_image_W_table,
#endif

#ifdef CONFIG_SUPPORT_WIFI
	s_boot_image_WIFI_table,
#endif
#ifdef CONFIG_MINI_TRUSTZONE
	s_boot_image_TZ_table,
#endif
	s_boot_image_COMMON_table,

	0
};

#ifdef CONFIG_SECURE_BOOT
uint8_t header_buf[SEC_HEADER_MAX_SIZE];
#endif
int read_logoimg(char *bmp_img, size_t size)
{
	block_dev_desc_t *p_block_dev = NULL;
	disk_partition_t info;

	p_block_dev = get_dev("mmc", 0);
	if (NULL == p_block_dev)
		return -1;

	if (!get_partition_info_by_name(p_block_dev, "logo", &info)) {
		if (FALSE == Emmc_Read(PARTITION_USER, info.start, size / EMMC_SECTOR_SIZE, bmp_img)) {
			debugf("function: %s nand read error\n");
			return -1;
		}
		debugf("read logo partition OK!\n");
	} else {
		debugf("no logo partition , no need to display\n");
		return -1;
	}
	return 0;
}

#ifdef CONFIG_SECURE_BOOT

int get_spl_hash(void *hash_data)
{
	NBLHeader *header;
	unsigned int len;
	uint8_t *spl_data;
	int ret = 0;
	int size = CONFIG_SPL_HASH_LEN;
	spl_data = malloc(size);
	if (!spl_data) {
		return ret;
	}

	if (FALSE == Emmc_Read(PARTITION_BOOT1, 0, size / EMMC_SECTOR_SIZE, (uint8_t *) spl_data)) {
		debugf("PARTITION_BOOT1 read error \n");
		return ret;
	}

	header = (NBLHeader *) ((uint8_t *) spl_data + BOOTLOADER_HEADER_OFFSET);
	len = header->mHashLen;
	/*clear header */
	memset(header, 0, sizeof(NBLHeader));
	header->mHashLen = len;
	debugf("cal spl hash len=%d\n", header->mHashLen * 4);
	ret = cal_sha1(spl_data, (header->mHashLen) << 2, hash_data);

	if (spl_data)
		free(spl_data);

	return ret;
}
#endif
void _boot_secure_check(void)
{
#ifdef SECURE_BOOT_ENABLE
	secure_check(DSP_ADR, 0, DSP_ADR + DSP_SIZE - VLR_INFO_OFF, CONFIG_SYS_NAND_U_BOOT_DST + CONFIG_SYS_NAND_U_BOOT_SIZE - KEY_INFO_SIZ - VLR_INFO_OFF);
	secure_check(MODEM_ADR, 0, MODEM_ADR + MODEM_SIZE - VLR_INFO_OFF,
		     CONFIG_SYS_NAND_U_BOOT_DST + CONFIG_SYS_NAND_U_BOOT_SIZE - KEY_INFO_SIZ - VLR_INFO_OFF);
#ifdef CONFIG_SIMLOCK
	secure_check(SIMLOCK_ADR, 0, SIMLOCK_ADR + SIMLOCK_SIZE - VLR_INFO_OFF,
		     CONFIG_SYS_NAND_U_BOOT_DST + CONFIG_SYS_NAND_U_BOOT_SIZE - KEY_INFO_SIZ - VLR_INFO_OFF);
#endif
#endif
	return;
}

/**
	Function for reading user partition.
*/
int _boot_partition_read(block_dev_desc_t * dev, uchar * partition_name, uint32_t offsetsector, uint32_t size, uint8_t * buf)
{
	int ret = 0;
	uint32_t left;
	uint32_t nsct;
	char *sctbuf = NULL;
	disk_partition_t info;

	if (NULL == buf) {
		debugf("buf is NULL!\n");
		goto end;
	}
	nsct = size / EMMC_SECTOR_SIZE;
	left = size % EMMC_SECTOR_SIZE;

	if (get_partition_info_by_name(dev, partition_name, &info)) {
		errorf("get partition %s info failed!\n", partition_name);
		goto end;
	}

	if (FALSE == Emmc_Read(PARTITION_USER, info.start + offsetsector, nsct, buf))
		goto end;

	if (left) {
		sctbuf = malloc(EMMC_SECTOR_SIZE);
		if (NULL != sctbuf) {
			if (FALSE != Emmc_Read(PARTITION_USER, info.start + offsetsector + nsct, 1, sctbuf)) {
				memcpy(buf + (nsct * EMMC_SECTOR_SIZE), sctbuf, left);
				ret = 1;
			}
			free(sctbuf);
		}
	} else {
		ret = 1;
	}

end:
	debugf("partition %s read %s!\n", partition_name, ret ? "success" : "failed");
	return ret;
}

/**
	Function for writing user partition.
*/
int _boot_partition_write(block_dev_desc_t * dev, uchar * partition_name, u32 size, u8 * buf)
{
	disk_partition_t info;

	if (NULL == buf) {
		debugf("buf is NULL!\n");
		return 0;
	}
	size = (size + (EMMC_SECTOR_SIZE - 1)) & (~(EMMC_SECTOR_SIZE - 1));
	size = size / EMMC_SECTOR_SIZE;
	if (0 == get_partition_info_by_name(dev, partition_name, &info)) {
		if (TRUE != Emmc_Write(PARTITION_USER, info.start, size, buf)) {
			debugf("partition:%s read error!\n", partition_name);
			return 0;
		}
	} else {
		debugf("partition:%s >>>get partition info failed!\n", partition_name);
		return 0;
	}
	debugf("partition:%s write success!\n", partition_name);
	return 1;
}

int splash_screen_prepare(void)
{
	int ret;
	size_t size = SZ_1M;
	u8 *addr;
	u8 *s;

	s = getenv("splashimage");
	if (!s) {
		debugf("%s: failed to get env from splashimage\n");
		return -1;
	}
	addr = (u8 *) simple_strtoul(s, NULL, 16);
	ret = read_logoimg(addr, size);
	if (ret) {
		debugf("%s: failed to read logo partition\n");
		return ret;
	}

	return 0;
}

/**
	we assume partition with backup must check ecc.
*/
int _boot_read_partition_with_backup(block_dev_desc_t * dev, boot_image_required_t info)
{
	uint8_t *bakbuf = NULL;
	uint8_t *oribuf = NULL;
	uint8_t status = 0;
	uint8_t header[EMMC_SECTOR_SIZE];
	uint32_t checksum = 0;
	nv_header_t *header_p = NULL;
	uint32_t bufsize = info.size + EMMC_SECTOR_SIZE;

	header_p = header;
	bakbuf = malloc(bufsize);
	if (NULL == bakbuf) {
		debugf("bakbuf malloc fail\n");
		return 0;
	}
	memset(bakbuf, 0xff, bufsize);
	oribuf = malloc(bufsize);
	if (NULL == oribuf) {
		debugf("oribuf malloc fail\n");
		free(bakbuf);
		return 0;
	}
	memset(oribuf, 0xff, bufsize);
	if (_boot_partition_read(dev, info.partition, 0, info.size + EMMC_SECTOR_SIZE, oribuf)) {
		memset(header, 0, EMMC_SECTOR_SIZE);
		memcpy(header, oribuf, EMMC_SECTOR_SIZE);
		checksum = header_p->checksum;
		debugf("_boot_read_partition_with_backup origin checksum 0x%x\n", checksum);
		if (_chkNVEcc(oribuf + EMMC_SECTOR_SIZE, info.size, checksum)) {
			memcpy(info.mem_addr, oribuf + EMMC_SECTOR_SIZE, info.size);
			status += 1;
		}
	}
	if (_boot_partition_read(dev, info.bak_partition, 0, info.size + EMMC_SECTOR_SIZE, bakbuf)) {
		memset(header, 0, EMMC_SECTOR_SIZE);
		memcpy(header, bakbuf, EMMC_SECTOR_SIZE);
		checksum = header_p->checksum;
		debugf("_boot_read_partition_with_backup backup checksum 0x%x\n", checksum);
		if (_chkNVEcc(bakbuf + EMMC_SECTOR_SIZE, info.size, checksum))
			status += 1 << 1;
	}

	switch (status) {
	case 0:
		debugf("(%s)both org and bak partition are damaged!\n", info.partition);
		memset(info.mem_addr, 0, info.size);
		free(bakbuf);
		free(oribuf);
		return 0;
	case 1:
		debugf("(%s)bak partition is damaged!\n", info.bak_partition);
		_boot_partition_write(dev, info.bak_partition, info.size + EMMC_SECTOR_SIZE, oribuf);
		break;
	case 2:
		debugf("(%s)org partition is damaged!\n!", info.partition);
		memcpy(info.mem_addr, bakbuf + EMMC_SECTOR_SIZE, info.size);
		_boot_partition_write(dev, info.partition, info.size + EMMC_SECTOR_SIZE, bakbuf);
		break;
	case 3:
		debugf("(%s)both org and bak partition are ok!\n", info.partition);
		break;
	default:
		debugf("status error!\n");
		free(bakbuf);
		free(oribuf);
		return 0;
	}
	free(bakbuf);
	free(oribuf);
	return 1;
}

/**
	Function for reading image which is needed when power on.
*/
int _boot_load_required_image(block_dev_desc_t * dev, boot_image_required_t img_info)
{
	uint32_t secure_boot_offset = 0;
	debugf("load %s to addr 0x%08x\n", img_info.partition, img_info.mem_addr);

	if (NULL != img_info.bak_partition) {
		debugf("load %s with backup img %s\n", img_info.partition, img_info.bak_partition);
		_boot_read_partition_with_backup(dev, img_info);
	} else {
#ifdef CONFIG_SECURE_BOOT
		if (!_boot_partition_read(dev, img_info.partition, 0, SEC_HEADER_MAX_SIZE, header_buf)) {
			debugf("%s:%s read error!\n", img_info.partition);
			return 0;
		}
		//if(header_parser(header_buf) )
		secure_boot_offset = get_code_offset(header_buf);
		_boot_partition_read(dev, img_info.partition, 0 + secure_boot_offset, img_info.size, (u8 *) img_info.mem_addr);

		secure_verify("uboot", header_buf, img_info.mem_addr);
#else
		_boot_partition_read(dev, img_info.partition, 0, img_info.size, (u8 *) img_info.mem_addr);

#endif
	}

	return 1;
}

/**
	Function for checking and loading kernel/ramdisk image.
*/
int _boot_load_kernel_ramdisk_image(block_dev_desc_t * dev, char *bootmode, boot_img_hdr * hdr)
{
	uchar *partition = NULL;
	uint32_t size, offset;
	uint32_t dt_img_adr;
	uint32_t secure_boot_offset = 0;
	if (0 == memcmp(bootmode, RECOVERY_PART, strlen(RECOVERY_PART))) {
		partition = "recovery";
		debugf("enter recovery mode!\n");
	} else {
		partition = "boot";
		debugf("enter boot mode!\n");
	}
#ifdef CONFIG_SECURE_BOOT
	if (!_boot_partition_read(dev, partition, 0, 512, (u8 *) hdr)) {
		errorf("%s read error!\n", partition);
		return 0;
	}
	secure_boot_offset = get_code_offset(hdr);
#endif
	if (!_boot_partition_read(dev, partition, 0 + secure_boot_offset, 4 * EMMC_SECTOR_SIZE, (u8 *) hdr)) {
		errorf("%s read error!\n", partition);
		return 0;
	}
	/*image header check */
	if (0 != memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		errorf("bad boot image header, give up boot!!!!\n");
		return 0;
	}

	/*read kernel image */
	offset = 4;
	size = (hdr->kernel_size + (KERNL_PAGE_SIZE - 1)) & (~(KERNL_PAGE_SIZE - 1));
	if (size <= 0) {
		errorf("kernel image should not be zero!\n");
		return 0;
	}
	if (!_boot_partition_read(dev, partition, offset + secure_boot_offset, size, (u8 *) KERNEL_ADR)) {
		errorf("%s kernel read error!\n", partition);
		return 0;
	}
	debugf("%s kernel read OK,size=%u! \n", partition, size);
	/*read ramdisk image */
	offset += size / 512;
	offset = ((offset + 3) / 4) * 4;
	size = (hdr->ramdisk_size + (KERNL_PAGE_SIZE - 1)) & (~(KERNL_PAGE_SIZE - 1));
	if (size < 0) {
		debugf("ramdisk size error\n");
		return 0;
	}
	if (!_boot_partition_read(dev, partition, offset + secure_boot_offset, size, (u8 *) RAMDISK_ADR)) {
		errorf("ramdisk read error!\n");
		return 0;
	}
	debugf("%s ramdisk read OK,size=%u! \n", partition, size);
#ifdef CONFIG_OF_LIBFDT
	//read dt image
	offset += size / 512;
	offset = ((offset + 3) / 4) * 4;
	size = (hdr->dt_size + (KERNL_PAGE_SIZE - 1)) & (~(KERNL_PAGE_SIZE - 1));
	dt_img_adr = RAMDISK_ADR - size - KERNL_PAGE_SIZE;
	if (size < 0) {
		errorf("dt size error\n");
		return 0;
	}
	if (!_boot_partition_read(dev, partition, offset + secure_boot_offset, size, (u8 *) dt_img_adr)) {
		errorf("dt read error!\n");
		return 0;
	}
	debugf("%s dtb read OK,size=%u! \n", partition, size);
	if (load_dtb((void*) DT_ADR, (void *)dt_img_adr)) {
		errorf("dt load error!\n");
		return 0;
	}
#endif
#ifdef CONFIG_SDRAMDISK
	{
		int sd_ramdisk_size = 0;
#ifdef WDSP_ADR
		size = WDSP_ADR - RAMDISK_ADR;
#else
		size = TDDSP_ADR - RAMDISK_ADR;
#endif
		if (size > 0)
			sd_ramdisk_size = load_sd_ramdisk((void *) RAMDISK_ADR, size);
		if (sd_ramdisk_size > 0)
			hdr->ramdisk_size = sd_ramdisk_size;
	}
#endif
	return 1;
}

#ifdef CONFIG_SECURE_BOOT
int secure_verify_partition(block_dev_desc_t * dev, uchar * partition_name, void *ram_addr)
{
	int ret = 0;
	int size;
	disk_partition_t info;

	if (get_partition_info_by_name(dev, partition_name, &info)) {
		errorf("verify get partition %s info failed!\n", partition_name);
		ret = 1;
	}
	size = info.size * EMMC_SECTOR_SIZE;
	debugf("%s=%x  =%x\n", partition_name, info.size, size);
	_boot_partition_read(dev, partition_name, 0, size, (uint8_t *) ram_addr);
	secure_verify("uboot", (uint8_t *) ram_addr, 0);
	return ret;
}
#endif
void vlx_nand_boot(char *kernel_pname, int backlight_set)
{
	boot_img_hdr *hdr = (void *)raw_header;
	block_dev_desc_t *dev = NULL;
	char *mode_ptr = NULL;
	uchar *partition = NULL;
	int i = 0;
	int j = 0;
	int ret = 0;
	dev = get_dev("mmc", 0);
	if (NULL == dev) {
		errorf("Fatal Error,get_dev mmc failed!\n");
		return;
	}
#ifdef CONFIG_SPLASH_SCREEN
	set_backlight(backlight_set);
#endif
#ifdef CONFIG_SECURE_BOOT

	if (0 == memcmp(kernel_pname, RECOVERY_PART, strlen(RECOVERY_PART))) {
		partition = "recovery";
	} else {
		partition = "boot";
	}
	secure_verify_partition(dev, partition, KERNEL_ADR);
#endif
#ifdef OTA_BACKUP_MISC_RECOVERY
	ret = memcmp(kernel_pname, RECOVERY_PART, strlen(RECOVERY_PART));
	if ((ret != 0) || (boot_load_recovery_in_sd(hdr) != 0))
		if (!_boot_load_kernel_ramdisk_image(dev, kernel_pname, hdr))
			return;
#else
	//loader kernel and ramdisk
	if (!_boot_load_kernel_ramdisk_image(dev, kernel_pname, hdr))
		return;
#endif


	set_vibrator(0);

#ifdef BOOT_NATIVE_LINUX_MODEM
	/*load required image which config in table */
	i = 0;
#ifdef CONFIG_ARM7_RAM_ACTIVE
			pmic_arm7_RAM_active();
#endif
	while (s_boot_image_table[i]) {
		j = 0;
		while (s_boot_image_table[i][j].partition) {
			_boot_load_required_image(dev, s_boot_image_table[i][j]);
			j++;
		}
		i++;
	}
#endif

	vlx_entry();
}
