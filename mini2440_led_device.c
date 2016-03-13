/* ´´½¨ ×¢²á */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>

struct resource mini2440_led_resource [] ={	

	[0] = {
			.start = 0x56000010,
			.end   = 0x56000010+8-1,
			.flags  = IORESOURCE_MEM , 
	},
	[1] = {
			.start = 6 ,
			.end   = 6 ,
			.flags  = IORESOURCE_IRQ	,
	},
};

static void mini2440_led_device_release(struct device *dev)
{
	printk("Does this usr for clean device hardward resouce\n");
}
struct platform_device mini2440_led_dev={
	.name = "mini2440_led",
	.id = -1,
	.num_resources = ARRAY_SIZE(mini2440_led_resource), 
	.resource = mini2440_led_resource,
	.dev = {
		.release = mini2440_led_device_release,
	},
};
static int __init mini2440_leddev_init(void)
{
		int err;
		printk(KERN_ALERT "mini2440_led_init\n");
		err = platform_device_register(&mini2440_led_dev);
		if(err)
		{
			printk(KERN_ALERT "mini2440 led device register failed\n");
		}
		
		return err;
}

static void __exit mini2440_leddev_exit(void)
{
	printk(KERN_ALERT "mini2440 led device exit\n");
	platform_device_unregister(&mini2440_led_dev);
}

module_init(mini2440_leddev_init);
module_exit(mini2440_leddev_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Tonyren  renweiaust@163.com");
MODULE_DESCRIPTION("tony ren excise");