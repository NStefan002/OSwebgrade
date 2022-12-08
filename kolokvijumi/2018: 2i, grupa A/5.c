#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
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


void findMinAndMax(const char *path, ssize_t *min_size, ssize_t *max_size);
bool isDir(const char *path);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./5 dir_path");

    check_error(isDir(argv[1]), "not a dir");

    ssize_t min = INT_MAX, max = 0;
    findMinAndMax(argv[1], &min, &max);
    printf("%ld\n", max - min);

    return 0;
}

bool isDir(const char *path)
{
    struct stat file_info;
    check_error(stat(path, &file_info) != -1, "stat");

    return S_ISDIR(file_info.st_mode);
}

void findMinAndMax(const char *path, ssize_t *min_size, ssize_t *max_size)
{
    struct stat file_info;
    check_error(stat(path, &file_info) != -1, "stat");

    // ukoliko nismo naisli na direktorijum, izlazimo iz rekurzije
    if (!S_ISDIR(file_info.st_mode))
    {
        // ukoliko smo naisli na reg file, azuriramo min_size i max_size
        if (S_ISREG(file_info.st_mode))
        {
            if (*min_size > file_info.st_size)
            {
                *min_size = file_info.st_size;
            }
            if (*max_size < file_info.st_size)
            {
                *max_size = file_info.st_size;
            }
        }

        return;
    }
    
    DIR *dirp
     = opendir(path);
    check_error(dirp != NULL, "opendir");

    check_error(chdir(path) != -1, "chdir1");

    errno = 0;
    struct dirent *dir_entry = NULL;
    while ((dir_entry = readdir(dirp)) != NULL)
    {
        // preskacemo . i .. da bismo izbegli beskonacnu rekurziju
        if (!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, ".."))
        {
            continue;
        }

        errno = 0;
        findMinAndMax(dir_entry->d_name, min_size, max_size);
    }

    check_error(errno != EBADF, "readdir");
    check_error(chdir("..") != -1, "chdir2");
    check_error(closedir(dirp) != -1, "closedir");
}