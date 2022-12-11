#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>

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


bool sameGrnameUsrname(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./1 file_path");

    if (sameGrnameUsrname(argv[1]))
    {
        printf("da\n");
    }
    else
    {
        printf("ne\n");
    }

    return 0;
}

bool sameGrnameUsrname(const char *fpath)
{
    struct stat file_info;
    check_error(lstat(fpath, &file_info) != -1, "stat");

    struct passwd *user_info;
    check_error((user_info = getpwuid(file_info.st_uid)) != NULL, "getpwnam");

    struct group *group_info;
    check_error((group_info = getgrgid(file_info.st_gid)) != NULL, "getgrnam");

    return !strcmp(user_info->pw_name, group_info->gr_name);
}