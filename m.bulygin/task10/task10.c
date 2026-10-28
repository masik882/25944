/*
 * Запускает команду, заданную первым аргументом, в порожденном процессе.
 * Остальные аргументы передаются этой команде.
 * Родитель ждет завершения потомка и печатает его код завершения.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
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
        /* Потомок: argv[1] — имя команды, &argv[1] — ее argv (с NULL в конце) */
        execvp(argv[1], &argv[1]);
        perror(argv[1]);
        exit(127);
    }

    /* Родитель: ждем завершения потомка */
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        exit(1);
    }

    if (WIFEXITED(status))
        printf("Child %ld exited with code %d\n", (long)pid, WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("Child %ld killed by signal %d\n", (long)pid, WTERMSIG(status));
    else
        printf("Child %ld finished with status 0x%x\n", (long)pid, status);

    return 0;
}
