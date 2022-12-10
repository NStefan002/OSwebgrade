#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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


void readBcharsFromA(const char *fpath, off_t a, size_t b);

int main(int argc, char **argv)
{
    check_error(argc == 4, "./4 file_path a b");

    readBcharsFromA(argv[1], strtol(argv[2], NULL, 10), strtol(argv[3], NULL, 10));

    return 0;
}

void readBcharsFromA(const char *fpath, off_t a, size_t b)
{
    int fd;
    check_error((fd = open(fpath, O_RDONLY)) != -1, "open");

    check_error(lseek(fd, a, SEEK_SET), "lseek");
    
    char *mem_buf = malloc((b + 1) * sizeof(char));
    check_error(mem_buf != NULL, "malloc");

    ssize_t bytes_read;
    check_error((bytes_read = read(fd, mem_buf, b)) != -1, "read");
    mem_buf[bytes_read] = '\0';

    check_error(write(STDOUT_FILENO, mem_buf, bytes_read) != -1, "write");
    
    free(mem_buf);
    check_error(close(fd) != -1, "close");
}