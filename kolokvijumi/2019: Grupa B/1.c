#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
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


int main(int argc, char **argv)
{
    check_error(argc == 3, "./1 file1 file2");    

    struct stat file_info1 , file_info2;

    check_error(stat(argv[1], &file_info1) != -1, "stat");
    check_error(stat(argv[2], &file_info2) != -1, "stat");

    if (file_info1.st_mtime > file_info2.st_mtime)
    {
        char *name = strrchr(argv[1], '/');
        if (name == NULL)
        {
            printf("%s\n", argv[1]);
        }
        else
        {
            printf("%s\n", name + 1);
        }
    }
    else
    {
        char *name = strrchr(argv[2], '/');
        if (name == NULL)
        {
            printf("%s\n", argv[2]);
        }
        else
        {
            printf("%s\n", name + 1);
        }
    }

    return 0;
}