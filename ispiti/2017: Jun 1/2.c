#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define RD_END (0)
#define WR_END (1)
#define MODE_LEN (10)

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


int main(int argc, char **argv)
{
    check_error(2 == argc, "./2 file_path");

    // ne moramo da proveravamo da li je fajl regularan (dato uslovom zadatka)

    int pipe_fds[2];
    check_error(-1 != pipe(pipe_fds), "pipe");

    pid_t child_pid = fork();
    check_error((pid_t)-1 != child_pid, "fork");

    if (0 == child_pid) // child
    {
        check_error(-1 != close(pipe_fds[RD_END]), "close");
        check_error(-1 != dup2(pipe_fds[WR_END], STDOUT_FILENO), "dup2");
        check_error(-1 != execlp("ls", "ls", "-l", argv[1], NULL), "execlp");
    }

    // parent

    check_error(-1 != close(pipe_fds[WR_END]), "close");
    FILE *from_child = fdopen(pipe_fds[RD_END], "r");
    check_error(NULL != from_child, "fdopen");

    char mode[MODE_LEN];
    fscanf(from_child, "%s", mode);
    printf("%s\n", mode);

    check_error(-1 != fclose(from_child), "fclose");
    // fdclose ce zatvoriti i read end na pipe-u
    // check_error(-1 != close(pipe_fds[RD_END]), "close");

    int status = 0;
    check_error((pid_t)-1 != wait(&status), "wait");

    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
    {
        printf("Neuspeh\n");
    }

    return 0;
}
