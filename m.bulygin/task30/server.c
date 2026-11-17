#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "task30.sock"

int main(void)
{
    int listen_fd;
    int client_fd;
    struct sockaddr_un addr;
    char buffer[256];
    ssize_t n;
    ssize_t i;

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

    if (listen(listen_fd, 1) == -1)
    {
        perror("listen");
        close(listen_fd);
        unlink(SOCKET_PATH);
        exit(1);
    }

    client_fd = accept(listen_fd, NULL, NULL);
    if (client_fd == -1)
    {
        perror("accept");
        close(listen_fd);
        unlink(SOCKET_PATH);
        exit(1);
    }

    close(listen_fd);
    unlink(SOCKET_PATH);

    while ((n = read(client_fd, buffer, sizeof(buffer))) > 0)
    {
        for (i = 0; i < n; i++)
        {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }

        if (write(STDOUT_FILENO, buffer, n) == -1)
        {
            perror("write");
            close(client_fd);
            exit(1);
        }
    }

    if (n == -1)
    {
        perror("read");
        close(client_fd);
        exit(1);
    }

    close(client_fd);

    return 0;
}
