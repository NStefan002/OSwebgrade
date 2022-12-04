#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

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


void osCreateOrChangeMode(const char *file_type, const char *fpath, const char *mode);
void createDirOrChangeMode(const char *dirpath, mode_t mode);
void createRegFileOrChangeMode(const char *filepath, mode_t mode);

int main(int argc, char **argv)
{
    check_error(argc == 4, "./3 -[f/d] file_path mode");

    osCreateOrChangeMode(argv[1], argv[2], argv[3]);

    return 0;
}

void createDirOrChangeMode(const char *dirpath, mode_t mode)
{
    errno = 0;
    // pokusavamo da promenimo mod zadatog direktorijuma
    if (chmod(dirpath, mode) != -1)
    {
        // dir postoji, menjamo mu mod i izlazimo
        return;
    }

    // ukoliko je doslo do greske pri pozivu chmod, razmatramo sledece slucajeve
    
    // ukoliko dir ne postoji, pravimo ga sa zadatim modom, inace je doslo do neke druge greske prilikom chmod
    if (errno == ENOENT)
    {
        mode_t old_umask = umask(0);
        check_error(mkdir(dirpath, mode) != -1, "mkdir");
        umask(old_umask);
    }
    else
    {
        check_error(false, "chmod1");
    }
} 

void createRegFileOrChangeMode(const char *filepath, mode_t mode)
{
    errno = 0;
    // pokusavamo da promenimo mod zadatog fajla
    if (chmod(filepath, mode) != -1)
    {
        // fajl postoji, menjamo mu mod i izlazimo
        return;
    }

    // ukoliko fajl ne postoji, pravimo ga sa zadatim modom, inace je doslo do neke druge greske prilikom chmod
    if (errno == ENOENT)
    {
        check_error(open(filepath, O_WRONLY | O_CREAT | O_TRUNC, mode) != -1, "open");
    }
    else
    {
        check_error(false, "chmod2");
    }
}

void osCreateOrChangeMode(const char *file_type, const char *fpath, const char *mode)
{
    check_error(file_type[0] == '-', "not an option");

    mode_t m = strtol(mode, NULL, 8);

    if (file_type[1] == 'f')
    {
        createRegFileOrChangeMode(fpath, m);
    }
    else if (file_type[1] == 'd')
    {
        createDirOrChangeMode(fpath, m);
    }
    else
    {
        check_error(false, "invalid option");
    }
}