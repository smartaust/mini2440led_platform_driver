/*tony for define test here*/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define major 250
#define MAGIC_LED_NUM 8
#define LEDS_ON _IO(MAGIC_LED_NUM,0xa)
#define LEDS_OFF _IO(MAGIC_LED_NUM,0x8a)

#define LED1_ON _IO(MAGIC_LED_NUM,0xb)
#define LED1_OFF _IO(MAGIC_LED_NUM,0x8b)

#define LED2_ON _IO(MAGIC_LED_NUM,0xc)
#define LED2_OFF _IO(MAGIC_LED_NUM,0x8c)

#define LED3_ON _IO(MAGIC_LED_NUM,0xd)
#define LED3_OFF _IO(MAGIC_LED_NUM,0x8d)

#define LED4_ON _IO(MAGIC_LED_NUM,0xe)
#define LED4_OFF _IO(MAGIC_LED_NUM,0x8e)





struct cdev led_cdev;
struct class * led_class ;

volatile unsigned long * gpbcon ;
volatile unsigned long * gpbdat ;
int pin = -1;


dev_t  devno;

static int mini2440_led_open(struct inode * pinode , struct file * pfile)
{
	    //配置gpbio5的io 为输出
		*gpbcon |= ((1<<(5*2))|(1<<(6*2))|(1<<(7*2))|(1<<(8*2)));
		*gpbdat |= ((1<<5)|(1<<6)|(1<<7)|(1<<8));//每次打开默认全部关闭
		printk(KERN_INFO "open led now\n");
		return 0;
}

static int mini2440_led_close (struct inode *pinode, struct file *pfile)
{

		//we can release interrupt num here
		printk(KERN_INFO"close led now\n");
		return 0;
}
ssize_t mini2440_led_write (struct file *pfile, const char __user * pbuf , size_t count, loff_t * offset )
{
		int val;
		
		copy_from_user(&val,pbuf,sizeof(val));
		printk(KERN_ALERT "write is coming %d\n",val);
		if(1 == val)
		{
				*gpbdat &= ~(1<<pin); 
		}
		else if(0 == val)
		{
			*gpbdat |= (1<<pin);
		}

		return 0 ;
			
}
int mini2440_led_ioctl (struct inode * pinode, struct file * pfile, unsigned int cmd, unsigned long size)
{
		int ret = 0;
		printk(KERN_ALERT "ioctl cmd is coming 0x%04x\n",cmd);
		switch(_IO(MAGIC_LED_NUM,cmd))
		{
			case LEDS_ON :
				*gpbdat &= ~(1<<5|1<<6|1<<7|1<<8); 
				break;
			case LED1_ON :
				*gpbdat &= ~(1<<5); 
				break;
			case LED2_ON :
				*gpbdat &= ~(1<<6); 
				break;
			case LED3_ON :
				*gpbdat &= ~(1<<7);
				break;
			case LED4_ON :
				*gpbdat &= ~(1<<8); 
				break;
			case LEDS_OFF :
				*gpbdat |= (1<<5|1<<6|1<<7|1<<8); 
				break;
			case LED1_OFF :
				*gpbdat |= (1<<5); 
				break;
			case LED2_OFF :
				*gpbdat |= (1<<6);
				break;
			case LED3_OFF :
				*gpbdat |= (1<<7);
				break;
			case LED4_OFF :
				*gpbdat |= (1<<8);
				break;
			default:
				printk("unknow operator\n");
				ret = -EINVAL;
				break;
		}
		return ret;
}

struct file_operations fops={
		.owner = THIS_MODULE,
		.open = mini2440_led_open,
		.release = mini2440_led_close,
		.write = mini2440_led_write,
		.ioctl = mini2440_led_ioctl,
};

static void led_cdev_setup(struct cdev *pdev)
{
	 	cdev_init(pdev,&fops),
		pdev->owner= THIS_MODULE,
		cdev_add(pdev,devno,1);
}
static int mini2440_led_probe(struct platform_device * pdev)
{
		/*
		*1、获取一个设备号 绑定fileoperations 结构体 
		*/
		int err;
		struct resource * psrc;
		printk(KERN_INFO "probe resouce\n");
		devno = MKDEV(major,0);
		if(major)
		{
			err = register_chrdev_region(devno,1,"mini2440_led");	
		}
		else 
		{
			err = alloc_chrdev_region(&devno,0,1,"mini2440_led");
		}

		led_cdev_setup(&led_cdev); // To bind cdev and fileoperation.

		device_create(led_class,NULL,devno,NULL,"mini2440_led%d",0);

		psrc = platform_get_resource(pdev,IORESOURCE_MEM,0);
		gpbcon = ioremap(psrc->start,psrc->end - psrc->start +1);
		gpbdat = gpbcon+1;

		psrc = platform_get_resource(pdev,IORESOURCE_IRQ,0);
		pin = psrc->start ;
		
		return 0;
}

static int mini2440_led_remove(struct platform_device *pdev)
{
		printk(KERN_INFO "oh ,this remove work\n");
	    cdev_del(&led_cdev);
		device_destroy(led_class,devno);
		unregister_chrdev_region(devno,1);
		return 0 ;
}

static struct platform_driver mymini2440_led_driver = {

	.probe =  mini2440_led_probe,
	.remove = mini2440_led_remove,
	.driver = {
				.owner = THIS_MODULE ,
				.name = "mini2440_led",
		},
};

static int __init mini2440_led_driver_init(void)
{
	led_class = class_create(THIS_MODULE,"mini2440_led");
	platform_driver_register(&mymini2440_led_driver);
	printk(KERN_INFO "mini2440_led_driver_init\n");
	return 0;
}

static void __exit mini2440_led_driver_exit(void)
{	
	platform_driver_unregister(&mymini2440_led_driver);
	class_destroy(led_class);
	printk(KERN_INFO "mini2440 platform led driver exit \n");
}

module_init(mini2440_led_driver_init);
module_exit(mini2440_led_driver_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("tonyren renweiaust@163.com");
MODULE_DESCRIPTION("this is excise for tony");
