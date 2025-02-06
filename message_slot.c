#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE

#include <linux/kernel.h>  /* We're doing kernel work */
#include <linux/module.h>  /* Specifically, a module */
#include <linux/fs.h>      /* for register_chrdev */
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/string.h>  /* for memset. NOTE - not string.h!*/
#include <linux/slab.h>    /* for kmalloc and kfree */
#include <linux/ioctl.h>   /* for ioctl */

#include "message_slot.h"

MODULE_LICENSE("GPL");

typedef struct channel
{
    unsigned long channel_id;       // Channel ID
    char message[MAX_MESSAGE_SIZE]; // Message written to the channel
    size_t message_length;          // Length of the message
    struct channel *next;           // next channel
} channel_t;

typedef struct message_slot
{
    int minor_number;          // Minor Number
    struct channel *channels;  // List of channels in the slot
    struct message_slot *next; // next slot
} message_slot_t;

static message_slot_t *message_slots = NULL;

message_slot_t *create_slot(int minor_number)
{
    message_slot_t *slot = (message_slot_t *)kmalloc(sizeof(message_slot_t), GFP_KERNEL);
    if (!slot)
    {
        return NULL;
    }
    slot->minor_number = minor_number;
    slot->channels = NULL;
    slot->next = NULL;
    return slot;
}

message_slot_t *get_slot(int minor_number)
{
    message_slot_t *slot = message_slots;
    while (slot != NULL)
    {
        if (slot->minor_number == minor_number)
        {
            return slot;
        }
        slot = slot->next;
    }
    return NULL;
}

static int device_open(struct inode *inode, struct file *file)
{
    int minor_number;
    message_slot_t *slot;

    minor_number = iminor(inode);  // Minor Number
    slot = get_slot(minor_number); // Find the slot by Minor Number

    if (slot == NULL)
    {
        slot = create_slot(minor_number); // Create a new slot if it doesn't exist
        if (slot == NULL)
        {
            return -ENOMEM;
        }
        slot->next = message_slots;
        message_slots = slot;
    }

    file->private_data = NULL;
    return SUCCESS;
}

static ssize_t device_ioctl(struct file *file, unsigned int ioctl_command_id, unsigned long ioctl_param)
{
    struct inode *inode = file->f_inode;
    int minor_number = iminor(inode);

    message_slot_t *slot = get_slot(minor_number);
    if (slot == NULL)
    {
        printk(KERN_ERR "Slot not found\n");
        return -EINVAL;
    }

    channel_t *channel, *prev_channel = NULL;
    channel_t *new_channel;

    if (ioctl_command_id != MSG_SLOT_CHANNEL || ioctl_param == 0)
    {
        return -EINVAL;
    }

    channel = slot->channels;
    while (channel != NULL)
    {
        if (channel->channel_id == ioctl_param)
        {
            file->private_data = (void *)channel; // Save the channel in the file's private data
            return SUCCESS;
        }
        prev_channel = channel;
        channel = channel->next;
    }

    // Channel not found, create a new channel
    new_channel = (channel_t *)kmalloc(sizeof(channel_t), GFP_KERNEL);
    if (!new_channel)
    {
        printk(KERN_ERR "Failed to allocate memory for new channel\n");
        return -ENOMEM;
    }
    // Initialize the new channel
    new_channel->channel_id = ioctl_param;
    new_channel->message_length = 0;
    memset(new_channel->message, 0, MAX_MESSAGE_SIZE);
    new_channel->next = NULL;

    if (prev_channel)
    {
        prev_channel->next = new_channel;
    }
    else
    {
        slot->channels = new_channel;
    }

    file->private_data = (void *)new_channel; // Save the channel in the file's private data
    return SUCCESS;
}

static ssize_t device_read(struct file *file, char __user *buffer, size_t length, loff_t *offset)
{
    channel_t *channel;

    channel = (channel_t *)file->private_data;
    if (!channel)
    {
        return -EINVAL;
    }
    else if (channel->message_length == 0)
    {
        return -EWOULDBLOCK;
    }
    else if (length < channel->message_length)
    {
        return -ENOSPC;
    }
    if (copy_to_user(buffer, channel->message, channel->message_length))
    {
        return -EFAULT;
    }

    return channel->message_length;
}

static ssize_t device_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset)
{
    channel_t *channel;

    channel = (channel_t *)file->private_data;
    if (!channel)
    {
        return -EINVAL;
    }
    else if (length > MAX_MESSAGE_SIZE || length == 0)
    {
        return -EMSGSIZE;
    }
    if (copy_from_user(channel->message, buffer, length))
    {
        return -EFAULT;
    }
    channel->message_length = length;

    return length;
}

static void cleanup_slots(void)
{
    message_slot_t *slot;
    message_slot_t *next_slot;
    channel_t *channel;
    channel_t *next_channel;

    slot = message_slots;
    while (slot != NULL)
    {
        next_slot = slot->next;
        channel = slot->channels;
        while (channel != NULL)
        {
            next_channel = channel->next;
            kfree(channel);
            channel = next_channel;
        }
        kfree(slot);
        slot = next_slot;
    }
    message_slots = NULL;
}

struct file_operations Fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .unlocked_ioctl = device_ioctl,
};

static int __init simple_init(void)
{
    int major_num;

    major_num = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);
    if (major_num < 0)
    {
        printk(KERN_ERR "Registering char device failed with %d\n", major_num);
        return major_num;
    }
    return 0;
}

static void __exit simple_cleanup(void)
{
    cleanup_slots();
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}
module_init(simple_init);
module_exit(simple_cleanup);
