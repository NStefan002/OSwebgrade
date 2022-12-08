#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
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


#define MAX_WORD (256)

void rw(const char *src, const char *dest);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./4 src dest");    

    rw(argv[1], argv[2]);

    return 0;
}

void rw(const char *src, const char *dest)
{
    int fd_src, fd_dest;
    check_error((fd_src = open(src, O_RDONLY)) != -1, "open1");
    check_error((fd_dest = open(dest, O_WRONLY)) != -1, "open1");

    ssize_t buf_size = 1U << 13;
    char *mem_buf = malloc(buf_size * sizeof(char));
    check_error(mem_buf != NULL, "malloc");

    ssize_t bytes_read;

    while ((bytes_read = read(fd_src, mem_buf, buf_size)) > 0)
    {
        mem_buf[bytes_read] = '\0';
        off_t offset;
        char word[MAX_WORD];
        int k;
        off_t mem_buf_offset = 0;
        // k cuva koliko je karaktera procitao sscanf
        while (sscanf(mem_buf + mem_buf_offset, "%ld%s%n", &offset, word, &k) == 2)
        {           
            mem_buf_offset += k;
            check_error(lseek(fd_dest, offset, SEEK_SET) != -1, "lseek");
            check_error(write(fd_dest, word, strlen(word)) != -1, "write");
        }
    }
    
    check_error(bytes_read != -1, "read");
    close(fd_src);
    close(fd_dest);
}