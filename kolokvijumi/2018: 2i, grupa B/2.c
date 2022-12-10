#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h> 
#include <limits.h>

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


void printLenOfAbsPaths(const char *dpath);

int main(int argc, char **argv)
{
    check_error(argc >= 2, "./2 dir_path1 dir_path2 ...");

    for (int i = 1; i < argc; i++)
    {
        printLenOfAbsPaths(argv[i]);
    }
    
    return 0;
}

void printLenOfAbsPaths(const char *dpath)
{
    struct stat file_info;
    check_error(stat(dpath, &file_info) != -1, "stat");

    check_error(S_ISDIR(file_info.st_mode), "not a dir");

    // ako ostali imaju neko pravo pristupa izlazimo iz funkcije
    if (S_IRWXO & file_info.st_mode)
    {
        return;
    }
    
    char *abs_path = realpath(dpath, NULL);

    // ne moramo da proveravamo da li je realpath vratio NULL,
    // jer smo u stat-u proverili da li fajl postoji
    printf("%lu\n", strlen(abs_path));
    
    free(abs_path);
}