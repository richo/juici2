#include <unistd.h>
#include <arpa/inet.h>
#include "notification.h"

#include "log.h"

void notify(fd_list **subs, pid_t child, int status) {
    info("notifying build exit\n");

    fd_list *sub_node;
    if ((sub_node = subs[child]) != NULL) {
        while(sub_node != NULL) {
            info("notifying fd %d\n", sub_node->fd);
            write(sub_node->fd, &status, sizeof(status));
            sub_node = sub_node->next;
        }
    }
}
