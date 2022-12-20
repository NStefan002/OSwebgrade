#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>

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
    check_error(2 == argc, "./2 .ext");    

    int pipefds[2];
    check_error(-1 != pipe(pipefds), "pipe");

    pid_t child_pid = fork();
    check_error(-1 != child_pid, "fork");

    if (0 == child_pid) // child
    {
        check_error(-1 != close(pipefds[RD_END]), "close");
        check_error(-1 != dup2(pipefds[WR_END], STDOUT_FILENO), "dup2");
        check_error(-1 != execlp("find", "find", ".", "-type", "f", NULL), "execlp");
    }
    
    // parent
    check_error(-1 != close(pipefds[WR_END]), "close");

    FILE *child_to_parent = fdopen(pipefds[RD_END], "r");
    check_error(NULL != child_to_parent, "fdopen");

    unsigned num_of_ext_files = 0;
    
    char path[PATH_MAX];
    size_t ext_len = strlen(argv[1]);
    while (EOF != fscanf(child_to_parent, "%s", path))
    {
        // uporedjujemo zadnja ext_len karaktera imena fajla sa datom ekstenzijom
        if (!strncmp(path + strlen(path) - ext_len, argv[1], ext_len))
        {
            num_of_ext_files++;
        }
    }

    printf("%u\n", num_of_ext_files);

    check_error(-1 != fclose(child_to_parent), "fclose");
    // fclose ce zatvroti i pipefds[RD_END] pa nema potrebe pozivati close

    return 0;
}