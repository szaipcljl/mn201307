#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include "head.h"
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>

#define CLASS_DEV_CREATE
#ifdef CLASS_DEV_CREATE
#include <linux/device.h>
#endif


#define N 128
#define DEV_NAME "hello_class"
MODULE_LICENSE("GPL");

char data[N];

static int major = 220;
static int minor = 1;

#ifdef CLASS_DEV_CREATE
static struct class *cls;
static struct device *device;
#endif

static int hello_open(struct inode *inode, struct file *fl)
{
	printk("hello_open\n");
	return 0;
}

static int hello_release(struct inode *inode, struct file *file)
{
	printk("hello_release\n");

	return 0;
}

static ssize_t hello_read(struct file *file, char __user *buf,
		size_t size, loff_t *loff)
{
	if (size > N)
		size = N;
	if (size < 0)
		return -EINVAL;

	if (copy_to_user(buf, data, size))
		return -ENOMEM;

	printk("hello_read\n");
	return size;
}

static ssize_t hello_write(struct file *file, const char __user *buff,
		size_t size, loff_t *loff)
{
	if (size > N)
		size = N;
	if (size < 0)
		return -EINVAL;

	memset(data, '\0', sizeof(data));

	if (0 != copy_from_user(data, buff, size))
		return -ENOMEM;

	printk("hello_write\n");
	printk("data = %s\n", data);

	return size;
}

static long hello_unlocked_ioctl(struct file *file, unsigned int cmd,
		unsigned long arg)
{
	switch(cmd) {
	case LED_ON:
		printk("LED_ON\n");
		break;
	case LED_OFF:
		printk("LED_OFF\n");
		break;
	}

	printk("hello_unlocked_ioctl\n");

	return 0;
}

static struct cdev cdev;
static struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.open = hello_open,
	.read = hello_read,
	.write = hello_write,
	.release = hello_release,
	.unlocked_ioctl = hello_unlocked_ioctl,
};


typedef struct magic_heap {
	char * name;
	int magic[100];
} magic_heap_t;

void test_thread(void)
{
	printk("test_driver: enter %s\n", __func__);
	magic_heap_t *magic_heap_p;

	int magic_data[] = {
		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,

		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,

		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,

		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,
		0xdeadbeef,
	};
	printk("test_driver magic_data addr: 0x%llx\n", magic_data);

	magic_heap_p = (magic_heap_t *)kmalloc(sizeof(magic_heap_t), GFP_KERNEL);
	magic_heap_p->name = "my_magic_heap";
	int i;
	for (i = 0; i < 100; i++) {
		magic_heap_p->magic[i] = 0xdeadbeef;
	}
	printk("test_driver magic_heap_p: 0x%llx\n", magic_heap_p);


	i = 0;
	while(1) {
		printk("test_driver: magic_data %x\n", magic_data[i % 20]);
		printk("test_driver: magic_heap_p->magic %x\n", magic_heap_p->magic[i % 100]);
		msleep(5000);
		i++;
	}
}

static int hello_init(void)
{
	int ret;

	printk("hello_init\n");
	dev_t devno = MKDEV(major, minor);
	ret = register_chrdev_region(devno, 1, DEV_NAME);
	if (0 != ret) {
		//alloc_chrdev_region(&devno,0,1,DEV_NAME);
		printk("register_chrdev_region : error\n");
	}

	cdev_init(&cdev, &hello_ops);
	ret = cdev_add(&cdev, devno, 1);
	if (0 != ret) {
		printk("cdev_add\n");
		unregister_chrdev_region(devno, 1);
		return -1;
	}

#ifdef CLASS_DEV_CREATE
	cls = class_create(THIS_MODULE, DEV_NAME);
	device_create(cls, device, devno, NULL, DEV_NAME);
#endif

	printk("hello_init\n");

	kthread_run(test_thread, NULL, "my_test_thread");
	return 0;
}

static void hello_exit(void)
{
	dev_t devno = MKDEV(major, minor);

#ifdef CLASS_DEV_CREATE
	device_destroy(cls, devno);
	class_destroy(cls);
#endif

	cdev_del(&cdev);
	unregister_chrdev_region(devno, 1);

	printk("hello_exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
