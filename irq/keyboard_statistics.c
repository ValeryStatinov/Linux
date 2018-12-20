#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Valery Kernel Professional");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define DEVICE_NAME "keyboard_statistics"
#define TASKLET_NAME "keyboard_tasklet"
#define KEYBOARD_IRQ 1
#define BUFSIZE 1024

/* This structure points to all of the device functions */
static struct file_operations file_opers = {
    .read = device_read,
    .open = device_open,
    .release = device_release,
    .owner = THIS_MODULE,
};

static int device_open_count = 0;
static int device_major_number;
static time_t click_time[BUFSIZE];
static unsigned int current_pos = 0;
static int my_dev_id;
static char msg_buffer[32];

void tasklet_action(unsigned long flag) {
    struct timespec curtime;
    getnstimeofday(&curtime);
    click_time[current_pos++] = curtime.tv_sec;
    if (current_pos == BUFSIZE) 
        current_pos = 0;
}

DECLARE_TASKLET(tasklet, tasklet_action, 0);

static irqreturn_t key_handler(int irq, void* dev_id) {
    int scancode = inb(0x60);
    if (!(scancode & 0x80))
        tasklet_schedule(&tasklet);
    return IRQ_HANDLED;
}

static int device_open(struct inode *inode, struct file *file) {
    if (device_open_count) {
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    unsigned int num = 0;
    unsigned int index;
    time_t current_time;
    struct timespec curtime;
    getnstimeofday(&curtime);
    current_time = curtime.tv_sec;
    if (current_pos) {
        index = current_pos - 1;
    } else {
        index = BUFSIZE - 1;
    }
    while (click_time[index] - current_time < 60 && click_time[index] != 0) {
        ++num;
        if (index) {
            --index;
        } else {
            index = BUFSIZE - 1;
        }
    }
    memset(msg_buffer, 0, 32);
    sprintf(msg_buffer, "%u\n", num);
    copy_to_user(buffer, msg_buffer, strlen(msg_buffer) + 1);
    return 0;
}

static int __init lkm_example_init(void) {
    device_major_number = register_chrdev(0, DEVICE_NAME, &file_opers);
    if (device_major_number < 0) {
        printk(KERN_ALERT "Could not register device\n");
        return device_major_number;
    } else {
        printk(KERN_INFO "Keyboard_statistics module registered, device major number: %d\n", device_major_number);
    }
    // register handler
    if (request_irq(KEYBOARD_IRQ, key_handler, IRQF_SHARED, DEVICE_NAME, &my_dev_id)) {
        printk(KERN_ALERT "cannot register IRQ 1");
        return -EIO;
    }
    memset(click_time, 0, BUFSIZE*sizeof(time_t));
    return 0;
}
static void __exit lkm_example_exit(void) {
    unregister_chrdev(device_major_number, DEVICE_NAME);
    tasklet_kill(&tasklet);
    free_irq(KEYBOARD_IRQ, &my_dev_id);
    printk(KERN_INFO "Goodbye, World!\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
