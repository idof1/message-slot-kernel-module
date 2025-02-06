#ifndef MSG_SLOT_H
#define MSG_SLOT_H

#include <linux/ioctl.h>

// The major device number.
// We set this explicitly as required in the assignment.
#define MAJOR_NUM 235

// IOCTL command to set the channel of the message slot device
#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned long)

// Device name and buffer parameters
#define DEVICE_NAME "message_slot"
#define MAX_MESSAGE_SIZE 128
#define SUCCESS 0

#endif