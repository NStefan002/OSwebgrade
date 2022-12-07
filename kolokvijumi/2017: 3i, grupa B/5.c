#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
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


void countRegFilesWithExt(const char *path, const char *ext, int *count);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./5 dir_path ext");

    int result = 0;
    countRegFilesWithExt(argv[1], argv[2], &result);
    printf("%d\n", result);

    return 0;
}

void countRegFilesWithExt(const char *path, const char *ext, int *count)
{
    struct stat file_info;
    check_error(stat(path, &file_info) != -1, "stat");

    if (!S_ISDIR(file_info.st_mode))
    {
        // ukoliko je u pitanju regularni fajl, ispitujemo koja mu je ekstenzija
        if (S_ISREG(file_info.st_mode))
        {
            char *dot = strrchr(path, '.');

            if (dot != NULL && strcmp(dot, ext) == 0)
            {
                (*count)++;
            }
        }
        
        // ukoliko trenutni fajl nije direktorijum, izlazimo iz rekurzije
        return;
    }
    
    DIR *dirp = opendir(path);
    check_error(dirp != NULL, "opendir");

    check_error(chdir(path) != -1, "chdir");

    errno = 0;
    struct dirent *dir_entry = NULL;
    while ((dir_entry = readdir(dirp)) != NULL)
    {
        // izbegavamo ispitivanje . i .. da ne bismo upali u beskonacnu rekurziju
        if (!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, ".."))
        {
            continue;
        }
        
        countRegFilesWithExt(dir_entry->d_name, ext, count);
    }

    check_error(chdir("..") != -1, "chdir");
    check_error(errno != EBADF, "readdir");
}