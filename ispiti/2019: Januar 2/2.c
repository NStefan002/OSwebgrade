#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

#define RD_END (0)
#define WR_END (1)

int main(int argc, char **argv)
{
    check_error(2 <= argc, "./2 file1 file2");

    int pipe_fds[2];
    check_error(-1 != pipe(pipe_fds), "pipe");
    FILE *rd_end_for_parrent = fdopen(pipe_fds[RD_END], "r");

    pid_t child;

    for (int i = 1; i < argc; i++)
    {
        child = fork();
        check_error(-1 != child, "fork");

        if (0 == child) // child
        {
            check_error(-1 != fclose(rd_end_for_parrent), "fclose");

            check_error(-1 != dup2(pipe_fds[WR_END], STDOUT_FILENO), "dup2");

            check_error(-1 != execlp("du", "du", "-sch", argv[i], NULL), "execlp");
        }
        else // parent
        {
            int status = 0;
            check_error(-1 != wait(&status), "wait");
            if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
            {
                printf("neuspeh");
            }
            else 
            {
                /* Pogledati sta ispisuje komanda du. Moramo da procitamo obe linije da bismo
                pomerili kursor na mesto na kom pocinje ispis sledeceg poziva du komande. Ako 
                to ne uradimo moze se desiti sledece: prvi poziv ispise 2 linije, mi procitamo 
                samo jednu, drugi poziv ispise svoje 2 linije, umesto da citamo prvu liniju drugog
                poziva mi cemo procitati drugu liniju prvog poziva, sto nam daje netacan rezultat. */
                char *line = NULL;
                size_t n = 0;
                getline(&line, &n, rd_end_for_parrent);
                getline(&line, &n, rd_end_for_parrent);
                int i;
                for (i = 0; !isblank(line[i]); i++)
                {
                    if (line[i] == '.') // ?? iz nekog razloga se trazi format ispisa sa zarezom ??
                    {
                        line[i] = ',';
                    }
                }
                line[i] = '\0';
                printf("%s", line);
                free(line);
            }
            putchar(' ');
        } 
    }
    
    putchar('\n');
    return 0;
}