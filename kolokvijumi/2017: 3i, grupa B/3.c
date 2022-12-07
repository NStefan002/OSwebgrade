#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
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


void osCreateReg(const char *fpath, mode_t mode);
void osCreateDir(const char *dpath, mode_t mode);

void osCreate(const char *path, const char *option, const char *mode);

int main(int argc, char **argv)
{
    check_error(argc == 4, "./3 -[f/d] path mode");

    osCreate(argv[2], argv[1], argv[3]);

    return 0;
}

void osCreate(const char *path, const char *option, const char *mode)
{
    check_error(option[0] == '-', "not an option");
    check_error((option[1] == 'f' || option[1] == 'd') && option[2] == '\0', "invalid option");
    
    mode_t m = strtol(mode, NULL, 8);

    mode_t old_umask = umask(0);
    if (option[1] == 'f')
    {
        osCreateReg(path, m);
    }
    else
    {
        osCreateDir(path, m);
    }
    umask(old_umask);
}

void osCreateReg(const char *fpath, mode_t mode)
{
    int fd;
    // O_EXCL u kombinaciji sa O_CREAT postavlja errno na EEXIST i tera open da pukne ukoliko fajl 
    // na putanji fpath vec postoji 
    check_error((fd = open(fpath, O_CREAT | O_EXCL, mode)) != -1, "open");
    close(fd);
}

void osCreateDir(const char *dpath, mode_t mode)
{
    check_error(mkdir(dpath, mode) != -1, "mkdir");
}