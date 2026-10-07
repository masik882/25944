#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void print_ids()
{
    printf("Real UID:      %ld\n", (long)getuid());
    printf("Effective UID: %ld\n", (long)geteuid());
}

void try_open_file(const char *filename)
{
    FILE *file;

    file = fopen(filename, "r+");

    if (file == NULL)
    {
        perror("fopen");
        return;
    }

    printf("File opened successfully\n");

    fclose(file);
}

int main()
{
    uid_t real_uid;

    printf("Before setuid():\n");
    print_ids();
    try_open_file("data.txt");

    real_uid = getuid();

    if (setuid(real_uid) == -1)
    {
        perror("setuid");
        return 1;
    }

    printf("\nAfter setuid():\n");
    print_ids();
    try_open_file("data.txt");

    return 0;
}