#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <utime.h>

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


void changeTime(const char *fpath, const char *sec);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./1 file_path sec");

    changeTime(argv[1], argv[2]);

    return 0;
}

void changeTime(const char *fpath, const char *sec)
{
    time_t time = strtol(sec, NULL, 10);

    struct utimbuf new_times;
    new_times.actime = time;
    new_times.modtime = time;
    
    check_error(utime(fpath, &new_times) != -1, "utime");   
}