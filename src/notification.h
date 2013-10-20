#ifndef __JUICI_NOTIFICATION_H
#define __JUICI_NOTIFICATION_H

struct _fd_list {
    int fd;
    struct _fd_list *next;
};

typedef struct _fd_list fd_list;

void notify(fd_list **subs, pid_t child, int status);

#endif
