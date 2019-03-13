#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

unsigned char *pagemem;
unsigned char *kmallocmem;
unsigned char *vmallocmem;

int init_module(void)
{
	pagemem = get_free_page(0);
	printk("<1>pagemem=%s",pagemem);

	kmallocmem = kmalloc(100,0);
	printk("<1>kmallocmem=%s",kmallocmem);

	vmallocmem = vmalloc(1000000);
	printk("<1>vmallocmem=%s",vmallocmem);

}

void cleanup_module(void)
{
	free_page(pagemem);

	kfree(kmallocmem);

	vfree(vmallocmem);
}
