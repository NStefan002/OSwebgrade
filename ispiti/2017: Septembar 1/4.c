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

    time_t input = strtol(argv[1], NULL, 10);
    
    struct tm *broken_time = localtime(&input);

    /* polje tm_wday strukture tm predstavlja broj dana nakon 
    nedelje (ako je 0 znaci u pitanju je nedelja)
     * man 3 localtime */
    switch (broken_time->tm_wday)
    {
    case 0:
        printf("nedelja\n");
        break;
        
    case 1:
        printf("ponedeljak\n");
        break;

    case 2:
        printf("utorak\n");
        break;

    case 3:
        printf("sreda\n");
        break;

    case 4:
        printf("cetvrtak\n");
        break;
        
    case 5:
        printf("petak\n");
        break;

    case 6:
        printf("subota\n");
        break;
    }

    return 0;
}