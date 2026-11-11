#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pid_t pid;
    char buffer[256];
    ssize_t n;
    ssize_t i;
    size_t total;
    size_t length;
    int status;

    const char *text =
        "Hello, World!\n"
        "This Text Contains UPPER and lower Case letters.\n"
        "Pipe test: abcXYZ 123\n";

    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0)
    {
        close(fd[1]);

        while ((n = read(fd[0], buffer, sizeof(buffer))) > 0)
        {
            for (i = 0; i < n; i++)
            {
                buffer[i] = toupper((unsigned char)buffer[i]);
            }

            if (write(STDOUT_FILENO, buffer, n) == -1)
            {
                perror("write");
                exit(1);
            }
        }

        if (n == -1)
        {
            perror("read");
            exit(1);
        }

        close(fd[0]);
        exit(0);
    }

    close(fd[0]);

    length = strlen(text);
    total = 0;

    while (total < length)
    {
        n = write(fd[1], text + total, length - total);

        if (n == -1)
        {
            perror("write");
            close(fd[1]);
            exit(1);
        }

        total += (size_t)n;
    }

    close(fd[1]);

    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        exit(1);
    }

    return 0;
}
