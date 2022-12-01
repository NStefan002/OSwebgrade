#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define DAY_TO_SEC (60 * 60 * 24)

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
    

unsigned osNumOfDaysFileModified(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./3 filepath");

    printf("%u\n", osNumOfDaysFileModified(argv[1]));

    return 0;
}

unsigned osNumOfDaysFileModified(const char *fpath)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    time_t now = time(NULL);
    unsigned time_since_last_mod = now - file_info.st_mtime;

    return time_since_last_mod / DAY_TO_SEC;
}