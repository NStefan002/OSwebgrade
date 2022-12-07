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


void swapGRPandOTH(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./3 file_path");

    swapGRPandOTH(argv[1]);    

    return 0;
}

void swapGRPandOTH(const char *fpath)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    mode_t mode = 0;
    /* bitovi su redom od najveceg do najmanjeg uuugggooo
    shiftovanjem prava pristupa za grupu za tri mesta u desno ta prava
    postaju prava pristupa za ostale, a shiftovanjem prava pristupa za ostale
    za tri mesta u levo ta prava postaju prava pristupa za grupu */
    mode |= (S_IRWXU & file_info.st_mode);
    mode |= (S_IRWXG & file_info.st_mode) >> 3;
    mode |= (S_IRWXO & file_info.st_mode) << 3;

    check_error(chmod(fpath, mode) != -1, "chmod");
}