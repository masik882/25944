#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "task31.sock"

int main(void)
{
    int fd;
    struct sockaddr_un addr;
    char buffer[256];
    ssize_t n;
    ssize_t total;
    ssize_t written;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("connect");
        close(fd);
        exit(1);
    }

    while ((n = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0)
    {
        total = 0;

        while (total < n)
        {
            written = write(fd, buffer + total, n - total);

            if (written == -1)
            {
                perror("write");
                close(fd);
                exit(1);
            }

            total += written;
        }
    }

    if (n == -1)
    {
        perror("read");
        close(fd);
        exit(1);
    }

    close(fd);

    return 0;
}
