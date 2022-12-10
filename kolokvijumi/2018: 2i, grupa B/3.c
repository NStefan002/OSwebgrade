#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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


void osChmod(const char *fpath, const char *m);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./3 file_path mode");

    osChmod(argv[1], argv[2]);

    return 0;
}

void osChmod(const char *fpath, const char *m)
{
    char ugo, plusminus, rwx;
    sscanf(m, "%c%c%c", &ugo, &plusminus, &rwx);
    
    /* check_error(ugo == 'u' || ugo == 'g' || ugo == 'o', "...");
    check_error(plusminus == '-' || plusminus == '+', "...");
    check_error(rwx == 'r' || rwx == 'w' || rwx == 'x', "..."); */

    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    mode_t mode = 0;

    switch (rwx)
    {
    case 'r':
        mode = 1 << 2;
        break;
    case 'w':
        mode = 1 << 1;
        break;
    case 'x':
        mode = 1;
        break;
    default:
        check_error(false, "...");
        break;
    }

    switch (ugo)
    {
    case 'u':
        mode <<= 6;
        break;
    case 'g':
        mode <<= 3;
        break;
    case 'o':
        // nista se ne menja
        break;
    default:
        check_error(false, "...");
        break;
    }

    if (plusminus == '+')
    {
        mode |= file_info.st_mode;
    }
    else
    {
        mode = file_info.st_mode & ~mode;
    }

    check_error(chmod(fpath, mode) != -1, "chmod");
}