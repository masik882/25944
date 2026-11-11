#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t count = 0;
volatile sig_atomic_t quit = 0;

void sigint_handler(int sig)
{
    write(STDOUT_FILENO, "\a", 1);
    count++;
}

void sigquit_handler(int sig)
{
    quit = 1;
}

int main(void)
{
    struct sigaction sa;
    sigset_t block_set;
    sigset_t wait_set;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    sa.sa_handler = sigquit_handler;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = 0;

    if (sigaction(SIGQUIT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);
    sigaddset(&block_set, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &block_set, &wait_set) == -1)
    {
        perror("sigprocmask");
        exit(1);
    }

    sigdelset(&wait_set, SIGINT);
    sigdelset(&wait_set, SIGQUIT);

    while (!quit)
    {
        sigsuspend(&wait_set);
    }

    printf("\nThe bell rang %d times\n", (int)count);

    return 0;
}
