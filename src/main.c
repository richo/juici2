#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "proto/build_payload.pb-c.h"

#include "log.h"
#include "socket.h"
#include "work.h"
#include "worktree.h"


int main(int argc, char** argv) {
    int sock = juici_socket();
    listen(sock, 16);

    if (init_worktree() != 0) {
        error("Couldn't init worktree\n");
        exit(1);
    }

    mainloop(sock);
}
