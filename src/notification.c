#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "notification.h"

#include "log.h"
#include "socket.h"
#include "proto/build_complete.pb-c.h"

void notify(fd_list **subs, pid_t child, int status) {
    info("notifying build exit\n");
    BuildComplete msg = BUILD_COMPLETE__INIT;
    size_t len;
    int msg_type = htonl(MSG_BUILD_COMPLETE);

    msg.pid = child;
    /* TODO fetch nonce */
    msg.nonce = 0;
    msg.status = status;

    len = build_complete__get_packed_size(&msg);

#define HEADER_LENGTH 2 + 2 + 4
    /* 2 bytes for mask
     * 2 bytes for type
     * 4 bytes for length
     * len for buffer */
    uint8_t *buf = malloc(HEADER_LENGTH + len);
    len = htonl(len);

    memcpy(buf, &msg_type, 4);
    memcpy(buf + 4, &len, 4);
    build_complete__pack(&msg, buf + 8);
    len = ntohl(len);

    fd_list *sub_node, *next;
    if ((sub_node = subs[child]) != NULL) {
        while(sub_node != NULL) {
            info("notifying fd %d\n", sub_node->fd);
            send(sub_node->fd, buf, len + HEADER_LENGTH, 0);
            next = sub_node->next;
            free(sub_node);
            sub_node = next;
        }
    }
    subs[child] = NULL;
    free(buf);
#undef HEADER_LENGTH
}
