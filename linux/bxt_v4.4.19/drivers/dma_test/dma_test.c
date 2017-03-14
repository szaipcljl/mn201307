#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
/*#include <linux/moduleparam.h>*/
#include <linux/stat.h>

MODULE_LICENSE("GPL");



static int dma_test_init(void)
{
	printk("dma_test###14 %s\n", __func__);
	return 0;
}

static void dma_test_exit(void)
{
	printk("dma_test###14%s:\n", __func__);

}

module_init(dma_test_init);
module_exit(dma_test_exit);
