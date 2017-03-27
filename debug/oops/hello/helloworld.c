#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
	char *p = NULL;
	memcpy(p, "test", 4); //NULL pointer error
	printk(KERN_ALERT "Hello, world\n");
	return 0;
}

static void hello_exit(void)
{

	printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
