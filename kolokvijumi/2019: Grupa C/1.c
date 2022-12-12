#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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


time_t absoluteDiff(const char *fpath1, const char *fpath2);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./1 file1 file2");

    printf("%ld\n", absoluteDiff(argv[1], argv[2]));

    return 0;
}

time_t absoluteDiff(const char *fpath1, const char *fpath2)
{
    struct stat file_info1, file_info2;
    check_error(stat(fpath1, &file_info1) != -1, "stat");
    check_error(stat(fpath2, &file_info2) != -1, "stat");

    time_t diff = labs(file_info1.st_mtime - file_info2.st_mtime);
    diff = (diff / DAY_TO_SEC) + ((diff % DAY_TO_SEC)? 1 : 0);

    return diff;
}