#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>

#include <linux/timer.h>
#include <linux/delay.h>

struct timer_list sln_timer;

void sln_timer_do(unsigned long l)
{
	mod_timer(&sln_timer, jiffies + HZ); //kernel/time/timer.c

	printk(KERN_ALERT"param: %ld, jiffies: %ld\n", l, jiffies);
}

void sln_timer_set(void)
{
	init_timer(&sln_timer); //include/linux/timer.h

	sln_timer.expires = jiffies + HZ;   //1s
	sln_timer.function = sln_timer_do;
	sln_timer.data = 9527;

	add_timer(&sln_timer); //kernel/time/timer.c
}

static int __init sln_init(void)
{
	printk(KERN_ALERT"===%s===\n", __func__);

	sln_timer_set();
	return 0;
}

static void __exit sln_exit(void)
{
	printk(KERN_ALERT"===%s===\n", __func__);

	del_timer(&sln_timer);
}

module_init(sln_init);
module_exit(sln_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");

/*
 * note:
 * 是每秒钟打印出当前系统jiffies的值。
 *	init_timer()
 *  add_timer()
 *  mod_timer()
 *  del_timer
 *
 *	struct timer_list - 内核定时器由结构timer_list表示
 */
