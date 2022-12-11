#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>

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



int printFilesWithDotInName(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./5 dir_path");

    struct stat file_info;
    check_error(stat(argv[1], &file_info) != -1, "stat");

    check_error(S_ISDIR(file_info.st_mode), "not a dir");

    nftw(argv[1], &printFilesWithDotInName, 50, FTW_PHYS);

    return 0;
}

int printFilesWithDotInName(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_D && ftwbuf->level >= 1 && ftwbuf->level <= 3 && strchr(fpath + ftwbuf->base, '.') != NULL)
    {
        printf("%s\n", fpath + ftwbuf->base);
    }
    
    return 0;
}