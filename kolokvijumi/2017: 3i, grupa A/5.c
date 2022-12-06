#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <sys/types.h>
#include <sys/stat.h>
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


/* globalne promenjive pomocu kojih brojimo koliko puta se koji
tip fajla pojavio */
int regs = 0;
int dirs = 0;
int chars = 0;
int blocks = 0;
int links = 0;
int FIFOs = 0;
int sockets = 0;
int overall = 0;


int count(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./5 dirname");

    check_error(nftw(argv[1], &count, 50, FTW_PHYS) != -1, "nftw");

    printf("%d %d %d %d %d %d %d %d\n", regs, dirs, chars, blocks, links, FIFOs, sockets, overall);

    return 0;
}

int count(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (S_ISREG(sb->st_mode))
        regs++;
    if (S_ISDIR(sb->st_mode))
        dirs++;
    if (S_ISCHR(sb->st_mode))
        chars++;
    if (S_ISBLK(sb->st_mode))
        blocks++;
    if (S_ISLNK(sb->st_mode))
        links++;
    if (S_ISFIFO(sb->st_mode))
        FIFOs++;
    if (S_ISSOCK(sb->st_mode))
        sockets++;

    overall++;
    
    return 0;
}