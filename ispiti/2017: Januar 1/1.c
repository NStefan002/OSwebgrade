#include <stdio.h>
#include <stdlib.h>
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


int sigusr1 = 0;
int sigusr2 = 0;

void signal_handler_sigusr(int signum);
void signal_handler_sigterm(int signum);

int main(int argc, char **argv)
{
    do
    {
        check_error(SIG_ERR != signal(SIGUSR1, &signal_handler_sigusr), "signal");
        check_error(SIG_ERR != signal(SIGUSR2, &signal_handler_sigusr), "signal");
        check_error(SIG_ERR != signal(SIGTERM, &signal_handler_sigterm), "signal");
    } while (1);
    
    return 0;
}

void signal_handler_sigusr(int signum)
{
    if (signum == SIGUSR1)
    {
        sigusr1++;
    }
    else // if (signum == SIGUSR2)
    {
        sigusr2++;
    }
}

void signal_handler_sigterm(int signum)
{
    printf("%d %d\n", sigusr1, sigusr2);
    exit(EXIT_SUCCESS);
}