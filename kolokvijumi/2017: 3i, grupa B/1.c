#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define DAY_TO_SEC (24 * 60 * 60)

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


void printDiffAtimeMtime(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./1 file_path");

    printDiffAtimeMtime(argv[1]);

    return 0;
}

void printDiffAtimeMtime(const char *fpath)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    check_error(S_ISREG(file_info.st_mode), "not a regular file");

    // labs je abs samo za long
    int diffInDays = labs(file_info.st_atime - file_info.st_mtime) / DAY_TO_SEC;

    printf("%d\n", diffInDays);
}