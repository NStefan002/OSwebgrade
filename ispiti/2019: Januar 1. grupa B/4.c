#define _POSIX_C_SOURCE 200809L

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

#define MAX_LINE (256)
#define MAX_COMMAND_OUTPUT (4094)

int main(int argc, char **argv)
{
    check_error(2 == argc, "./4 commands_file");

    FILE *commands_file = fopen(argv[1], "r");
    check_error(NULL != commands_file, "fopen");

    int pipe_fds[2];
    check_error(-1 != pipe(pipe_fds), "pipe");

    FILE *rd_end_for_parent = fdopen(pipe_fds[RD_END], "r");
    check_error(NULL != rd_end_for_parent, "fdopen");

    char line[MAX_LINE];
    size_t n = MAX_LINE;

    while (-1 != getline(&line, &n, commands_file))
    {
        pid_t child_pid = fork();
        if (0 == child_pid) // child
        {
            check_error(-1 != fclose(rd_end_for_parent), "fclose");

        }

    }

    check_error(-1 != fclose(commands_file), "fclose");

    return 0;
}
