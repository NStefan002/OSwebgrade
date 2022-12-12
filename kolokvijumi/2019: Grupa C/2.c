#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
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


ssize_t sizeDiff(const char *lpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./2 symlink");    

    printf("%ld\n", sizeDiff(argv[1]));

    return 0;
}

ssize_t sizeDiff(const char *lpath)
{
    struct stat file_info, link_info;
    check_error(lstat(lpath, &link_info) != -1, "lstat");
    check_error(S_ISLNK(link_info.st_mode), "not a link");

    check_error(stat(lpath, &file_info) != -1, "stat");

    return file_info.st_size - link_info.st_size;
}