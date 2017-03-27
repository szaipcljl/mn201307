#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/unaligned.h>
MODULE_LICENSE("GPL");

static struct file *fp = NULL;

int write_to_file(char *buf, int size)
{
	int ret = 0;
	struct file *fp;
	mm_segment_t old_fs;
	loff_t pos = 0;
	int nwrite = 0;
	static int offset = 0;
	static int first_flag = 0;

	/* change to KERNEL_DS address limit */
	old_fs = get_fs();
	set_fs(KERNEL_DS);


	if (first_flag == 0) {
		first_flag = 1;
		/* open file to write */
		/*fp = filp_open("/data/at_log1", O_WRONLY|O_CREAT, 0640);*/
		fp = filp_open("./log1", O_WRONLY | O_CREAT, 0640);
		if (!fp) {
			printk("%s: open file error\n", __FUNCTION__);
			ret = -1;
			goto exit;
		}
	}
	pos=(unsigned long)offset;

	/* Write buf to file */
	nwrite = vfs_write(fp, buf, size, &pos);
	offset += nwrite;

exit:
	return ret;
}

static int test_init(void)
{
	write_to_file("hello world", sizeof("hello world") - 1);
	return 0;
}

static void test_exit(void)
{
	if (fp)
		filp_close(fp, NULL);
}

module_init(test_init);
module_exit(test_exit);
