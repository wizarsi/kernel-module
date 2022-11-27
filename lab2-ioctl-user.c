#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "lab2-ioctl.h"

void print_dentry(struct message_dentry *msg_dentry);


int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Command line has no additional arguments\n");
        return 0;
    }
    printf("Directory path: %s\n", argv[1]);

    unsigned char name[BUF_SIZE];
    unsigned char parent_name[BUF_SIZE];
    struct message_dentry msg_dentry;
    msg_dentry.path_name = argv[1];
    msg_dentry.name = name;
    msg_dentry.parent_name = parent_name;

    int dev = open("/dev/lab2_device", O_RDWR);
    if (dev == -1) {
        printf("Opening was not possible!\n");
        return -1;
    }

    if (ioctl(dev, DENTRY_RW_VALUE, &msg_dentry) < 0) {
        printf("Exchange data was failure\n");
        return -2;
    }
    print_dentry((struct message_dentry *) &msg_dentry);
    if(close(dev)== -1){
        printf("Close device error");
    }
    return 0;
}

void print_dentry(struct message_dentry *msg_dentry) {
    printf("DENTRY:\n");
    printf("Dentry name: %s\n", msg_dentry->name);
    printf("Dentry parent name: %s\n", msg_dentry->parent_name);
    printf("Dentry inode UID: %d\n", msg_dentry->i_uid);
    printf("Dentry inode GID: %d\n", msg_dentry->i_gid);
    printf("Dentry element flags: %d\n", msg_dentry->d_flags);
}
