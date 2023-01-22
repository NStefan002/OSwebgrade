#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

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

#define UNUSED(x) ((void)x)

int res; // globalna promenljiva koju azuriamo iz signal handler-a

void signal_handler(int signum);

int main(int argc, char **argv)
{
    UNUSED(argv);
    check_error(1 == argc, "no arguments needed");

    res = 0;

    while (true)
    {
        check_error(SIG_ERR != signal(SIGUSR1, &signal_handler), "signal");
        check_error(SIG_ERR != signal(SIGUSR2, &signal_handler), "signal");
        check_error(SIG_ERR != signal(SIGINT, &signal_handler), "signal");
        check_error(SIG_ERR != signal(SIGTERM, &signal_handler), "signal");
    }

    return 0;
}

void signal_handler(int signum)
{
    switch (signum)
    {
    case SIGUSR1:
        res++;
        break;
    case SIGUSR2:
        res += 2;
        break;
    case SIGINT:
        res -= 4;
        break;
    default:
        printf("%d\n", res);
        exit(EXIT_SUCCESS);
    }
}