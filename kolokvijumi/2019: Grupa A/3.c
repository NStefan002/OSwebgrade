#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
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


void swapText(const char *fpath, off_t a, off_t b, size_t n);

int main(int argc, char **argv)
{
    check_error(argc == 5, "./3 file_path a b n");

    swapText(argv[1], strtol(argv[2], NULL, 10), strtol(argv[3], NULL, 10), strtol(argv[4], NULL, 10));

    return 0;
}

void swapText(const char *fpath, off_t a, off_t b, size_t n)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");
    
    check_error(S_ISREG(file_info.st_mode), "not a reg file");
       
    int fd;
    check_error((fd = open(fpath, O_RDWR)) != -1, "open");
    char *mem_buf_a = malloc(n * sizeof(char));
    check_error(mem_buf_a != NULL, "malloc");

    char *mem_buf_b  = malloc(n * sizeof(char));
    check_error(mem_buf_b != NULL, "malloc");

    ssize_t read_a, read_b;
    
    // pokusavamo da citamo n karaktera od pozicije a
    check_error(lseek(fd, a, SEEK_SET) != (off_t)-1, "lseek");
    check_error((read_a = read(fd, mem_buf_a, n)) != -1, "read");
    
    // pokusavamo da citamo n karaktera od pozicije b
    check_error(lseek(fd, b, SEEK_SET) != (off_t)-1, "lseek");
    check_error((read_b = read(fd, mem_buf_b, n)) != -1, "read");

    // pokusavamo da pisemo karaktere procitane sa pozicije a na poziciju b
    check_error(lseek(fd, b, SEEK_SET) != (off_t)-1, "lseek");
    check_error(write(fd, mem_buf_a, read_a) != -1, "write");

    // pokusavamo da pisemo karaktere procitane sa pozicije b na poziciju a
    check_error(lseek(fd, a, SEEK_SET) != (off_t)-1, "lseek");
    check_error(write(fd, mem_buf_b, read_b) != -1, "write");

    check_error(close(fd) != -1, "close");
    free(mem_buf_a);
    free(mem_buf_b);
}