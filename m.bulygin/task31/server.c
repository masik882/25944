#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "task31.sock"
#define MAX_CLIENTS 64

volatile sig_atomic_t stop = 0;

void stop_handler(int sig)
{
    stop = 1;
}

int main(void)
{
    int listen_fd;
    int client_fd;
    struct sockaddr_un addr;
    struct sigaction sa;
    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds;
    int ready;
    int i;
    int j;
    char buffer[256];
    ssize_t n;

    sa.sa_handler = stop_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1 ||
        sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        close(listen_fd);
        exit(1);
    }

    if (listen(listen_fd, 5) == -1)
    {
        perror("listen");
        close(listen_fd);
        unlink(SOCKET_PATH);
        exit(1);
    }

    fds[0].fd = listen_fd;
    fds[0].events = POLLIN;
    nfds = 1;

    while (!stop)
    {
        ready = poll(fds, nfds, -1);

        if (ready == -1)
        {
            if (errno == EINTR)
                continue;

            perror("poll");
            break;
        }

        for (i = nfds - 1; i >= 1; i--)
        {
            if (fds[i].revents == 0)
                continue;

            n = read(fds[i].fd, buffer, sizeof(buffer));

            if (n > 0)
            {
                for (j = 0; j < n; j++)
                {
                    buffer[j] = toupper((unsigned char)buffer[j]);
                }

                if (write(STDOUT_FILENO, buffer, n) == -1)
                {
                    perror("write");
                }
            }
            else
            {
                if (n == -1)
                    perror("read");

                close(fds[i].fd);
                nfds--;
                fds[i] = fds[nfds];
            }
        }

        if (fds[0].revents & POLLIN)
        {
            client_fd = accept(listen_fd, NULL, NULL);

            if (client_fd == -1)
            {
                if (errno != EINTR)
                    perror("accept");
            }
            else if (nfds == MAX_CLIENTS + 1)
            {
                fprintf(stderr, "Too many clients\n");
                close(client_fd);
            }
            else
            {
                fds[nfds].fd = client_fd;
                fds[nfds].events = POLLIN;
                fds[nfds].revents = 0;
                nfds++;
            }
        }
    }

    for (i = 0; i < nfds; i++)
    {
        close(fds[i].fd);
    }

    unlink(SOCKET_PATH);

    return 0;
}
