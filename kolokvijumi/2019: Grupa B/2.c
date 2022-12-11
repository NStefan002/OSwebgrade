#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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


bool sameOwnerAndGroup(const char *link_path);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./2 link");    

    if (sameOwnerAndGroup(argv[1]))
    {
        printf("da\n");
    }
    else
    {
        printf("ne\n");
    }
    
    return 0;
}

bool sameOwnerAndGroup(const char *link_path)
{
    struct stat file_info, link_info;
    // stat izdvaja podatke o fajlu na koji link pokazuje, a lstat o samom linku
    check_error(lstat(link_path, &link_info) != -1, "lstat");
    check_error(S_ISLNK(link_info.st_mode), "not a link");
    check_error(stat(link_path, &file_info) != -1, "stat");

    return (file_info.st_uid == link_info.st_uid && file_info.st_gid == link_info.st_gid);
}