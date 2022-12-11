#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
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


char *ext = NULL;

int regsWithExt(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./5 dir_name ext");

    struct stat file_info;
    check_error(lstat(argv[1], &file_info) != -1, "lstat");
    check_error(S_ISDIR(file_info.st_mode), "not a dir");

    ext = argv[2];

    check_error(nftw(argv[1], &regsWithExt, 50, 0) != -1, "nftw");

    return 0;
}

int regsWithExt(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (S_ISREG(sb->st_mode) && ftwbuf->level >= 2 && ftwbuf->level <= 5)
    {
        char *file_ext = strrchr(fpath, '.');
        if (!strcmp(file_ext, ext))
        {
            // traze se nazivi fajlova ne putanje do njih
            printf("%s\n", fpath + ftwbuf->base);
        }
    }

    return 0;
}