#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
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


#define KB_TO_B (1U << 10)
#define MB_TO_B (1U << 20)
#define GB_TO_B (1U << 30)


void printSize(const char *fpath, const char *unit);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./2 file_path [KB/MB/GB]");    

    printSize(argv[1], argv[2]);

    return 0;
}

void printSize(const char *fpath, const char *unit)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    // ne moramo da proveravamo da li je realpath vratio NULL, jer ce stat proveriti da li fajl postoji
    char *real_fpath = realpath(fpath, NULL);
    char *slash = strrchr(real_fpath, '/');
    char *file_name = malloc(slash - real_fpath);
    // ime fajla se nalazi iza poslednjeg '/' karaktera
    // strcpy postavlja terminirajucu nulu
    strcpy(file_name, slash+1);

    if (!strcmp(unit, "KB"))
    {
        printf("%s %ldKB\n", file_name, file_info.st_size / KB_TO_B + ((file_info.st_size % KB_TO_B > 0)? 1 : 0));
    }
    else if (!strcmp(unit, "MB"))
    {
        printf("%s %ldMB\n", file_name, file_info.st_size / MB_TO_B + ((file_info.st_size % MB_TO_B > 0)? 1 : 0));
    }
    else if (!strcmp(unit, "GB"))
    {
        printf("%s %ldGB\n", file_name, file_info.st_size / GB_TO_B + ((file_info.st_size % GB_TO_B > 0)? 1 : 0));
    }
    else
    {
        check_error(false, "invalid size");
    }

    free(real_fpath);
    free(file_name);
}