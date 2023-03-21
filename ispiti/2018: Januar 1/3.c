#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

#define RD_END (0)
#define WR_END (1)

int main(int argc, char **argv)
{
    check_error(3 == argc, "./3 filepath -[cwl]");

    int pipefds[2];
    check_error(-1 != pipe(pipefds), "pipe");

    pid_t child_pid = fork();
    check_error((pid_t)-1 != child_pid, "fork");

    if (0 == child_pid) // child
    {
        check_error(-1 != close(pipefds[RD_END]), "close");

        check_error(-1 != dup2(pipefds[WR_END], STDOUT_FILENO), "dup2");

        check_error(-1 != execlp("wc", "wc", argv[2], argv[1], NULL), "execlp");
    }
    else // parent
    {
        check_error(-1 != close(pipefds[WR_END]), "close");
        FILE *f_rd_end = fdopen(pipefds[RD_END], "r");
        int n;
        bool file_not_found = false;
        if (1 != fscanf(f_rd_end, "%d", &n))
        {
            file_not_found = true;
        }
        else
        {
            printf("%d\n", n);
        }

        int status = 0;
        check_error((pid_t)-1 != wait(&status), "wait");

        if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS || file_not_found)
        {
            printf("Neuspeh\n");
        }
        check_error(-1 != fclose(f_rd_end), "fclose");
    }

    return 0;
}
