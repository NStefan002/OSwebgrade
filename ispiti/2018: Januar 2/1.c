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

    time_t sec = atol(argv[1]);
    struct tm *broken_time = localtime(&sec);
    check_error(NULL != broken_time, "localtime");

    printf("%02d:%02d\n", broken_time->tm_hour, broken_time->tm_min);

    return 0;
}
