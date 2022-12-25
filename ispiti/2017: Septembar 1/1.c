#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#define MAX_LEN (64)
#define UNUSED(x) ((void)x)

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


// moguce vrednosti globalne promenljive flag
typedef enum
{
    REVERSE,
    LOWER_TO_UPPER,
    EXIT
} Flags;

// signal handler u flag upisuje informaciju o tome sta program dalje treba da radi
Flags flag;

void signal_handler(int signum);
void reverse(char *str);
void lower_to_upper(char *str);

int main(int argc, char **argv)
{
    UNUSED(argv);
    check_error(1 == argc, "no arguments needed");

    char str[MAX_LEN];
    while (true)
    {
        scanf("%s", str);
        check_error(SIG_ERR != signal(SIGUSR1, &signal_handler), "signal");
        check_error(SIG_ERR != signal(SIGUSR2, &signal_handler), "signal");
        check_error(SIG_ERR != signal(SIGTERM, &signal_handler), "signal");
        pause();

        switch (flag)
        {
        case REVERSE:
            reverse(str);
            break;
        
        case LOWER_TO_UPPER:
            lower_to_upper(str);
            break;

        case EXIT:
            exit(EXIT_SUCCESS);
            break;
        }
        printf("%s\n", str);
    }

    return 0;
}

void signal_handler(int signum)
{
    switch (signum)
    {
    case SIGUSR1:
        flag = REVERSE;
        break;
    
    case SIGUSR2:
        flag = LOWER_TO_UPPER;
        break;

    case SIGTERM:
        flag = EXIT;
        break;
    }
}

void reverse(char *str)
{
    for (int i = 0, j = strlen(str) - 1; i < j; i++, j--)
    {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
}

void lower_to_upper(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (islower(str[i]))
        {
            str[i] = toupper(str[i]);
        }
    }
}