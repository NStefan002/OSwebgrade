#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

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
#define RD_END (0)
#define WR_END (1)

int num_of_digits(int x);

int main(int argc, char **argv)
{
    check_error(1 == argc, "no arguments needed");
    UNUSED(argv);

    int x, y;
    char op[2]; // man expr
    while (EOF != scanf("%d%d%s", &x, &y, op))
    {
        /* Odredjujemo duzine niski koje ce da cuvaju vrednosti x i y u 
        karakterskom obliku. Alociramo niske i u njih upisujemo x i y 
        uz pomoc funkcije sprintf (man 3 sprintf). */
        int len_of_x = num_of_digits(x);
        int len_of_y = num_of_digits(y);
        char *to_string_x = (char *)malloc(len_of_x * sizeof(char));
        check_error(NULL != to_string_x, "malloc");
        sprintf(to_string_x, "%d", x);
        char *to_string_y = (char *)malloc(len_of_y * sizeof(char));
        check_error(NULL != to_string_y, "malloc");
        sprintf(to_string_y, "%d", y);
        
        int pipe_fds[2];
        check_error(-1 != pipe(pipe_fds), "pipe");

        pid_t child_pid = fork();
        check_error(-1 != child_pid, "fork");

        if (0 == child_pid) // child
        {
            check_error(-1 != close(pipe_fds[RD_END]), "close");
            
            check_error(-1 != dup2(pipe_fds[WR_END], STDOUT_FILENO), "dup2");
            check_error(-1 != execlp("expr", "expr", to_string_x, op, to_string_y, NULL), "execlp");
        }
        else // parent
        {
            check_error(-1 != close(pipe_fds[WR_END]), "close");
            FILE *f = fdopen(pipe_fds[RD_END], "r");
            check_error(NULL != f, "fdopen");

            char *res = (char *)malloc((len_of_x + len_of_y + 2) * sizeof(char));
            check_error(NULL != res, "malloc");
            
            fscanf(f, "%s", res);
            printf("%s\n", res);

            free(res);
            check_error(-1 != fclose(f), "fclose");
            
            // NULL jer nas ne zanima koji je exit code deteta
            check_error((pid_t)-1 != wait(NULL), "wait");
        }

        free(to_string_x);
        free(to_string_y);
    }

    return 0;
}


int num_of_digits(int x)
{
    int n = 0;
    // uracunavamo i minus
    if (x < 0)
    {
        n++;
    }
    
    while (x)
    {
        x /= 10;
        n++;
    }

    return n;
}