#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

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


void osPrintFileTypeAndMode(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./1 file_path");

    osPrintFileTypeAndMode(argv[1]);

    return 0;
}

void osPrintFileTypeAndMode(const char *fpath)
{
    // treba nam 10 karaktera da prikazemo tip i prava pristupa fajla (npr -rw-r--r--)
    char *permissions = malloc(10 * sizeof(char));
    check_error(permissions != NULL, "malloc");

    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    if (S_ISDIR(file_info.st_mode))
        permissions[0] = 'd';
    else if (S_ISREG(file_info.st_mode))
        permissions[0] = '-';
    else // ispitujemo samo direktorijume i regularne fajlove
        return;

    permissions[1] = (file_info.st_mode & S_IRUSR)? 'r' : '-';
    permissions[2] = (file_info.st_mode & S_IWUSR)? 'w' : '-';
    permissions[3] = (file_info.st_mode & S_IXUSR)? 'x' : '-';
    permissions[4] = (file_info.st_mode & S_IRGRP)? 'r' : '-';
    permissions[5] = (file_info.st_mode & S_IWGRP)? 'w' : '-';
    permissions[6] = (file_info.st_mode & S_IXGRP)? 'x' : '-';
    permissions[7] = (file_info.st_mode & S_IROTH)? 'r' : '-';
    permissions[8] = (file_info.st_mode & S_IWOTH)? 'w' : '-';
    permissions[9] = (file_info.st_mode & S_IXOTH)? 'x' : '-';

    printf("%s\n", permissions);
    free(permissions);
}