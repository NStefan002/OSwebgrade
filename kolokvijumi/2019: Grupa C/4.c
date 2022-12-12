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


void create3rdFile(const char *file1, const char *file2, const char *file3);

int main(int argc, char **argv)
{
    check_error(argc == 4, "./4 file1 file2 file3");

    create3rdFile(argv[1], argv[2], argv[3]);

    return 0;
}

void create3rdFile(const char *file1, const char *file2, const char *file3)
{
    struct stat file_info1, file_info2;
    check_error(stat(file1, &file_info1) != -1, "stat");
    check_error(stat(file2, &file_info2) != -1, "stat");
    
    mode_t mode = 0;
    mode |= (S_IRWXU & file_info1.st_mode) & (S_IRWXU & file_info2.st_mode);
    mode |= (S_IRWXG & file_info1.st_mode) & (S_IRWXG & file_info2.st_mode);
    mode |= (S_IRWXO & file_info1.st_mode) & (S_IRWXO & file_info2.st_mode);
    
    mode_t old_umask = umask(0);
    check_error(creat(file3, mode) != -1, "creat");
    umask(old_umask);
}