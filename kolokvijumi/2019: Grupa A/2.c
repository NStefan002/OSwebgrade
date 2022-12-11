#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

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


time_t sec(const char *link_path);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./2 link");

    printf("%ld\n", sec(argv[1]));

    return 0;
}

time_t sec(const char *link_path)
{
    struct stat file_info;
    check_error(lstat(link_path, &file_info) != -1, "lstat");

    check_error(S_ISLNK(file_info.st_mode), "not a link");
    
    return file_info.st_mtime;
}