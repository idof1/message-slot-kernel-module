# Message Slot Kernel Module

This project implements a Linux kernel module that provides an inter-process communication (IPC) mechanism using message slots. A message slot is a character device file that allows processes to communicate via channels. Each message slot can have multiple channels, and messages can be written and read persistently.

## Features
- Implements a Linux kernel module for IPC.
- Uses `ioctl` to select a message channel.
- Supports multiple message slots with distinct minor numbers.
- Messages are persistent until overwritten.
- Includes user-space programs to send and receive messages.
- Provides a `Makefile` for easy compilation and installation.

## Files Overview

| File | Description |
|------|-------------|
| `message_slot.c` | Kernel module implementation. |
| `message_slot.h` | Header file for the module. |
| `message_sender.c` | User-space program to send messages. |
| `message_reader.c` | User-space program to read messages. |
| `Makefile` | Compilation and installation script. |
| `test.sh` | Automated test script. |
| `LICENSE` | MIT License file. |

## Compilation & Installation

### 1. Compile the Kernel Module
```bash
make
```

### 2. Load the Module
```bash
sudo insmod message_slot.ko
```

### 3. Create a Message Slot Device File
```bash
sudo mknod /dev/slot0 c 235 0
sudo chmod 666 /dev/slot0
```

### 4. Send a Message
```bash
./message_sender /dev/slot0 1 "Hello, Kernel!"
```

### 5. Read the Message
```bash
./message_reader /dev/slot0 1
```

### 6. Remove the Module
```bash
sudo rmmod message_slot
```

## Running Tests
A script is provided to test the module automatically:
```bash
./test.sh
```

