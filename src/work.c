#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#ifdef __linux
# include <sys/signalfd.h>
#elif __APPLE__
# include <sys/event.h>
#else
# error "Don't know how to wire up a signalfd on your platform"
#endif
#include <sys/wait.h>

#include "proto/build_payload.pb-c.h"

#include "socket.h"
#include "build.h"
#include "log.h"

void mainloop(int socket) {
    uint8_t butts[2048];
    int res, i;
    size_t rcvd;
    pid_t build;
    BuildPayload* msg;
    /* Setup FD sets for monitoring */
    fd_set fds;
    fd_set rfds, wfds, efds;
    FD_ZERO(&fds); FD_ZERO(&rfds); FD_ZERO(&wfds); FD_ZERO(&efds);

    FD_SET(socket, &fds);
    /* Block SIGCHLD set setup signalfd for monitoring */
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGCHLD);
    /* Block sigchld */
    sigprocmask(SIG_BLOCK, &signals, NULL);
    /* Setup a signalfd */
#ifdef __linux
    int sigfd = signalfd(-1, &signals, SFD_NONBLOCK |
                                       SFD_CLOEXEC);
    struct signalfd_siginfo child;
    size_t child_read;
#elif __APPLE__
    int sigfd = kqueue();
    int kq_status;
    struct kevent ke, child;
    EV_SET(&ke, SIGCHLD, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);
    i = kevent(sigfd, &ke, 1, NULL, 0, NULL);
    if (i == -1) {
        warn("Couldn't set up sigfd\n");
        return;
    }
#endif
    /* Add sigfd to the main set */
    FD_SET(sigfd, &fds);
    /* Structure to read children into */
    pid_t child_pid;
    int child_status;

    while(1) {
        FD_ZERO(&rfds); FD_ZERO(&wfds); FD_ZERO(&efds);

        memcpy(&rfds, &fds, sizeof(fd_set));
        memcpy(&wfds, &fds, sizeof(fd_set));
        memcpy(&efds, &fds, sizeof(fd_set));

        res = select(FD_SETSIZE, &rfds, NULL, &efds, NULL);
        info("got %d fds\n", res);
        if (res == 0) {
            // timeout wtf?
        } else if (res == -1) {
            warn("select call interrupted");
            break;
            // We probably saw a signal. Defer signals and do some stuff.
        } else {
            for (i = 0; i < FD_SETSIZE; ++i) {
                if (FD_ISSET(i, &efds)) {
                    debug("%d has made a whoopsie\n", i);
                }
                if (!FD_ISSET(i, &rfds))
                    continue;

                if (i == socket) {
                    accept_new_connection(socket, &fds);
                    info("Accepted a new connection\n");
                } else if (i == sigfd) {
#ifdef __linux
                    child_read = read(sigfd, &child, sizeof(child));
                    switch(child_read) {
                        case 0:
                        case -1:
                            warn("Couldn't read from signalfd");
                            continue;
                        default:
                            info("Recieved signal %d from %d\n",
                                    child.ssi_signo,
                                    child.ssi_pid);

                    }
                    if (child.ssi_signo == SIGCHLD) {
                        child_pid = waitpid(child.ssi_pid, &child_status, 0);
                        info("Child %d exited with status %d\n", child_pid, child_status);
                    }
#elif __APPLE__
                    kq_status = kevent(sigfd, NULL, 0, &child, 1, NULL);
                    if (kq_status == -1) {
                        warn("kevent read failed\n");
                        return;
                    }
                    info("Recieved signal %ld\n", child.ident);
                    child_pid = wait(&child_status);
                    info("Child %d exited with status %d\n", child_pid, child_status);
#endif
                } else {
                    // Check if the socket is still alive
                    if (recv(i, butts, 1, MSG_PEEK) < 1) {
                        close(i);
                        FD_CLR(i, &fds);
                        continue;
                    }
                    msg = load_payload(i);
                    if (!msg) {
                        info("Got a null payload from %d\n", i);
                        continue;
                    }
                    info("command -> %s\n", msg->command);
                    build = start_build(msg);
                    info("Started a new build from %d\n", i);
                }
            }
        }
    }
}
