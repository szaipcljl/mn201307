/*ref: samples/kobject/kobject-example.c*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

#define SYSFS_ATTR_CREATE
#ifdef SYSFS_ATTR_CREATE
static int test_value;

static ssize_t test_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	printk("%s:%d\n", __func__, __LINE__);
	return sprintf(buf, "%d\n", test_value);
}

static ssize_t test_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	printk("%s:%d\n", __func__, __LINE__);

	sscanf(buf, "%du", &test_value);
	return count;
}

static struct kobj_attribute test_value_attribute = __ATTR(test_value, 0664/*0666*/, test_show, test_store);
/*
 * 0666: build error? why
 * include/linux/bug.h:33:45: error: negative width in bit-field ‘<anonymous>’
 *  #define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
 * chmod a+w test_value
 */

static struct kobject *mntest_kobj;
#endif

static int __init mntest_init(void)
{
	int ret;

#ifdef SYSFS_ATTR_CREATE
	/*
	 * Create a simple kobject with the name of "mntest",
	 * located under /sys/kernel/
	 **/
	mntest_kobj = kobject_create_and_add("mntest", kernel_kobj);
	if (!mntest_kobj)
		return -ENOMEM;

	ret = sysfs_create_file(mntest_kobj, &test_value_attribute);
	if (ret)
		kobject_put(mntest_kobj);
#endif

	printk("%s:%d\n", __func__, __LINE__);

	return ret;
}

static void __exit mntest_exit(void)
{
#ifdef SYSFS_ATTR_CREATE
	kobject_put(mntest_kobj);
#endif
	printk("%s:%d\n", __func__, __LINE__);
}

module_init(mntest_init);
module_exit(mntest_exit);
