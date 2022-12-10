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


#define H_TO_SEC (60 * 60)

time_t diffAtimeMtime(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./1 file_path");

    printf("%ld\n", diffAtimeMtime(argv[1]));

    return 0;
}

time_t diffAtimeMtime(const char *fpath)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    time_t razlika_u_sekundama = file_info.st_atime - file_info.st_mtime; 
    
    // zaokruzujemo na vecu decimalu
    time_t razlika_u_satima = (razlika_u_sekundama / H_TO_SEC) + ((razlika_u_sekundama % H_TO_SEC)? 1 : 0);
 
    return razlika_u_satima;
}