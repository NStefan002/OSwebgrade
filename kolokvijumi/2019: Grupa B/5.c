#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

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


void recursiveSearch(const char *path, bool is_under_dir_);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./5 dir_path");
    struct stat file_info;
    check_error(stat(argv[1], &file_info) != -1, "stat");
    check_error(S_ISDIR(file_info.st_mode), "not a dir");

    bool is_under_dir_;
    char *name = strrchr(argv[1], '/');
    if (name == NULL)
    {
        is_under_dir_ = !strncmp(argv[1], "dir_", 4);
    }
    else
    {
        is_under_dir_ = !strncmp(name + 1, "dir_", 4);
    }
    
    recursiveSearch(argv[1], is_under_dir_);

    return 0;
}

void recursiveSearch(const char *path, bool is_under_dir_)
{
    struct stat file_info;
    check_error(lstat(path, &file_info) != -1, "stat");

    // ukoliko smo naisli na fajl koji nije direktorijum, izlazimo iz rekurzije
    if (!S_ISDIR(file_info.st_mode))
    {
        if (is_under_dir_ && S_ISREG(file_info.st_mode))
        {
            printf("%s\n", path);
        }
    
        return;
    }

    DIR *dirp = opendir(path);
    check_error(dirp != NULL, "opendir");

    is_under_dir_ = (is_under_dir_)? true : !strncmp(path, "dir_", 4);
    check_error(chdir(path) != -1, "chdir");

    struct dirent *dir_entry = NULL;

    while ((dir_entry = readdir(dirp)) != NULL)
    {
        if (!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, ".."))
        {
            continue;
        }
        recursiveSearch(dir_entry->d_name, is_under_dir_);
    }

    check_error(chdir("..") != -1, "chdir");
    is_under_dir_ = false;
    check_error(closedir(dirp) != -1, "closedir");
}