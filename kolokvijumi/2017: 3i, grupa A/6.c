#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


void osMv(const char *src, const char *dest);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./6 src dest");

    osMv(argv[1], argv[2]);

    return 0;
}

void osMv(const char *src, const char *dest)
{
    char *real_src_path = realpath(src, NULL);
    char *real_dest_path = realpath(dest, NULL);

    check_error(real_src_path != NULL, "src does not exist");
    if (real_dest_path != NULL)
    {
        check_error(strcmp(real_src_path, real_dest_path), "src and dest are the same");
    }
    free(real_src_path);
    free(real_dest_path);

    check_error(rename(src, dest) != -1, "rename");

}