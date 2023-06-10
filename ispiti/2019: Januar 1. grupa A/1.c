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

#define YEAR_TO_SEC (365 * 24 * 60 * 60)

int main(int argc, char **argv)
{
    check_error(3 == argc, "./1 s y");

    time_t t1 = atol(argv[1]);

    // man localtime (nedelja je predstavljena kao 0)
    char *days[] = { "nedelja", "ponedeljak", "utorak", "sreda", "cetvrtak", "petak", "subota" };
    struct tm *broken_time = localtime(&t1);
    check_error(NULL != broken_time, "localtime");

    printf("%s ", days[broken_time->tm_wday]);

    // u zavisnosti od prestupnih godina
    time_t y = atol(argv[2]);
    time_t added_days = (broken_time->tm_yday % 4 + y) / 4;
    time_t t2 = t1 + YEAR_TO_SEC * y + added_days * 24 * 60 * 60;
    broken_time = localtime(&t2);
    check_error(NULL != broken_time, "localtime");

    printf("%s ", days[broken_time->tm_wday]);

    return 0;
}
