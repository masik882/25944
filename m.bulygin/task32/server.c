#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <aio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "task32.sock"
#define MAX_CLIENTS 64
#define BUF_SIZE 256

struct client
{
    volatile sig_atomic_t in_use;
    int fd;
    struct aiocb cb;
    char buffer[BUF_SIZE];
};

struct client clients[MAX_CLIENTS];

int start_read(struct client *c)
{
    memset(&c->cb, 0, sizeof(c->cb));
    c->cb.aio_fildes = c->fd;
    c->cb.aio_buf = c->buffer;
    c->cb.aio_nbytes = BUF_SIZE;
    c->cb.aio_offset = 0;
    c->cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    c->cb.aio_sigevent.sigev_signo = SIGRTMIN;
    c->cb.aio_sigevent.sigev_value.sival_ptr = c;

    return aio_read(&c->cb);
}

void close_client(struct client *c)
{
    close(c->fd);
    c->in_use = 0;
}

void aio_handler(int sig, siginfo_t *info, void *context)
{
    struct client *c;
    int saved_errno = errno;
    int err;
    ssize_t n;
    ssize_t i;

    c = (struct client *)info->si_value.sival_ptr;

    if (c == NULL || !c->in_use)
    {
        errno = saved_errno;
        return;
    }

    err = aio_error(&c->cb);

    if (err == EINPROGRESS)
    {
        errno = saved_errno;
        return;
    }

    n = aio_return(&c->cb);

    if (err == 0 && n > 0)
    {
        for (i = 0; i < n; i++)
        {
            c->buffer[i] = toupper((unsigned char)c->buffer[i]);
        }

        write(STDOUT_FILENO, c->buffer, n);

        if (start_read(c) == -1)
        {
            close_client(c);
        }
    }
    else
    {
        close_client(c);
    }

    errno = saved_errno;
}

void stop_handler(int sig)
{
    unlink(SOCKET_PATH);
    _exit(0);
}

int main(void)
{
    int listen_fd;
    int client_fd;
    struct sockaddr_un addr;
    struct sigaction sa;
    int i;

    sa.sa_sigaction = aio_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGRTMIN, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

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

    while (1)
    {
        client_fd = accept(listen_fd, NULL, NULL);

        if (client_fd == -1)
        {
            if (errno == EINTR)
                continue;

            perror("accept");
            break;
        }

        for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (!clients[i].in_use)
                break;
        }

        if (i == MAX_CLIENTS)
        {
            fprintf(stderr, "Too many clients\n");
            close(client_fd);
            continue;
        }

        clients[i].fd = client_fd;
        clients[i].in_use = 1;

        if (start_read(&clients[i]) == -1)
        {
            perror("aio_read");
            close_client(&clients[i]);
        }
    }

    close(listen_fd);
    unlink(SOCKET_PATH);

    return 1;
}
