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


void changePermissions(const char *fpath1, const char *fpath2);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./4 file1 file2");

    changePermissions(argv[1], argv[2]);

    return 0;
}

void changePermissions(const char *fpath1, const char *fpath2)
{
    struct stat file_info1;
    // prvi poziv lstat-a je samo da bismo proverili da li drugi fajl 
    // postoji, tako da nema potrebe cuvati informacije o tom fajlu ukoliko postoji
    check_error(lstat(fpath2, &file_info1) != -1, "lstat");
    check_error(lstat(fpath1, &file_info1) != -1, "lstat");

    mode_t new_mode = 0;
    // vlasnik drugog dobija prava kao grupa prvog
    new_mode |= (file_info1.st_mode & S_IRWXG) << 3;
    // grupa drugog dobija prava kao ostali prvog
    new_mode |= (file_info1.st_mode & S_IRWXO) << 3;
    // ostali drugog dobijaju prava kao vlasnik prvog
    new_mode |= (file_info1.st_mode & S_IRWXU) >> 6;

    check_error(chmod(fpath2, new_mode) != -1, "chmod");
}