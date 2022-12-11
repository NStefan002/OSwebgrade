#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
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


void osReadLink(const char *lpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./6 link_path");    

    osReadLink(argv[1]);

    return 0;
}

void osReadLink(const char *lpath)
{
    char fpath[PATH_MAX];
    ssize_t path_len;
    check_error((path_len = readlink(lpath, fpath, PATH_MAX)) != -1, "readlink");
    fpath[path_len] = '\0';

    char *name = strrchr(fpath, '/');
    if (name == NULL)
    {
        printf("%s\n", fpath);
    }
    else
    {
        printf("%s\n", name + 1);
    }
}