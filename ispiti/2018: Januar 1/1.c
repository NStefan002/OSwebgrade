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

    time_t sec = atoi(argv[1]);

    struct tm *broken_time;
    check_error(NULL != (broken_time = localtime(&sec)), "localtime");

    switch (broken_time->tm_mon)
    {
    case 0:
        printf("januar");
        break;
    case 1:
        printf("februar");
        break;
    case 2:
        printf("mart");
        break;
    case 3:
        printf("april");
        break;
    case 4:
        printf("maj");
        break;
    case 5:
        printf("jun");
        break;
    case 6:
        printf("jul");
        break;
    case 7:
        printf("avgust");
        break;
    case 8:
        printf("septembar");
        break;
    case 9:
        printf("oktobar");
        break;
    case 10:
        printf("novembar");
        break;
    case 11: 
        printf("decembar");
        break;
    }
    putchar('\n');

    return 0;
}