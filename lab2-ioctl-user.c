#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "messages.h"

void print_dentry(struct message_dentry *msg_dentry);
void print_syscall_info(struct message_syscall_info* info);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Command line must have file path and pid args\n");
        return 0;
    }

    printf("directory path: %s\n", argv[1]);
    printf("pid: %s\n\n", argv[2]);

    //file open
    int dev = open("/dev/lab2_device", O_RDWR);
    if (dev == -1) {
        printf("Opening was not possible!\n");
        return -1;
    }

    //dentry exchange

    struct message_dentry msg_dentry;
    msg_dentry.path_name = argv[1];

    if (ioctl(dev, DENTRY_RW_VALUE, &msg_dentry) < 0) {
        printf("Exchange dentry data was failure\n");
        return -2;
    }
    print_dentry((struct message_dentry *) &msg_dentry);

    //syscall_info exchange
    struct message_syscall_info msg_syscall_info;
    msg_syscall_info.pid = atoi(argv[2]);

    if (ioctl(dev, SYSCALL_INFO_RW_VALUE, &msg_syscall_info) < 0) {
        printf("Exchange syscall_info data was failure\n");
        return -3;
    }
    print_syscall_info(&msg_syscall_info);

    //file close
    if (close(dev) == -1) {
        printf("Close device error");
    }
    return 0;
}

void print_dentry(struct message_dentry *message_dentry) {
    printf("DENTRY:\n");
    printf("Name: %s\n", message_dentry->name);
    printf("Parent file name: %s\n", message_dentry->parent_name);
    printf("Inode UID: %d\n", message_dentry->i_uid);
    printf("Inode GID: %d\n", message_dentry->i_gid);
    printf("Element flags: %d\n\n", message_dentry->d_flags);
}

void print_syscall_info(struct message_syscall_info* message_syscall_info){
    printf("SYSCALL_INFO:\n");
    printf("Stack pointer: %lu\n", message_syscall_info->sp);
    printf("Instruction pointer: %lu\n", message_syscall_info->instruction_pointer);
    printf("The system call number: %d\n", message_syscall_info->nr);
    printf("Syscall arguments:\n1.%lu\n2.%lu\n3.%lu\n4.%lu\n5.%lu\n6.%lu\n",
           message_syscall_info->args[0],
           message_syscall_info->args[1],
           message_syscall_info->args[2],
           message_syscall_info->args[3],
           message_syscall_info->args[4],
           message_syscall_info->args[5]);
}