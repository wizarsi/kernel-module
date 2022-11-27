//
// Created by AYUVasilev on 27.11.2022.
//

#ifndef LAB2_IOCTL_H
#define LAB2_IOCTL_H

struct message_dentry {
    char *path_name;
    unsigned char* name;
    unsigned char* parent_name;
    unsigned int i_uid;
    unsigned int i_gid;
    unsigned int d_flags;
};

#define DENTRY_RW_VALUE _IOWR('a', 'a', struct message_dentry*)
#define BUF_SIZE 128

#endif //LAB2_IOCTL_H
