#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN (256)

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
    check_error(2 == argc, "./4 commands_file");

    FILE *f = fopen(argv[1], "r");
    check_error(NULL != f, "fopen");

    FILE *err = fopen("errors.txt", "w");
    check_error(NULL != err, "fopen");

    char command[MAX_LEN], argument[MAX_LEN];

    while (2 == fscanf(f, "%s%s", command, argument))
    {
        int pipefds[2];
        check_error(-1 != pipe(pipefds), "pipe");

        pid_t child_pid = fork();
        check_error((pid_t)-1 != child_pid, "fork");

        if (0 == child_pid) // child
        {
            check_error(-1 != close(pipefds[RD_END]), "close");
            check_error(-1 != dup2(pipefds[WR_END], STDERR_FILENO), "dup2");
            check_error(-1 != execlp(command, command, argument, NULL), "execlp");
        }
        else // parent
        {
            check_error(-1 != close(pipefds[WR_END]), "close");

            FILE *child_to_parent = fdopen(pipefds[RD_END], "r");
            check_error(NULL != child_to_parent, "fdopen");

            char *buff = NULL;
            size_t len = 0;

            int status;
            check_error(-1 != wait(&status), "wait");

            if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS)
            {
                while (EOF != getline(&buff, &len, child_to_parent))
                {
                    fprintf(err, "%s", buff);
                }
                free(buff);
            }
        }
    }

    check_error(-1 != fclose(f), "fclose");
    check_error(-1 != fclose(err), "fclose");

    return 0;
}
