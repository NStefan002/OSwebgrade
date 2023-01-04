#define _POSIX_C_SOURCE 200809L

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
    check_error(2 == argc, "./4 sec");

    time_t sec = atol(argv[1]);
    struct tm *broken_time = localtime(&sec);
    check_error(NULL != broken_time, "localtime");

    switch (broken_time->tm_mon)
    {
    case 0:
        printf("januar\n");
        break;
    case 1:
        printf("februar\n");
        break;
    case 2:
        printf("mart\n");
        break;
    case 3:
        printf("april\n");
        break;
    case 4:
        printf("maj\n");
        break;
    case 5:
        printf("jun\n");
        break;
    case 6:
        printf("jul\n");
        break;
    case 7:
        printf("avgust\n");
        break;
    case 8:
        printf("septembar\n");
        break;
    case 9:
        printf("oktobar\n");
        break;
    case 10:
        printf("novembar\n");
        break;
    case 11:
        printf("decembar\n");
        break;
    }

    return 0;
}