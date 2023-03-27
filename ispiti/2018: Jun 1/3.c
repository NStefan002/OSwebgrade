#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
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

int main(int argc, char **argv)
{
    check_error(1 == argc, "no arguments needed");
    UNUSED(argv);

    int pipe_fds[2];
    check_error(-1 != pipe(pipe_fds), "pipe");

    pid_t child_pid = fork();
    check_error((pid_t)-1 != child_pid, "fork");

    if (0 == child_pid) // child
    {
        check_error(-1 != close(pipe_fds[RD_END]), "close");
        check_error(-1 != dup2(pipe_fds[WR_END], STDOUT_FILENO), "dup2");
        check_error(-1 != execlp("ls", "ls", "-l", NULL), "execlp");
    }

    check_error(-1 != close(pipe_fds[WR_END]), "close");

    int status;
    check_error(-1 != wait(&status), "wait");

    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
    {
        printf("neuspeh\n");
        exit(WEXITSTATUS(status));
    }

    FILE *child_to_par = fdopen(pipe_fds[RD_END], "r");
    check_error(NULL != child_to_par, "fdopen");

    char *line = NULL;
    size_t n = 0;
    getline(&line, &n, child_to_par); // ingorisemo prvu liniju
    line = NULL;
    n = 0;

    while (EOF != getline(&line, &n, child_to_par))
    {
        if (line[7] == 'r' && line[8] == 'w' && line[9] == 'x')
        {
            char *file_name = strrchr(line, ' ');
            printf("%s", file_name + 1); // novi red ukljucen na kraju line
        }
    }

    free(line);
    check_error(-1 != fclose(child_to_par), "fclose");

    return 0;
}
