#!/bin/bash

# Compile the combined tester
gcc -O3 -Wall -std=c11 tester.c -o tester || { echo "Compilation failed"; exit 1; }

# Load the module
sudo insmod message_slot.ko || { echo "Failed to insert module"; exit 1; }

# Create device nodes
sudo mknod /dev/test0 c 235 0 || { echo "Failed to create /dev/test0"; exit 1; }
sudo mknod /dev/test1 c 235 1 || { echo "Failed to create /dev/test1"; exit 1; }
sudo chmod 0777 /dev/test0 /dev/test1

# Run the tester
./tester || { echo "Tests failed"; exit 1; }

# Unload the module
sudo rmmod message_slot || { echo "Failed to remove module"; exit 1; }

# Cleanup
echo "All tests completed successfully."
