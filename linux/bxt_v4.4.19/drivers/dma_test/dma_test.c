#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

#define SYSFS_ATTR_CREATE
#ifdef SYSFS_ATTR_CREATE

static int test_value;

static ssize_t test_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	printk("###%s:%d\n", __func__, __LINE__);
	return sprintf(buf, "%d\n", test_value);
}

static ssize_t test_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	printk("###%s:%d\n", __func__, __LINE__);

	sscanf(buf, "%d\n", &test_value);
	return count;
}

static struct kobj_attribute
test_value_attribute = __ATTR(test_value, 0664/*0666*/, test_show, test_store);
//chmod a+w test_value

static struct kobject *dmatest_kobj;
#endif

#define CREATE_CHAR_DEV
#ifdef CREATE_CHAR_DEV
#include <linux/cdev.h>			/* Char device regiseter / deregister */
#include <linux/device.h>		/* dynamic device creating */
#include <linux/fs.h>

#define DEVICE_NAME "dmatest_drv"
static int device_open(struct inode *inode, struct file *filep);
static int device_close(struct inode *inode, struct file *filep);
static ssize_t device_read(struct file *filp, char __user * buffer,
						   size_t length, loff_t * offset);
static ssize_t device_write(struct file *filp, const char __user * buff,
							size_t len, loff_t * off);
long device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

/* Char device creation functions */
static dev_t dmatest_drv_dev;		/* Device number */
static struct cdev dmatest_drv_cdev;	/* Char device structure */
static struct class *dmatest_drv_class;	/* Device class */

/* File operations structure */
static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = device_ioctl,
	.compat_ioctl = device_ioctl,
	.open = device_open,
	.release = device_close,
};


static ssize_t device_read(struct file *filp, char __user * buffer,
						   size_t length, loff_t * offset)
{
	return 0;
}
static ssize_t device_write(struct file *filp, const char __user * buff,
							size_t len, loff_t * off)
{
	return 0;
}

long device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	return 0;
}

int device_open(struct inode *inode, struct file *filep)
{
	return 0;
}

int device_close(struct inode *inode, struct file *filep)
{
	return 0;
}

/* Create a chr device */
static int create_device(void)
{

	/* Allocate char device */
	if (alloc_chrdev_region(&dmatest_drv_dev, 0, 1, DEVICE_NAME) < 0) {
		printk("Failed alloc_chardev_region\n");
		return -1;
	}

	/* Create a device class */
	if ((dmatest_drv_class = class_create(THIS_MODULE, "dmatest_drv_class")) == NULL) {
		printk("### class_create failed.\n");
		unregister_chrdev_region(dmatest_drv_dev, 1);
		return -1;
	}

	/* Create the device */
	if (device_create(dmatest_drv_class, NULL, dmatest_drv_dev, NULL, DEVICE_NAME) ==
		NULL) {
		printk("### device_create failed.\n");
		class_destroy(dmatest_drv_class);
		unregister_chrdev_region(dmatest_drv_dev, 1);
		return -1;
	}
	/* Add SHM char device */
	cdev_init(&dmatest_drv_cdev, &fops);
	if (cdev_add(&dmatest_drv_cdev, dmatest_drv_dev, 1) == -1) {
		printk("### cdev_init failed.\n");
		device_destroy(dmatest_drv_class, dmatest_drv_dev);
		class_destroy(dmatest_drv_class);
		unregister_chrdev_region(dmatest_drv_dev, 1);
		return -1;
	}
	printk("\n### driver installed\n");
	return 0;
}

static void delete_device(void)
{
	printk("Deleting tee_drv device:%s \n", DEVICE_NAME);
	cdev_del(&dmatest_drv_cdev);
	device_destroy(dmatest_drv_class, dmatest_drv_dev);
	class_destroy(dmatest_drv_class);
	unregister_chrdev_region(dmatest_drv_dev, 1);
	return;
}

#endif

#if 1
void make_vmcall(void)
{
	printk("dma_test###14 %s start\n", __func__);
}
#else
extern make_vmcall(void);
#endif


static void dma_debug_buffer_alloc(void)
{
	printk("dma_test###14 %s start\n", __func__);
#if 0
	unsigned long debug_gpa;
	int vmcall_id = TRUSTY_VMCALL_DEBUG_BUFFER;
	g_sg_debug_gva = (unsigned long)kmalloc(DEBUG_BUF_SIZE, GFP_KERNEL);
	if (!g_sg_debug_gva) {
		printk("Failed to allocate memory for debug_buf");
		return;
	}
	memset((char *)g_sg_debug_gva, 0, DEBUG_BUF_SIZE);
	/* The end addres of the debug buffer */
	g_sg_debug_gva_end = g_sg_debug_gva + DEBUG_BUF_SIZE;
	debug_gpa = virt_to_phys((void *)g_sg_debug_gva);
	printk("SG Debug Buffer: GVA=%lx, GPA=%lx, Size=%d\n", g_sg_debug_gva,
		   debug_gpa, DEBUG_BUF_SIZE);
#endif
	make_vmcall();
}

static void dmatest_dev_init(void)
{
	printk("dma_test###14 %s\n", __func__);
}
static void trigger_dmatest(void)
{
	printk("dma_test###14 %s\n", __func__);
}

static int dma_test_init(void)
{
	printk("dma_test###14 %s\n", __func__);

	int ret;

#ifdef SYSFS_ATTR_CREATE
	/*
	 * Create a simple kobject with the name of "dmatest",
	 * located under /sys/kernel/
	 **/
	dmatest_kobj = kobject_create_and_add("dmatest", kernel_kobj);
	if (!dmatest_kobj)
		return -ENOMEM;

	ret = sysfs_create_file(dmatest_kobj, &test_value_attribute);
	if (ret)
		kobject_put(dmatest_kobj);
#endif

#ifdef CREATE_CHAR_DEV
	/* register and create device */
	if (create_device() != 0) {
		printk("Failed to create device:%s\n", DEVICE_NAME);
		return -1;
	}
#endif

	dma_debug_buffer_alloc();
	dmatest_dev_init();
	trigger_dmatest();
	return 0;
}

static void dma_test_exit(void)
{
#ifdef SYSFS_ATTR_CREATE
	kobject_put(dmatest_kobj);
#endif

#ifdef CREATE_CHAR_DEV
	/* Unregister and delete the node */
	delete_device();
#endif
	printk("dma_test###14%s: device exit...\n", __func__);

}

module_init(dma_test_init);
module_exit(dma_test_exit);
