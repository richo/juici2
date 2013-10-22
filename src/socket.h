#ifndef __JUICI_SOCKET_H
#define __JUICI_SOCKET_H

#define JUICI_SOCKET_PATH "/tmp/juici.sock"

enum message_type {
    MSG_ERROR         = 0x00,
    MSG_BUILD_REQUEST = 0x01,
    MSG_BUILD_STARTED = 0x02,
    MSG_BUILD_COMPLETE = 0x03
};

int juici_socket(void);
BuildRequest* load_request(int sock);
void accept_new_connection(int socket, fd_set* fds);
enum message_type load_message_type(int sock);

#endif
