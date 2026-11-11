#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIX_MONTHS (6L * 30 * 24 * 60 * 60)

char file_type(mode_t mode)
{
    if (S_ISDIR(mode))
        return 'd';
    if (S_ISREG(mode))
        return '-';
    return '?';
}

void file_perms(mode_t mode, char *buf)
{
    buf[0] = (mode & S_IRUSR) ? 'r' : '-';
    buf[1] = (mode & S_IWUSR) ? 'w' : '-';
    buf[2] = (mode & S_IXUSR) ? 'x' : '-';
    buf[3] = (mode & S_IRGRP) ? 'r' : '-';
    buf[4] = (mode & S_IWGRP) ? 'w' : '-';
    buf[5] = (mode & S_IXGRP) ? 'x' : '-';
    buf[6] = (mode & S_IROTH) ? 'r' : '-';
    buf[7] = (mode & S_IWOTH) ? 'w' : '-';
    buf[8] = (mode & S_IXOTH) ? 'x' : '-';
    buf[9] = '\0';
}

void base_name(const char *path, char *buf, size_t size)
{
    size_t end = strlen(path);
    size_t start;
    size_t n;

    while (end > 1 && path[end - 1] == '/')
    {
        end--;
    }

    start = end;

    while (start > 0 && path[start - 1] != '/')
    {
        start--;
    }

    if (start == end)
    {
        start = 0;
    }

    n = end - start;

    if (n >= size)
    {
        n = size - 1;
    }

    memcpy(buf, path + start, n);
    buf[n] = '\0';
}

int print_file(const char *path)
{
    struct stat st;
    struct passwd *pw;
    struct group *gr;
    char perms[10];
    char owner[32];
    char group[32];
    char size[32];
    char date[32];
    char name[256];
    time_t now;

    if (lstat(path, &st) == -1)
    {
        perror(path);
        return -1;
    }

    file_perms(st.st_mode, perms);

    pw = getpwuid(st.st_uid);
    if (pw != NULL)
        sprintf(owner, "%.31s", pw->pw_name);
    else
        sprintf(owner, "%ld", (long)st.st_uid);

    gr = getgrgid(st.st_gid);
    if (gr != NULL)
        sprintf(group, "%.31s", gr->gr_name);
    else
        sprintf(group, "%ld", (long)st.st_gid);

    if (S_ISREG(st.st_mode))
        sprintf(size, "%lld", (long long)st.st_size);
    else
        size[0] = '\0';

    now = time(NULL);
    if (st.st_mtime > now - SIX_MONTHS && st.st_mtime <= now)
        strftime(date, sizeof(date), "%b %e %H:%M", localtime(&st.st_mtime));
    else
        strftime(date, sizeof(date), "%b %e  %Y", localtime(&st.st_mtime));

    base_name(path, name, sizeof(name));

    printf("%c%s %3lu %-8s %-8s %10s %s %s\n",
           file_type(st.st_mode),
           perms,
           (unsigned long)st.st_nlink,
           owner,
           group,
           size,
           date,
           name);

    return 0;
}

int main(int argc, char *argv[])
{
    int status = 0;
    int i;

    if (argc < 2)
    {
        if (print_file(".") == -1)
            status = 1;
        return status;
    }

    for (i = 1; i < argc; i++)
    {
        if (print_file(argv[i]) == -1)
            status = 1;
    }

    return status;
}
