#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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


bool osIsPublicFile(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./1 filepath");

    if (osIsPublicFile(argv[1]))
    {
        printf("true\n");
    }
    else
    {
        printf("false\n");
    }

    return 0;
}

bool osIsPublicFile(const char *fpath)
{
    struct stat statbuf;
    check_error(lstat(fpath, &statbuf) != -1, "lstat");

    check_error(S_ISREG(statbuf.st_mode), "not a regular file");

    return ((statbuf.st_mode & S_IROTH) && (statbuf.st_mode & S_IWOTH)); 
}