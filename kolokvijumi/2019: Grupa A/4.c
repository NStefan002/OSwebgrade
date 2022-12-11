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


void swapPermissions(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./4 file_path");

    swapPermissions(argv[1]);

    return 0;
}

void swapPermissions(const char *fpath)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    mode_t new_mode = 0;
    new_mode |= (file_info.st_mode & S_IRWXU) >> 6; 
    new_mode |= (file_info.st_mode & S_IRWXG) << 3; 
    new_mode |= (file_info.st_mode & S_IRWXO) << 3; 

    check_error(chmod(fpath, new_mode) != -1, "chmod");
}