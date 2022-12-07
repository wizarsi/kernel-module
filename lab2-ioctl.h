//
// Created by AYUVasilev on 27.11.2022.
//

#ifndef LAB2_IOCTL_H
#define LAB2_IOCTL_H
#define BUF_SIZE 128

struct message_dentry {
    char *path_name;
    unsigned char name[BUF_SIZE];
    unsigned char parent_name[BUF_SIZE];
    unsigned int i_uid;
    unsigned int i_gid;
    unsigned int d_flags;
};

struct message_syscall_info {
    int pid;
    unsigned long  sp;
    int nr;
    unsigned long instruction_pointer;
    unsigned long args[6];
};

#define DENTRY_RW_VALUE _IOWR('a', 'a', struct message_dentry*)
#define SYSCALL_INFO_RW_VALUE _IOWR('a', 'b', struct message_dentry*)


#endif //LAB2_IOCTL_H
