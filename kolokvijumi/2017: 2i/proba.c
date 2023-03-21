#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

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


int main(int argc, char **argv)
{
    char *real_destpath = realpath(argv[2], NULL);
    printf("RDEST: %s\n", real_destpath);
    char *real_currpath = realpath(argv[1], NULL);
    int n = strrchr(real_currpath, '/') - real_currpath;
    printf("\tn : %d\n", n);
    char *file_destpath = malloc((strlen(real_destpath) + strlen(real_currpath + n) + 1) * sizeof(char));
    check_error(file_destpath != NULL, "malloc");

    strcpy(file_destpath, real_destpath);
    // ukljucuje '/'
    strcat(file_destpath, real_currpath+n);

    printf("CURR : %s\nDEST : %s\n", real_currpath, file_destpath);

    free(real_currpath);
    free(file_destpath);
    free(real_destpath);
    return 0;
}