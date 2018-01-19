
//1. I created a custom device in my device tree:

my_device@ffdf0000 {
	compatible = "my_driver";
	reg = <0xffdf0000 0x1000>;
	/*
	 * reg = address of device and size
	 * (Minimum is System's Pagesize = 0x1000 Byte in my case
	 */
}

//2. I wrote a Kernel stub for this Device:
//(Here I took kernel_src/drivers/uio/uio_pdrv_genirq.c and
//Hans J. Koch: Userspace I/O drivers in a realtime context 
//(device driver tutorial) as basis.)

//	This stub has following two structs:
//	The of_device_id struct:
		static struct of_device_id my_match_table[] = {
			{
				.compatible = "my_driver",
			},
			{0}
		};
		MODULE_DEVICE_TABLE(of, my_match_table);

//	and the driver struct itself:
	static struct platform_driver my_platform_driver = {
		.probe = my_probe,
		.remove = my_remove,
		.driver = {
			.name = "my_driver",
			.owner = THIS_MODULE,
			.of_match_table = of_match_ptr(my_match_table),
		},
	};

//3. Now I have access to the properties of the device tree in my probe function:
static int my_probe(struct platform_device *dev)
{
	struct uio_info *uioinfo;
	struct resource *r = &dev->resource[0];
	//[...]
	uioinfo->name = dev->dev.of_node->name; /* name from device tree: "my_device" */
	uioinfo->mem[0].addr = r->start; /* device address from device tree */
	uioinfo->mem[0].size = resource_size(r); /* size from device tree */
	//[...]
}

//When there is a match in compatible with both the kernel stub's entry and the device tree, the probe function is called.
