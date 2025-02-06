#include "message_slot.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdlib.h>

#define DEVICE_PATH "/dev/test0"
#define DEVICE_PATH_2 "/dev/test1"
#define MSG_SLOT_CHANNEL _IOW(235, 0, unsigned long)
#define BUFFER_SIZE 128

void test_no_ioctl_write();
void test_no_ioctl_read();
void test_valid_ioctl_write_read();
void test_multiple_channels();
void extended_tests();

int main() {
    printf("Running Combined Tester for Message Slot Module\n");
    printf("------------------------------------------------\n");

    test_no_ioctl_write();
    test_no_ioctl_read();
    test_valid_ioctl_write_read();
    test_multiple_channels();
    extended_tests();

    printf("\nAll tests completed!\n");
    return 0;
}

void test_no_ioctl_write() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }
    printf("Test 1: Write without ioctl\n");
    if (write(fd, "Test message", 12) < 0 && errno == EINVAL) {
        printf("Pass: Writing without ioctl returned EINVAL as expected\n");
    } else {
        printf("Fail: Writing without ioctl did not return EINVAL\n");
    }
    close(fd);
}

void test_no_ioctl_read() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }
    printf("Test 2: Read without ioctl\n");
    char buffer[BUFFER_SIZE];
    if (read(fd, buffer, BUFFER_SIZE) < 0 && errno == EINVAL) {
        printf("Pass: Reading without ioctl returned EINVAL as expected\n");
    } else {
        printf("Fail: Reading without ioctl did not return EINVAL\n");
    }
    close(fd);
}

void test_valid_ioctl_write_read() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }
    printf("Test 3: Write and read with valid ioctl\n");
    ioctl(fd, MSG_SLOT_CHANNEL, 1);
    write(fd, "Hello Kernel", 13);
    char buffer[BUFFER_SIZE] = {0};
    read(fd, buffer, BUFFER_SIZE);
    if (strcmp(buffer, "Hello Kernel") == 0) {
        printf("Pass: Message successfully written and read\n");
    } else {
        printf("Fail: Message was not properly read\n");
    }
    close(fd);
}

void test_multiple_channels() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }
    printf("Test 4: Multiple channels\n");
    ioctl(fd, MSG_SLOT_CHANNEL, 1);
    write(fd, "Channel 1 Msg", 13);
    ioctl(fd, MSG_SLOT_CHANNEL, 2);
    write(fd, "Channel 2 Msg", 13);
    ioctl(fd, MSG_SLOT_CHANNEL, 1);
    char buffer[BUFFER_SIZE] = {0};
    read(fd, buffer, BUFFER_SIZE);
    if (strcmp(buffer, "Channel 1 Msg") == 0) {
        printf("Pass: Channel switching works correctly\n");
    } else {
        printf("Fail: Channel switching does not work\n");
    }
    close(fd);
}

void extended_tests() {
    int fd = open(DEVICE_PATH_2, O_RDWR);
    if (fd < 0) {
        perror("Failed to open second device");
        return;
    }
    printf("Test 5: Extended edge cases\n");
    ioctl(fd, MSG_SLOT_CHANNEL, 99);
    write(fd, "Device 2 Msg", 12);
    char buffer[BUFFER_SIZE] = {0};
    read(fd, buffer, BUFFER_SIZE);
    if (strcmp(buffer, "Device 2 Msg") == 0) {
        printf("Pass: Multi-device support confirmed\n");
    } else {
        printf("Fail: Multi-device messaging failed\n");
    }
    close(fd);
}
