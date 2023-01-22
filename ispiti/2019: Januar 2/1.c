#include <stdio.h>
#include <stdlib.h>
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
    check_error(2 == argc, "./1 sec");
    
    time_t num_of_sec = atol(argv[1]);
    struct tm *broken_time = localtime(&num_of_sec);
    check_error(NULL != broken_time, "localtime");

    if (broken_time->tm_hour == 0 && broken_time->tm_min == 0)
    {
        printf("vece");
    }
    else if (broken_time->tm_hour < 7 || (broken_time->tm_hour == 7 && broken_time->tm_min == 0))
    {
        printf("noc");
    }
    else if (broken_time->tm_hour < 9 || (broken_time->tm_hour == 9 && broken_time->tm_min == 0))
    {
        printf("jutro");
    }
    else if (broken_time->tm_hour < 12 || (broken_time->tm_hour == 12 && broken_time->tm_min == 0))
    {
        printf("prepodne");
    }
    else if (broken_time->tm_hour < 19 || (broken_time->tm_hour == 19 && broken_time->tm_min == 0))
    {
        printf("popodne");
    }
    else if (broken_time->tm_hour <= 23)
    {
        printf("vece");
    }
    putchar('\n');

    return 0;
}