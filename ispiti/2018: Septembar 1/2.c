#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
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
    check_error(2 <= argc, "./2 command [args]");

    int pipe_fds[2];
    check_error(-1 != pipe(pipe_fds), "pipe");

    pid_t child_pid = fork();
    check_error((pid_t)-1 != child_pid, "fork");

    if (0 == child_pid)
    {
        check_error(-1 != close(pipe_fds[RD_END]), "close");
        check_error(-1 != dup2(pipe_fds[WR_END], STDOUT_FILENO), "dup2");
        check_error(-1 != execvp(argv[1], argv + 1), "execvp");
    }

    check_error(-1 != close(pipe_fds[WR_END]), "close");
    FILE *f = fdopen(pipe_fds[RD_END], "r");
    check_error(NULL != f, "fdopen");

    int num_of_lines = 0;
    char *line = NULL;
    size_t n = 0;

    while (EOF != getline(&line, &n, f))
    {
        num_of_lines++;
    }

    int status = 0;
    check_error(-1 != wait(&status), "wait");

    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
    {
        printf("Neuspeh\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("%d\n", num_of_lines);
    }

    return 0;
}
