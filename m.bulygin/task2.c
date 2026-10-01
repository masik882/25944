#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main()
{
    time_t now;
    struct tm pst_time;
    struct tm pdt_time;

    (void) time(&now);

    setenv("TZ", "PST8", 1);
    tzset();

    pst_time = *localtime(&now);

    setenv("TZ", "PDT7", 1);
    tzset();

    pdt_time = *localtime(&now);

    printf("California time:\n");

    printf("PST: %02d/%02d/%04d %02d:%02d\n",
           pst_time.tm_mon + 1,
           pst_time.tm_mday,
           pst_time.tm_year + 1900,
           pst_time.tm_hour,
           pst_time.tm_min);

    printf("PDT: %02d/%02d/%04d %02d:%02d\n",
           pdt_time.tm_mon + 1,
           pdt_time.tm_mday,
           pdt_time.tm_year + 1900,
           pdt_time.tm_hour,
           pdt_time.tm_min);

    exit(0);
}