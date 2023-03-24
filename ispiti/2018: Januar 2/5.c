#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
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

#define ARRAY_MAX (1024)

typedef struct
{
    sem_t inDataReady;
    sem_t dataProcessed;
    char str[ARRAY_MAX];
} OsInputData;

void *get_mem_block(const char *shm_path, size_t *size);
void reverse_string(char *str);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./5 shm_path");

    size_t size;
    OsInputData *mem_block = (OsInputData *)get_mem_block(argv[1], &size);

    check_error(-1 != sem_wait(&(mem_block->inDataReady)), "sem_wait");

    reverse_string(mem_block->str);

    check_error(-1 != sem_post(&(mem_block->dataProcessed)), "sem_post");

    check_error(-1 != munmap(mem_block, size), "munmap");

    return 0;
}

void *get_mem_block(const char *shm_path, size_t *size)
{
    int shm_fd = shm_open(shm_path, O_RDWR, 0);
    check_error(-1 != shm_fd, "shm_open");

    struct stat file_info;
    check_error(-1 != fstat(shm_fd, &file_info), "fstat");
    *size = file_info.st_size;

    void *addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    check_error(MAP_FAILED != addr, "mmap");

    check_error(-1 != close(shm_fd), "close");

    return addr;
}

void reverse_string(char *str)
{
    for (int i = 0, j = strlen(str) - 1; i < j; i++, j--)
    {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
}
