#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ulimit.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <errno.h>

#define MAX_OPT 256

extern char *optarg;
extern char **environ;

typedef struct {
    int opt;
    char *optarg;
} Opt_inf;


static void usage(const char *program)
{
    printf("Usage: %s [-i] [-s] [-p] [-u] [-U value] [-c] [-C size] [-d] [-v] [-V name=value]\n",
           program);
}


static int parse_nonneg_long(const char *s, long *out)
{
    char *endptr;
    long v;

    errno = 0;
    v = strtol(s, &endptr, 10);

    if (endptr == s || *endptr != '\0' || errno != 0 || v < 0)
        return -1;

    *out = v;
    return 0;
}


static long get_nproc_limit(void)
{
#ifdef RLIMIT_NPROC
    struct rlimit rl;

    if (getrlimit(RLIMIT_NPROC, &rl) == 0)
    {
        if (rl.rlim_cur == RLIM_INFINITY)
            return -2;

        return (long)rl.rlim_cur;
    }
#endif

    return sysconf(_SC_CHILD_MAX);
}


static long get_core_limit_ulimit(void)
{
    struct rlimit rl;

    if (getrlimit(RLIMIT_CORE, &rl) == -1)
        return -1;

    if (rl.rlim_cur == RLIM_INFINITY)
        return -2;

    return (long)(rl.rlim_cur / 1024);
}


static void print_nproc(void)
{
    long n = get_nproc_limit();

    if (n == -2)
        printf("unlimited\n");
    else if (n < 0)
        perror("ulimit -u");
    else
        printf("%ld\n", n);
}


static void print_core(void)
{
    long n = get_core_limit_ulimit();

    if (n == -2)
        printf("unlimited\n");
    else if (n < 0)
        perror("ulimit -c");
    else
        printf("%ld\n", n);
}


int main(int argc, char *argv[])
{
    Opt_inf opts[MAX_OPT];

    int opt_count = 0;
    int c;

    if (argc == 1)
    {
        usage(argv[0]);
        return 0;
    }

    opterr = 0;

    while ((c = getopt(argc, argv, ":ispuU:cC:dvV:")) != -1)
    {
        if (c == '?')
        {
            fprintf(stderr, "unknown option: %s\n", argv[optind - 1]);
            usage(argv[0]);
            return 1;
        }

        if (c == ':')
        {
            fprintf(stderr, "unknown option: -%c requires an argument\n",
                    optopt);
            usage(argv[0]);
            return 1;
        }

        if (opt_count < MAX_OPT)
        {
            opts[opt_count].opt = c;
            opts[opt_count].optarg = optarg;
            opt_count++;
        }
        else
        {
            fprintf(stderr, "Maximum options is limited: %d\n", MAX_OPT);
            return 1;
        }
    }

    if (optind < argc)
    {
        fprintf(stderr, "unknown option: %s\n", argv[optind]);
        usage(argv[0]);
        return 1;
    }

    for (int i = opt_count - 1; i >= 0; i--)
    {
        switch (opts[i].opt)
        {
            case 'i':
                printf("Real UID: %d, Effective UID: %d\n",
                       getuid(), geteuid());
                printf("Real GID: %d, Effective GID: %d\n",
                       getgid(), getegid());
                break;

            case 's':
                if (setpgid(0, 0) == -1)
                    perror("setpgid");
                else
                    printf("New PGID: %d\n", (int)getpgrp());
                break;

            case 'p':
                printf("PID: %d, PPID: %d, PGID: %d\n",
                       (int)getpid(),
                       (int)getppid(),
                       (int)getpgrp());
                break;

            case 'u':
                print_nproc();
                break;

            case 'U':
            {
                long value;

                if (parse_nonneg_long(opts[i].optarg, &value) == -1)
                {
                    printf("Invalid value\n");
                    break;
                }

#ifdef RLIMIT_NPROC
                {
                    struct rlimit rl;

                    if (getrlimit(RLIMIT_NPROC, &rl) == 0)
                    {
                        rl.rlim_cur = (rlim_t)value;

                        if (setrlimit(RLIMIT_NPROC, &rl) == -1)
                            perror("setrlimit");
                    }
                }
#endif
                printf("%ld\n", value);
                break;
            }

            case 'c':
                print_core();
                break;

            case 'C':
            {
                struct rlimit rl;
                long value;

                if (parse_nonneg_long(opts[i].optarg, &value) == -1)
                {
                    printf("Invalid value\n");
                    break;
                }

                if (getrlimit(RLIMIT_CORE, &rl) == 0)
                {
                    rl.rlim_cur = (rlim_t)(value * 1024);

                    if (setrlimit(RLIMIT_CORE, &rl) == 0)
                        printf("%ld\n", value);
                    else
                        perror("setrlimit");
                }

                break;
            }

            case 'd':
            {
                char cwd[1024];

                if (getcwd(cwd, sizeof(cwd)))
                    printf("Current dir: %s\n", cwd);
                else
                    perror("getcwd");

                break;
            }

            case 'v':
            {
                char **env;

                for (env = environ; *env != NULL; env++)
                    printf("%s\n", *env);

                break;
            }

            case 'V':
                if (putenv(opts[i].optarg) == 0)
                    printf("Added/changed: %s\n", opts[i].optarg);
                else
                    perror("putenv");

                break;
        }
    }

    return 0;
}