#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
MODULE_LICENSE("DUAL BSD/GPL");

#define LED 17
#define BTN 18

int irqNum;

static inline void set_led(void){
    gpio_set_value(LED, 1);
}

static inline void clr_led(void){
    gpio_set_value(LED, 0);
}

static irqreturn_t irq_handler(int irq, void *dev_id, struct pt_regs *regs){
    printk(KERN_INFO "irq handler called\n");
    if(gpio_get_value(BTN)){
        set_led();
    }else{
        clr_led();
    }
    return IRQ_HANDLED;
}

static int __init test_init(void){
    printk(KERN_INFO "Loading module\n");
    gpio_request(BTN, "BTN");
    gpio_request(LED, "LED");
    gpio_direction_input(BTN);
    gpio_direction_output(LED, 0);
    printk(KERN_INFO "Setup GPIOs\n");
    if((irqNum = gpio_to_irq(BTN)) < 0){
        printk(KERN_INFO "Error getting irq number.\n");
        gpio_free(BTN);
        gpio_free(LED);
        return 0;
    }
    printk(KERN_INFO "Got irq number\n");
    if(request_irq(irqNum, (irq_handler_t ) irq_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "button handler", "plz work") < 0){
        printk(KERN_INFO "Error while making irq request\n");
        gpio_free(BTN);
        gpio_free(LED);
        return 0;
    }
    printk(KERN_INFO "Finished loading.\n");
    return 0;
}

static void __exit test_exit(void){
    printk(KERN_INFO "Goodbye, world!\n");
    gpio_free(LED);
    gpio_free(BTN);
    free_irq(irqNum, NULL);
}

module_init(test_init);
module_exit(test_exit);
