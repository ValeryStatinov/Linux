#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Valery Kernel Professional");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");
#define DEVICE_NAME "lkm_example"
#define EXAMPLE_MSG "Hello, World!\0"
#define MSG_BUFFER_LEN 15
/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;
static int device_open_count = 0;
static char msg_buffer[128];

/* Create phone book */
struct Contact {
    char last_name[28];
    char rest_info[100];
    struct list_head list;
};

struct Contact phone_book;

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};
/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    int bytes_read = 0;
 /* If we’re at the end, loop back to the beginning */
 /* Put data in the buffer */
    copy_to_user(buffer, msg_buffer, strlen(msg_buffer) + 1);
    return bytes_read;
}
/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    struct Contact *contact;
    struct Contact *tmp;
    int length = 0;
    int i = 0;
    if (!strncmp(buffer, "get", 3)) {
        buffer = buffer + 4;
        strcpy(msg_buffer, "No such contact in phone book:(");
        list_for_each_entry(contact, &phone_book.list, list) {
            if (!strncmp(contact->last_name, buffer, strlen(buffer))) {
                strncpy(msg_buffer, contact->last_name, strlen(contact->last_name));
                length += strlen(contact->last_name);
                strncpy(msg_buffer + length, contact->rest_info, strlen(contact->rest_info) + 1);
            }
        }
        return 0;
    }
    if (!strncmp(buffer, "add", 3)) {
        buffer = buffer + 4;
        contact = kmalloc(sizeof(*contact), GFP_KERNEL);
        memset(contact->last_name, 0, 28);
        memset(contact->rest_info, 0, 100);
        while(buffer[i] != ' ') {
            contact->last_name[i] = buffer[i];
            ++i;
        }
        contact->last_name[i] = '\0';
        strcat(contact->rest_info, buffer + i);
        INIT_LIST_HEAD(&contact->list);
        list_add_tail(&(contact->list), &(phone_book.list));
        return 0;
    }
    if (!strncmp(buffer, "del", 3)) {
        buffer = buffer + 4;
        list_for_each_entry_safe(contact, tmp, &phone_book.list, list){
            if (!strncmp(contact->last_name, buffer, strlen(buffer))) {
                list_del(&contact->list);
                kfree(contact);
            }
        }
        return 0;
    }
    printk(KERN_ALERT "Unsupported command\n");
    return -EINVAL;
}
/* Called when a process opens our device */
static int device_open(struct inode *inode, struct file *file) {
 /* If device is open, return busy */
    if (device_open_count) {
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}
/* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file) {
 /* Decrement the open counter and usage count. Without this, the module would not unload. */
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}
static int __init lkm_example_init(void) {
    struct Contact *new_contact;
    INIT_LIST_HEAD(&phone_book.list);
    new_contact = kmalloc(sizeof(*new_contact), GFP_KERNEL);
    strcpy(new_contact->last_name, "Statinov ");
    strcpy(new_contact->rest_info, "Valery 22 +79254500123 valery.statinov@gmail.com\0");
    INIT_LIST_HEAD(&new_contact->list);
    list_add_tail(&(new_contact->list), &(phone_book.list));

    strcpy(msg_buffer, "Hello, initial message\n");

    major_num = register_chrdev(0, "lkm_example", &file_ops);
    if (major_num < 0) {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    }
    else {
        printk(KERN_INFO "lkm_example module loaded with device major number %d\n", major_num);
        return 0;
    }
}
static void __exit lkm_example_exit(void) {
 /* Remember — we have to clean up after ourselves. Unregister the character device. */
    struct Contact *aContact, *tmp;
    printk(KERN_INFO "kernel module unloaded.\n");
    printk(KERN_INFO "deleting the list using list_for_each_entry_safe()\n");
    list_for_each_entry_safe(aContact, tmp, &phone_book.list, list){
        printk(KERN_INFO "freeing node %s\n", aContact->last_name);
        list_del(&aContact->list);
        kfree(aContact);
    }
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Goodbye, World!\n");
}
/* Register module functions */
module_init(lkm_example_init);
module_exit(lkm_example_exit);