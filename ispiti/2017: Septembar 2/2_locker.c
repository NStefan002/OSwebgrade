#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
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


int main(int argc, char **argv)
{
    check_error(argc == 5, "./2_locker file_path a b lock_type(r/w)");

    off_t a = atol(argv[2]);
    off_t b = atol(argv[3]);

    int fd = open(argv[1], O_RDWR);
    check_error(-1 != fd, "open");

    struct flock lock;
    lock.l_type = (argv[4][0] == 'r')? F_RDLCK : F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = a;
    lock.l_len = b;

    check_error(-1 != fcntl(fd, F_SETLK, &lock), "fcntl F_GETLK");

    sleep(100);

    check_error(-1 != close(fd), "close");

    return 0;
}