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


void osPrintFromOffset(const char *fpath, off_t offset, ssize_t len);

int main(int argc, char **argv)
{
    check_error(argc == 4, "./4 file_path offset len");    

    osPrintFromOffset(argv[1], strtol(argv[2], NULL, 10), strtol(argv[3], NULL, 10));

    return 0;
}

void osPrintFromOffset(const char *fpath, off_t offset, ssize_t len)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");
    check_error(S_ISREG(file_info.st_mode), "file is not regular");

    int fd;
    check_error((fd = open(fpath, O_RDONLY)) != -1, "open");
    check_error(lseek(fd, offset, SEEK_SET) != -1, "lseek");

    char *mem_buf = malloc(len * sizeof(char));
    check_error(mem_buf != NULL, "malloc");

    ssize_t bytes_read = read(fd, mem_buf, len);
    check_error(bytes_read != -1, "read");

    check_error(write(STDOUT_FILENO, mem_buf, bytes_read) != -1, "write");

    free(mem_buf);
    close(fd);
}