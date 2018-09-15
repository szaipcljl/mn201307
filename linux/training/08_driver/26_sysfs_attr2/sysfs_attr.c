#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kobject.h>
MODULE_LICENSE("GPL");

//#define MISC_DEVICE

#define HELLO_MINORS    256
#define HELLO_MAJOR     125

#define DRIVER_NAME "hello_driver"//驱动名字
#define DEVICE_NAME "hello_device"//生成的设备节点名字

static int value = 0;

#ifdef MISC_DEVICE

#else
static struct class *hello_dev_class;
#endif

//对应应用层的write
static ssize_t att_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	value = simple_strtoul(buf, NULL, 10);

	printk("value:%d\n", value);

	return count;
}

//对应应用层的read
static ssize_t att_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	int val = 0;

	val = sprintf(buf, "%d\n",value);

	return val;
}

static DEVICE_ATTR(test, 0664, att_show, att_store);

static int hello_open(struct inode *inode, struct file *file){
	printk("hello open\n");
	return 0;
}

static int hello_release(struct inode *inode, struct file *file)
{
	printk("hello release\n");
	return 0;
}

static struct file_operations hello_ops = {
	.owner = THIS_MODULE,
	.open = hello_open,
	.release = hello_release,
};

#ifdef MISC_DEVICE
static  struct miscdevice hello_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,//设备节点名
	.fops = &hello_ops,
};
#endif

#ifdef MISC_DEVICE
//如果总线匹配成功 就会执行probe函数
static int hello_probe(struct platform_device *pdv)
{

	int ret = 0;
	printk("hello_probe\n");
	misc_register(&hello_dev);

	//在删除设备的时候自动删除该节点

	//由于对应的节点   /sys/devices/platform/hello_driver/test
	//ret = device_create_file(&pdv->dev, &dev_attr_test);

	//在/sys/class/misc/hello_device/test
	ret = device_create_file(hello_dev.this_device, &dev_attr_test);

	if(ret)
		printk("create enable file for hello_device err!\n");

	return ret;
}
#else  //普通字符设备
static int hello_probe(struct platform_device *pdv)
{
	int ret;
	struct device *dev;

	//参数二的名字不是作为设备名 仅仅是作为kobject里面的标识 kobject_set_name(&cdev->kobj, "%s", name)
	ret = register_chrdev(HELLO_MAJOR, DEVICE_NAME, &hello_ops );
	if (ret) {
		pr_info(KERN_ERR "%s:register chrdev failed\n", __FILE__);
		return ret;
	}

	hello_dev_class= class_create(THIS_MODULE, "hello_dev");
	if (IS_ERR(hello_dev_class)) {
		ret = PTR_ERR(hello_dev_class);
		class_destroy(hello_dev_class);
	}

	//生成设备 /dev/hello_device       /dev/char/125:256
	//参数二为指向的父设备NULL
	dev = device_create(hello_dev_class, NULL, MKDEV(HELLO_MAJOR, HELLO_MINORS), NULL, "%s", DEVICE_NAME);
	if (IS_ERR(dev)) {
		ret = PTR_ERR(dev);
		return ret;
	}

	//在删除设备的时候自动删除该节点
	//生成节点位置/sys/class/hello_dev/hello_device/device/test
	ret = device_create_file(dev, &dev_attr_test);
	if (ret)
		printk("create enable file for hello_device err!\n");

	return ret;
}
#endif

static int hello_remove(struct platform_device *pdv)
{

	printk("hello_remove\n");

#ifdef MISC_DEVICE
	misc_deregister(&hello_dev);
#endif

	return 0;
}

static void hello_shutdown(struct platform_device *pdv)
{

	;
}

static int hello_suspend(struct platform_device *pdv, pm_message_t pmt){

	return 0;
}

static int hello_resume(struct platform_device *pdv)
{

	return 0;
}

//创建设备
static struct platform_device hello_device = {
	.name = DRIVER_NAME,  //用于总线匹配
	.id = -1,
};

//创建驱动
struct platform_driver hello_driver = {
	.probe = hello_probe,
	.remove = hello_remove,
	.shutdown = hello_shutdown,
	.suspend = hello_suspend,
	.resume = hello_resume,
	.driver = {
		.name = DRIVER_NAME, //用于总线匹配
		.owner = THIS_MODULE,
	}
};

static int hello_init(void)
{
	int DriverState;

	//注册设备
	platform_device_register(&hello_device);

	printk(KERN_EMERG "hello_init!\n");
	//打印信息使用dmesg查看
	//执行platform_driver_register后 适配完 就会执行
	//hello_probe，即先打印hello_probe信息，才会打印
	//tDriverState信息
	//注册驱动
	DriverState = platform_driver_register(&hello_driver);

	printk(KERN_EMERG "\tDriverState is %d\n", DriverState);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_EMERG "hello_exit!\n");

#ifdef MISC_DEVICE

#else
	unregister_chrdev(HELLO_MAJOR,DEVICE_NAME);
	/*删除设备节点和设备类*/
	//sysfs: cannot create duplicate filename '/dev/char/125:256'
	// 如果没有正确删除或者系统已存在那么就会出现上面错误
	device_destroy(hello_dev_class, MKDEV(HELLO_MAJOR, HELLO_MINORS));
	class_destroy(hello_dev_class);
#endif
	platform_device_unregister(&hello_device);
	platform_driver_unregister(&hello_driver);
}

module_init(hello_init);
module_exit(hello_exit);
