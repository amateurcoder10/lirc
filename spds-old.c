/*
===============================================================================
Driver Name		:		SPDS
Author			:		SHUBHAM
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include"SPDS.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHUBHAM");
MODULE_DESCRIPTION("This module sends POWER_OFF pulse to switch off the projector.");

//===============================================================================


static void safe_udelay(unsigned long usecs)
{
	while (usecs > MAX_UDELAY_US)
	{
		udelay(MAX_UDELAY_US);
		usecs -= MAX_UDELAY_US;
	}
	udelay(usecs);
}



void pulse(unsigned long usec){
	gpio_set_value(GPIO_PIN,1); //HIGH
	safe_udelay(usec);
}
void space(unsigned long usec){
	gpio_set_value(GPIO_PIN,0); //HIGH
	safe_udelay(usec);
}

void send_header(void){

	pulse(HEADER_PULSE);
	space(HEADER_SPACE);
}
void send_message(void){
	long data = KEY_POWER;
	int i=0;
	for(i=0;i<NUMBITS;i++){
		if(data & 0x80000000)
		{
			pulse(ONE_PULSE);
			space(ONE_SPACE);
		}
		else
		{
			pulse(ZERO_PULSE);
			space(ZERO_SPACE);
		}
		data = data<<1; //shift data left by 1 bit
	}
}
void send_trail_pulse(void){
	pulse(TRAIL_PULSE);
	space(0);
}

void send_off_pulse(void){
	send_header();
	send_message();
	send_trail_pulse();
}


//===============================================================================
struct SPDS_DEVICE {
		char *name ;
		int status;
		struct device dev;
} ir_dev;
#define to_SPDS_device(x)	container_of(x, struct SPDS_DEVICE, dev)


//===============================================================================
static ssize_t get_name(struct device *dev, struct device_attribute *attr, char *resp)
{
	pr_info("name = %s\n", ir_dev.name);
	return snprintf(resp,40, "%s\n",ir_dev.name);
}

static DEVICE_ATTR(name, S_IRUGO, get_name, NULL);



static ssize_t get_hdmi_status(struct device *dev, struct device_attribute *attr, char *resp)
{
	return sprintf(resp, "%d\n",ir_dev.status );
}

static ssize_t set_hdmi_status(struct device *dev, struct device_attribute *attr, const char *newval, size_t valsize)
{
	int value;
	ssize_t result;
	result = sscanf(newval, "%d", &value);
	dev_alert(dev, "changing hdmi_status to %d\n", value);
	if(result != 1)
		return -EINVAL;
	else if(value==1)
	{
		dev_alert(dev, "HDMI unplugged! Sending ir_pulse.\n");
		ir_dev.status = value;
		result = valsize;
		send_off_pulse();
	}
	else if(value==0){
		ir_dev.status = value;
		result = valsize;
	}
	else{
		dev_alert(dev, "This value is not permitted. \n");
		return -EINVAL;
	}
	return result;

}

static DEVICE_ATTR(hdmi_status, S_IRUGO|S_IWUSR, get_hdmi_status, set_hdmi_status);



static struct attribute *SPDS_attrs[] = {
	&dev_attr_name.attr,
	&dev_attr_hdmi_status.attr,
	NULL
};

static struct attribute_group SPDS_basic_attrs = {
	.attrs = SPDS_attrs,
};

static const struct attribute_group *SPDS_all_attrs[] = {
 	&SPDS_basic_attrs,
 	NULL
};

//===============================================================================
static void ir_blaster_release(struct device *dev)
{
	dev_alert(dev, "releasing %s\n", dev->kobj.name);
}

//int match_func(struct gpio_chip* chip, void * data){
//	pr_info("chip-label %s %s",chip->label, data);
//	if(strcmp(name,chip->label)==0)
//		return 1;
//	else
//		return 0;
//}
//
//int load_chip(void){
//	chip = gpiochip_find("pinctrl-bcm2835",match_func);
//	if(!chip)
//		return 0;
//	return 1;
//}
static void gpio_init(void){
	if(!gpio_is_valid(GPIO_PIN))
		pr_info("Error Invalidd GPIO");
	gpio_request(GPIO_PIN,"sysfs");
	gpio_direction_output(GPIO_PIN,0);
	gpio_export(GPIO_PIN,false);
}
static int __init SPDS_init(void)
{
	int err;
	struct device *dev= &ir_dev.dev;
	gpio_init();
//	err = load_chip();
//	if(err){
//		pr_err("failed to register device because gpio-chip is unavailable\n");
//		return err;
//	}
	ir_dev.name ="ir_blaster";
	ir_dev.status = 0;
	dev->init_name = ir_dev.name;
	dev->release = ir_blaster_release;
	dev->groups = SPDS_all_attrs;
	err=device_register(dev);
	if (err)
			pr_err("failed to register device %s error %d\n", ir_dev.name, err);
	dev_alert(dev, "registered successfully!\n");
	return err;
}

static void __exit SPDS_exit(void)
{
	struct device *dev = &ir_dev.dev;
	dev_alert(dev, "unregistering ir_blaster %s\n", ir_dev.name);
	//device_remove_file(dev, &dev_attr_name);
	//device_remove_file(dev, &dev_attr_hdmi_status);
	device_unregister(&ir_dev.dev);
}

module_init(SPDS_init);
module_exit(SPDS_exit);

