/*
 * Часть 2: то же самое, но последняя строка родителя выводится
 * только после завершения порожденного процесса (waitpid).
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    char *file = (argc > 1) ? argv[1] : "text.txt";
    pid_t pid;
    int status;

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0)
    {
        /* Потомок: заменяем образ процесса на cat */
        execlp("cat", "cat", file, (char *)0);
        perror("execlp");
        exit(127);
    }

    /* Родитель: этот текст может появиться в любом месте вывода cat */
    printf("Parent: child process created, pid = %ld\n", (long)pid);
    fflush(stdout);

    /* Ждем завершения потомка */
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        exit(1);
    }

    if (WIFEXITED(status))
        printf("\nParent: child exited with status %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("\nParent: child killed by signal %d\n", WTERMSIG(status));

    /* Последняя строка родителя — строго после завершения потомка */
    printf("Parent: last line, printed after child terminated\n");

    return 0;
}
