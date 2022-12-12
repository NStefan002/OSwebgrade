#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>

#define DAY_TO_SEC (24 * 60 * 60)

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


void recursiveSearch(const char *fpath, time_t curr_time);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./5 dir_path");    

    struct stat file_info;
    check_error(stat(argv[1], &file_info) != -1, "stat");
    check_error(S_ISDIR(file_info.st_mode), "not a dir");

    time_t curr_time = time(NULL);
    recursiveSearch(argv[1], curr_time);

    return 0;
}

void recursiveSearch(const char *fpath, time_t curr_time)
{
    struct stat file_info;
    // koristimo lstat jer ukoliko naidjemo na symlink koji pokazuje na reg file
    // stat ce nam vratiti podatke o tom reg file-u (a zelimo podatke o sym link-u)
    check_error(lstat(fpath, &file_info) != -1, "lstat");

    if (!S_ISDIR(file_info.st_mode))
    {
        if (S_ISREG(file_info.st_mode))
        {
            time_t diff = (curr_time - file_info.st_mtime)  / DAY_TO_SEC + 1;
            if (diff <= 5)
            {
                printf("%s\n", fpath);
            }
        }
        
        return;
    }
    
    DIR *dirp = opendir(fpath);
    check_error(dirp != NULL, "opendir");

    check_error(chdir(fpath) != -1, "chdir");

    errno = 0;
    struct dirent *dir_entry = NULL;
    while ((dir_entry = readdir(dirp)) != NULL)
    {
        if (strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, ".."))
        {
            errno = 0;
            recursiveSearch(dir_entry->d_name, curr_time);
        }
    }

    check_error(errno != EBADF, "readdir");
    check_error(chdir("..") != -1, "chdir");
    check_error(closedir(dirp) != -1, "closedir");
}