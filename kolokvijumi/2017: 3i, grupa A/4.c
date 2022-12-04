#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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


void osRWA(const char *option, const char *fpath);
void osR(const char *fpath);
void osW(const char *fpath);
void osA(const char *fpath);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./4 -[r/w/a] file_path");

    osRWA(argv[1], argv[2]);

    return 0;
}

void osRWA(const char *option, const char *fpath)
{
    check_error(option[0] == '-', "not an option");

    switch (option[1])
    {
    case 'r':
        osR(fpath);
        break;
    
    case 'w':
        osW(fpath);
        break;
        
    case 'a':
        osA(fpath);
        break;

    default:
        check_error(false, "invalid option");
        break;
    };
}

void osR(const char *fpath)
{
    int fd;
    check_error((fd = open(fpath, O_RDONLY)) != -1, "open");

    ssize_t buffer_size = 1U << 13; // 8KB   
    char *mem_buffer = malloc(buffer_size * sizeof(char));
    check_error(mem_buffer != NULL, "malloc");

    ssize_t bytes_read = 0;
    while ((bytes_read = read(fd, mem_buffer, buffer_size)) > 0)
    {
        check_error(write(STDOUT_FILENO, mem_buffer, bytes_read) == bytes_read, "write");
    }

    check_error(bytes_read == 0, "read");
}

void osW(const char *fpath)
{
    static mode_t default_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int fd;
    check_error((fd = open(fpath, O_WRONLY | O_CREAT | O_TRUNC, default_mode)) != -1, "open");

    ssize_t buffer_size = 1U << 13; // 8KB   
    char *mem_buffer = malloc(buffer_size * sizeof(char));
    check_error(mem_buffer != NULL, "malloc");

    ssize_t bytes_read = 0;
    while ((bytes_read = read(STDIN_FILENO, mem_buffer, buffer_size)) > 0)
    {
        check_error(write(fd, mem_buffer, bytes_read) == bytes_read, "write");
    }

    check_error(bytes_read == 0, "read");
}

void osA(const char *fpath)
{
    int fd;
    check_error((fd = open(fpath, O_WRONLY | O_APPEND)) != -1, "open");

    ssize_t buffer_size = 1U << 13; // 8KB   
    char *mem_buffer = malloc(buffer_size * sizeof(char));
    check_error(mem_buffer != NULL, "malloc");

    ssize_t bytes_read = 0;
    while ((bytes_read = read(STDIN_FILENO, mem_buffer, buffer_size)) > 0)
    {
        check_error(write(fd, mem_buffer, bytes_read) == bytes_read, "write");
    }

    check_error(bytes_read == 0, "read");
}