#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>

#define MAX_LEN 40

#define CTRL_D 0x04
#define CTRL_G 0x07
#define CTRL_W 0x17

struct termios saved_tty;

char line[MAX_LEN];
int len = 0;

void restore_tty(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_tty);
}

void signal_handler(int sig)
{
    restore_tty();
    _exit(1);
}

void put_char(char c)
{
    write(STDOUT_FILENO, &c, 1);
}

void erase_char(void)
{
    if (len > 0)
    {
        write(STDOUT_FILENO, "\b \b", 3);
        len--;
    }
}

void kill_line(void)
{
    while (len > 0)
    {
        erase_char();
    }
}

void erase_word(void)
{
    while (len > 0 && line[len - 1] == ' ')
    {
        erase_char();
    }

    while (len > 0 && line[len - 1] != ' ')
    {
        erase_char();
    }
}

void new_line(void)
{
    put_char('\n');
    len = 0;
}

void add_char(char c)
{
    char word[MAX_LEN];
    int start;
    int word_len;
    int i;

    if (len == MAX_LEN)
    {
        if (c == ' ')
        {
            new_line();
            return;
        }

        start = len;

        while (start > 0 && line[start - 1] != ' ')
        {
            start--;
        }

        word_len = len - start;

        if (start == 0)
        {
            new_line();
        }
        else
        {
            memcpy(word, line + start, word_len);

            for (i = 0; i < word_len; i++)
            {
                erase_char();
            }

            new_line();

            for (i = 0; i < word_len; i++)
            {
                line[len++] = word[i];
                put_char(word[i]);
            }
        }
    }

    line[len++] = c;
    put_char(c);
}

int main(void)
{
    struct termios tty;
    unsigned char c;
    ssize_t n;

    if (!isatty(STDIN_FILENO))
    {
        fprintf(stderr, "stdin is not a terminal\n");
        exit(1);
    }

    if (tcgetattr(STDIN_FILENO, &saved_tty) == -1)
    {
        perror("tcgetattr");
        exit(1);
    }

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    tty = saved_tty;
    tty.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    tty.c_iflag &= ~(IXON);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty) == -1)
    {
        perror("tcsetattr");
        exit(1);
    }

    atexit(restore_tty);

    while ((n = read(STDIN_FILENO, &c, 1)) == 1)
    {
        if (c == saved_tty.c_cc[VERASE])
        {
            erase_char();
        }
        else if (c == saved_tty.c_cc[VKILL])
        {
            kill_line();
        }
        else if (c == CTRL_W)
        {
            erase_word();
        }
        else if (c == CTRL_D)
        {
            if (len == 0)
            {
                break;
            }

            put_char(CTRL_G);
        }
        else if (c == '\n')
        {
            new_line();
        }
        else if (isprint(c))
        {
            add_char((char)c);
        }
        else
        {
            put_char(CTRL_G);
        }
    }

    if (n == -1)
    {
        perror("read");
        exit(1);
    }

    return 0;
}
