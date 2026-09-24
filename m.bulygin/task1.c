#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <limits.h>
#include <string.h>

extern char **environ;


void print_ids()
{
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    printf("Real GID: %d\n", getgid());
    printf("Effective GID: %d\n", getegid());
}


void print_process_info()
{
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());
    printf("PGID: %d\n", getpgrp());
}


void become_group_leader()
{
    if (setpgid(0, 0) == -1)
    {
        perror("setpgid");
    }
    else
    {
        printf("Process became group leader\n");
    }
}


void print_limit()
{
    struct rlimit lim;

    if (getrlimit(RLIMIT_FSIZE, &lim) == -1)
    {
        perror("getrlimit");
        return;
    }

    printf("File size limit: Unlimited");
}


void change_limit(char *value)
{
    struct rlimit lim;

    lim.rlim_cur = atol(value);
    lim.rlim_max = atol(value);

    if (setrlimit(RLIMIT_FSIZE, &lim) == -1)
    {
        perror("setrlimit");
    }
    else
    {
        printf("Limit changed\n");
    }
}


void print_core_size()
{
    struct rlimit lim;

    if (getrlimit(RLIMIT_CORE, &lim) == -1)
    {
        perror("getrlimit");
        return;
    }

    printf("Core size: %ld bytes\n", lim.rlim_cur);
}


void change_core_size(char *value)
{
    struct rlimit lim;

    lim.rlim_cur = atol(value);
    lim.rlim_max = atol(value);

    if (setrlimit(RLIMIT_CORE, &lim) == -1)
    {
        perror("setrlimit");
    }
    else
    {
        printf("Core size changed\n");
    }
}


void print_directory()
{
    char path[PATH_MAX];

    if (getcwd(path, sizeof(path)) != NULL)
    {
        printf("Current directory: %s\n", path);
    }
    else
    {
        perror("getcwd");
    }
}


void print_environment()
{
    char **env = environ;

    while (*env)
    {
        printf("%s\n", *env);
        env++;
    }
}


void change_environment(char *value)
{
    if (putenv(value) != 0)
    {
        perror("putenv");
    }
    else
    {
        printf("Environment changed: %s\n", value);
    }
}


int main(int argc, char *argv[])
{
    int opt;


    while ((opt = getopt(argc, argv, "ispucC:dvV:U:")) != -1)
    {
        switch(opt)
        {

        case 'i':
            print_ids();
            break;


        case 's':
            become_group_leader();
            break;


        case 'p':
            print_process_info();
            break;


        case 'u':
            print_limit();
            break;


        case 'U':
            change_limit(optarg);
            break;


        case 'c':
            print_core_size();
            break;


        case 'C':
            change_core_size(optarg);
            break;


        case 'd':
            print_directory();
            break;


        case 'v':
            print_environment();
            break;


        case 'V':
            change_environment(optarg);
            break;


        default:
            printf("Unknown option\n");
            return 1;
        }
    }


    return 0;
}