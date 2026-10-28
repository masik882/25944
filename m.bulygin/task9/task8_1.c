/*
 * Часть 1: родитель создает подпроцесс, который выполняет cat(1)
 * для длинного файла. Родитель НЕ ждет потомка, поэтому его вывод
 * перемешивается с выводом cat.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    char *file = (argc > 1) ? argv[1] : "text.txt";
    pid_t pid;

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

    /* Родитель: печатаем текст, не дожидаясь потомка */
    printf("Parent: child process created, pid = %ld\n", (long)pid);
    printf("Parent: this text may appear in the middle of cat output\n");
    printf("Parent: last line printed by parent\n");

    return 0;
}
