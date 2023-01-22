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

#define SEC_IN_DAY (24 * 60 * 60)

char *read_month(struct tm *broken_time);

int main(int argc, char **argv)
{
    check_error(3 == argc, "./1 seconds days");

    time_t first_time = atol(argv[1]);
    time_t second_time = first_time + SEC_IN_DAY * atol(argv[2]);

    struct tm *broken_time = localtime(&first_time);
    printf("%s ", read_month(broken_time));

    broken_time = localtime(&second_time);
    printf("%s\n", read_month(broken_time));

    return 0;
}

char *read_month(struct tm *broken_time)
{
    switch (broken_time->tm_mon)
    {
        case 0: return "januar";
        case 1: return "februar";
        case 2: return "mart";
        case 3: return "april";
        case 4: return "maj";
        case 5: return "jun";
        case 6: return "jul";
        case 7: return "avgust";
        case 8: return "septembar";
        case 9: return "oktobar";
        case 10: return "novembar";
        case 11: return "decembar";
    }
}