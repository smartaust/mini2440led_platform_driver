obj-m += mini2440_led_device.o
obj-m += mini2440_led_driver.o

KERN_DIR = /home/tony/linux-2.6.32.2
default:
	make -C $(KERN_DIR) M=$(shell pwd) modules
clean:
	rm -rf *.mod.o *.o *.mod.o *.symvers *.order *.mod.c
	
