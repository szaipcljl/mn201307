

void printk_every_x_count(unsigned int step)
{
	static u64 count = 0;

	if (step == 0)
		return -1;

	if (++count % step) {
		printk("printk some information ... \n");
		count = 0;
	}
}
