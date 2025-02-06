#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "message_slot.h"

int main(int argc, char *argv[]){
    int fd; 
    int ret_val;
    char *slot_path;
    unsigned long channel_id;
    char message[MAX_MESSAGE_SIZE]; // Buffer to read the message into
    
    if (argc != 3)
    {
        perror("Invalid arguments");
        return 1;
    }
    
    slot_path = argv[1]; // Path to the message slot device
    channel_id = atoi(argv[2]); // Channel ID to read from

    fd = open(slot_path, O_RDONLY);
    if (fd < 0)
    {
        perror("Error opening device file");
        return 1;
    }

    ret_val = ioctl(fd, MSG_SLOT_CHANNEL, channel_id);
    if (ret_val < 0)
    {
        perror("Error setting channel");
        close(fd);
        return 1;
    }

    ret_val = read(fd, message, MAX_MESSAGE_SIZE); // Read the message from the device
    if (ret_val < 0)
    {
        perror("Error reading from device");
        close(fd);
        return 1;
    }

    close(fd);
    
    if (write(STDOUT_FILENO, message, ret_val) < 0) // Write the message to stdout
    {
        perror("Error writing to stdout");
        return 1;
    }

    return 0;
}