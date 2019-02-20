
//相隔一定时间才printk一次信息
//当有时候，你需要10S 内，某个打印语句只打印一次，你就可以使用下面的demo:

int f() //???
{
	unsigned long later;

	later = jiffies + msecs_to_jiffies(1000*10);
	if (time_after(jiffies, later)) {
		printk(KERN_ERR " printk some information .... \n");
		return -1;
	}
}

//


int f2()
{
	unsigned long later;
	later = jiffies + msecs_to_jiffies(1000*10); // time to print

	while (1) {
		if (time_after(jiffies, later)) {
			printk(KERN_ERR " printk some information .... \n");
			later = jiffies + msecs_to_jiffies(1000*10); //update time to print
		}
		//do sometine...
	}
}

//time_after(a,b) returns true if the time a is after time b.

