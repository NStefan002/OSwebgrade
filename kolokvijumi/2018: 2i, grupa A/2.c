#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
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


void renameDir(const char *old_name, const char *new_name);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./2 dir_name new_name");

    renameDir(argv[1], argv[2]);

    return 0;
}

void renameDir(const char *old_name, const char *new_name)
{
    struct stat file_info;
    check_error(stat(old_name, &file_info) != -1, "stat");

    check_error(S_ISDIR(file_info.st_mode), "file is not dir");

    char *real_old_name = realpath(old_name, NULL);
    // +1 da bi nam n bilo indeks prvog karaktera nakon '/'
    int n = strrchr(real_old_name, '/') - real_old_name + 1; 

    char *new = malloc((n + strlen(new_name) + 1) * sizeof(char));
    check_error(new != NULL, "malloc");

    strncpy(new, real_old_name, n);
    strcat(new, new_name);

    check_error(rename(old_name, new) != -1, "rename");
    free(new);
}