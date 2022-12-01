#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>           
#include <sys/stat.h>


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


void osMkPublicDir(const char *dname);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./2 dirname");

    osMkPublicDir(argv[1]);

    return 0;
}

void osMkPublicDir(const char *dname)
{
    mode_t old_umask = umask(0);
    check_error(mkdir(dname, 0777) != -1, "mkdir");
    umask(old_umask);
}