#include <stdio.h>
#include <stdlib.h>
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


void osPrintOwners(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./2 file_path");    

    osPrintOwners(argv[1]);

    return 0;
}

void osPrintOwners(const char *fpath)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    // man 2 getpwuid, man 2 getgrgid
    
    // ne treba da se oslobadja
    struct passwd *user_info = getpwuid(file_info.st_uid);    
    check_error(user_info != NULL, "getpwuid");

    // ne treba da se oslobadja
    struct group *group_info = getgrgid(file_info.st_gid);
    check_error(group_info != NULL, "getgrgid");

    printf("%s %s\n", user_info->pw_name, group_info->gr_name);
}