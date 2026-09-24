#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <limits.h>
#include <string.h>

extern char **environ;


struct option_item
{
    char option;
    char *argument;
};


void print_ids()
{
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    printf("Real GID: %d\n", getgid());
    printf("Effective GID: %d\n", getegid());
}


void become_group_leader()
{
    if (setpgid(0, 0) == -1)
        perror("setpgid");
    else
        printf("Process became group leader\n");
}


void print_process_info()
{
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());
    printf("PGID: %d\n", getpgrp());
}


void print_limit()
{
    struct rlimit limit;

    if (getrlimit(RLIMIT_FSIZE, &limit) == 0)
    {
        if (limit.rlim_cur == RLIM_INFINITY)
            printf("Limit: unlimited\n");
        else
            printf("Limit: %ld bytes\n", limit.rlim_cur);
    }
    else
    {
        perror("getrlimit");
    }
}


void change_limit(char *value)
{
    struct rlimit limit;

    limit.rlim_cur = atol(value);
    limit.rlim_max = atol(value);

    if (setrlimit(RLIMIT_FSIZE, &limit) == 0)
        printf("Limit changed to %s\n", value);
    else
        perror("setrlimit");
}


void print_core_size()
{
    struct rlimit limit;

    if (getrlimit(RLIMIT_CORE, &limit) == 0)
        printf("Core size: %ld bytes\n", limit.rlim_cur);
    else
        perror("getrlimit");
}


void change_core_size(char *value)
{
    struct rlimit limit;

    limit.rlim_cur = atol(value);
    limit.rlim_max = atol(value);

    if (setrlimit(RLIMIT_CORE, &limit) == 0)
        printf("Core size changed to %s\n", value);
    else
        perror("setrlimit");
}


void print_directory()
{
    char path[PATH_MAX];

    if (getcwd(path, sizeof(path)))
        printf("Directory: %s\n", path);
    else
        perror("getcwd");
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
    if (putenv(value) == 0)
        printf("Environment changed: %s\n", value);
    else
        perror("putenv");
}



void execute_option(struct option_item item)
{
    switch(item.option)
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
            change_limit(item.argument);
            break;


        case 'c':
            print_core_size();
            break;


        case 'C':
            change_core_size(item.argument);
            break;


        case 'd':
            print_directory();
            break;


        case 'v':
            print_environment();
            break;


        case 'V':
            change_environment(item.argument);
            break;


        default:
            printf("Unknown option\n");
            break;
    }
}



int main(int argc, char *argv[])
{
    struct option_item options[100];

    int count = 0;
    int opt;


    while ((opt = getopt(argc, argv, "ispucC:dvV:U:")) != -1)
    {
        options[count].option = opt;

        if (optarg)
            options[count].argument = optarg;
        else
            options[count].argument = NULL;

        count++;
    }


    for (int i = count - 1; i >= 0; i--)
    {
        execute_option(options[i]);
    }


    return 0;
}