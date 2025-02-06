#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "message_slot.h"

int main(int argc, char *argv[])
{
    int fd;
    int ret_val;
    char *slot_path;
    unsigned long channel_id;
    char *message;

    if (argc != 4)
    {
        perror("Invalid arguments");
        return 1;
    }

    slot_path = argv[1]; // Path to the message slot device
    channel_id = atoi(argv[2]); // Channel ID to write to
    message = argv[3]; // Message to write

    fd = open(slot_path, O_WRONLY);
    if (fd < 0)
    {
        perror("Error opening device file");
        return 1;
    }

    ret_val = ioctl(fd, MSG_SLOT_CHANNEL, channel_id); // Set the channel to write to
    if (ret_val < 0)
    {
        perror("Error setting channel");
        close(fd);
        return 1;
    }

    ret_val = write(fd, message, strlen(message)); // Write the message to the device
    if (ret_val < 0)
    {
        perror("Error writing to device");
        close(fd);
        return 1;
    }

    close(fd);

    return 0;
}