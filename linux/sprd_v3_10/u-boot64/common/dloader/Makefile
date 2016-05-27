

obj-y += dl_common.o

obj-y += dl_cmd_proc.o
obj-$(CONFIG_EXT4_SPARSE_DOWNLOAD) += simg2img.o sparse_crc32.o

obj-$(CONFIG_EMMC_BOOT) += dl_emmc_operate.o
obj-$(CONFIG_NAND_BOOT) += dl_nand_operate.o dl_ubi.o parsemtdparts.o
obj-y += root_inspect.o
