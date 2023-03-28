#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define check_error(cond, msg)\
    do\
    {\
        if (!(cond))\
        {\
            perror(msg);\
            fprintf(stderr, "file: %s\nfunc: %s\nline: %d\n", __FILE__, __func__, __LINE__);\
            exit(EXIT_FAILURE);\
        }\
    } while (0)


int main(int argc, char **argv)
{
    check_error(3 == argc, "./1 sec inc");

    time_t sec = atol(argv[1]);
    time_t min_increment = atol(argv[2]);

    sec += min_increment * 60;

    struct tm *broken_time = localtime(&sec);
    check_error(NULL != broken_time, "localtime");

    // man localtime (pogledati deo o struct tm)
    printf("%02d/%02d/%d %02d:%02d\n", broken_time->tm_mday, broken_time->tm_mon + 1, broken_time->tm_year + 1900, broken_time->tm_hour, broken_time->tm_min);

    return 0;
}
