#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/cdev.h>			/* Char device regiseter / deregister */
#include <linux/device.h>		/* dynamic device creating */
#include <linux/fs.h>
#include <linux/slab.h>			/* kmalloc etc.. */
#include "pci_access.h"
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define LED_ON _IO('K',0)
/*#define LED_OFF _IO('K',1)*/
#define LED_OFF _IOW('K',1,int)

#define SYSFS_ATTR_CREATE
#ifdef SYSFS_ATTR_CREATE

static int test_value;

static ssize_t test_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	printk("=== %s:%d\n", __func__, __LINE__);
	return sprintf(buf, "%d\n", test_value);
}

static ssize_t test_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	printk("=== %s:%d\n", __func__, __LINE__);

	sscanf(buf, "%d\n", &test_value);
	return count;
}

static struct kobj_attribute
test_value_attribute = __ATTR(test_value, 0664/*0666*/, test_show, test_store);
//chmod a+w test_value

static struct kobject *pcihacker_kobj;
#endif

#define CREATE_CHAR_DEV
#ifdef CREATE_CHAR_DEV

#define DEVICE_NAME "pcihacker_drv"
static int device_open(struct inode *inode, struct file *filep);
static int device_close(struct inode *inode, struct file *filep);
static ssize_t device_read(struct file *filp, char __user * buffer,
		size_t length, loff_t * offset);
static ssize_t device_write(struct file *filp, const char __user * buff,
		size_t len, loff_t * off);
long device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

/* Char device creation functions */
static dev_t pcihacker_drv_dev;		/* Device number */
static struct cdev pcihacker_drv_cdev;	/* Char device structure */
static struct class *pcihacker_drv_class;	/* Device class */

/* File operations structure */
static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = device_ioctl,
	.compat_ioctl = device_ioctl,
	.open = device_open,
	.release = device_close,
};

struct pci_test_t {
	uint16_t pci_dev;
	/*uint8_t reg;*/
	uint16_t reg;
	uint16_t val;
};

typedef struct pci_test_t pci_test;
pci_test g_pci_test;
pci_block_device_t g_pci_block_dev;

static ssize_t device_read(struct file *filp, char __user * buff,
		size_t len, loff_t * off)
{
	printk("=== %s:%d\n", __func__, __LINE__);

	copy_from_user(&g_pci_test , buff, len);

	// read cfg
	/*uint16_t ret = pci_read16(g_pci_test.pci_dev, g_pci_test.reg);*/
	g_pci_block_dev.pci_dev = g_pci_test.pci_dev;
	pci_cfg_bars_decode(&g_pci_block_dev);
	printk("### g_pci_block_dev.pci_dev=%x\n", g_pci_block_dev.pci_dev);  

	int i;
	for (i=0; i < PCI_DEVICE_BAR_NUM; i++) {
		printk("### bars[%d]=%llx\n", i, g_pci_block_dev.bars[i].addr);
	}

	if ((g_pci_block_dev.bars[0].addr & (0xffffffffULL<<32)) == (0xffffffffULL<<32)) {
		printk("### offset-240%x\n", *(uint32_t *)(g_pci_block_dev.bars[0].addr+0x240));
		printk("### offset-248 %x\n", *(uint32_t *)(g_pci_block_dev.bars[0].addr+0x248));
	}

	// read bar
	uint32_t ret = pci_read32(g_pci_test.pci_dev, g_pci_test.reg);
	printk("### ret = %x, pci_dev=%x, reg=%x\n", ret, g_pci_test.pci_dev, g_pci_test.reg);

	return 0;
}

static ssize_t device_write(struct file *filp, const char __user * buff,
		size_t len, loff_t * off)
{
	printk("=== %s:%d\n", __func__, __LINE__);

	copy_from_user(&g_pci_test , buff, len);

#if 0
	uint16_t pci_dev = PCI_DEV(0, 0xd, 0);
	uint8_t reg = 0xe0;
	uint16_t ret;
#endif

	pci_write16(g_pci_test.pci_dev, g_pci_test.reg, g_pci_test.val);
	printk("###pci_dev=%x, reg=%x\n", g_pci_test.pci_dev, g_pci_test.reg);
	return 0;
}

long device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
#define N 123
	char data[N];
	printk("=== %s:%d\n", __func__, __LINE__);

	switch(cmd) {
	case LED_ON:
		printk("LED_ON\n");
		break;
	case LED_OFF:
		copy_from_user(data, (void __user *)arg, sizeof(void *));
		printk("### LED_OFF:fd = %s\n", (char *)data);
		break;
	}

	return 0;
}

int device_open(struct inode *inode, struct file *filep)
{
	printk("=== %s:%d\n", __func__, __LINE__);
	return 0;
}

int device_close(struct inode *inode, struct file *filep)
{
	printk("=== %s:%d\n", __func__, __LINE__);
	return 0;
}

/* Create a chr device */
static int create_device(void)
{
	/* Allocate char device */
	if (alloc_chrdev_region(&pcihacker_drv_dev, 0, 1, DEVICE_NAME) < 0) {
		printk("### Failed alloc_chardev_region\n");
		return -1;
	}

	/* Create a device class */
	if ((pcihacker_drv_class = class_create(THIS_MODULE, "dmatest_drv_class")) == NULL) {
		printk("### class_create failed.\n");
		unregister_chrdev_region(pcihacker_drv_dev, 1);
		return -1;
	}

	/* Create the device */
	if (device_create(pcihacker_drv_class, NULL, pcihacker_drv_dev, NULL, DEVICE_NAME) ==
			NULL) {
		printk("### device_create failed.\n");
		class_destroy(pcihacker_drv_class);
		unregister_chrdev_region(pcihacker_drv_dev, 1);
		return -1;
	}
	/* Add SHM char device */
	cdev_init(&pcihacker_drv_cdev, &fops);
	if (cdev_add(&pcihacker_drv_cdev, pcihacker_drv_dev, 1) == -1) {
		printk("### cdev_init failed.\n");
		device_destroy(pcihacker_drv_class, pcihacker_drv_dev);
		class_destroy(pcihacker_drv_class);
		unregister_chrdev_region(pcihacker_drv_dev, 1);
		return -1;
	}
	printk("\npcihacker### driver installed\n");
	return 0;
}

static void delete_device(void)
{
	printk("=== Deleting tee_drv device:%s \n", DEVICE_NAME);
	cdev_del(&pcihacker_drv_cdev);
	device_destroy(pcihacker_drv_class, pcihacker_drv_dev);
	class_destroy(pcihacker_drv_class);
	unregister_chrdev_region(pcihacker_drv_dev, 1);
	return;
}

#endif



static int pcihacker_init(void)
{
	printk("=== pcihacker###14 %s\n", __func__);
	int ret;

#ifdef SYSFS_ATTR_CREATE
	/*
	 * Create a simple kobject with the name of "pcihacker",
	 * located under /sys/kernel/
	 **/
	pcihacker_kobj = kobject_create_and_add("pcihacker", kernel_kobj);
	if (!pcihacker_kobj)
		return -ENOMEM;

	ret = sysfs_create_file(pcihacker_kobj, &test_value_attribute);
	if (ret)
		kobject_put(pcihacker_kobj);
#endif

#ifdef CREATE_CHAR_DEV
	/* register and create device */
	if (create_device() != 0) {
		printk("Failed to create device:%s\n", DEVICE_NAME);
		return -1;
	}
#endif

	return 0;
}

static void pcihacker_exit(void)
{
#ifdef SYSFS_ATTR_CREATE
	kobject_put(pcihacker_kobj);
#endif

#ifdef CREATE_CHAR_DEV
	/* Unregister and delete the node */
	delete_device();
#endif
	printk("pcihacker###14%s: device exit...\n", __func__);

}

module_init(pcihacker_init);
module_exit(pcihacker_exit);
