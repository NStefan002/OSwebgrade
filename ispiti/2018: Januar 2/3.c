#define _POSIX_C_SOURCE 200809L

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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
    check_error(3 == argc, "./3 filepath a");

    int pipefds[2];
    check_error(-1 != pipe(pipefds), "pipe");

    pid_t child_pid = fork();
    check_error((pid_t)-1 != child_pid, "fork");

    if (0 == child_pid) // child
    {
        check_error(-1 != close(pipefds[RD_END]), "close");
        check_error(-1 != dup2(pipefds[WR_END], STDOUT_FILENO), "dup2");
        close(pipefds[WR_END]);
        check_error(-1 != execlp("tail", "tail", "-n", argv[2], argv[1], NULL), "execlp");
        exit(EXIT_SUCCESS);
    }

    // parent
    check_error(-1 != close(pipefds[WR_END]), "close");

    FILE *child_to_parent = fdopen(pipefds[RD_END], "r");
    check_error(NULL != child_to_parent, "fdopen");

    char *line = NULL;
    size_t n = 0;
    while (-1 != getline(&line, &n, child_to_parent))
    {
        printf("%s", line);
    }

    free(line);
    check_error(-1 != fclose(child_to_parent), "fclose");

    int status = 0;
    check_error((pid_t)-1 != wait(&status), "wait");

    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
    {
        printf("Neuspeh\n");
    }

    return 0;
}
