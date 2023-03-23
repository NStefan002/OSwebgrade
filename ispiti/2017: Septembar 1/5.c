#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
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

#define ARRAY_MAX (1024)

typedef struct{
    sem_t inDataReady;
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

void *get_mem_block(const char *shm_path, size_t *size);
bool is_power_of_3(int x);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./5 shm");

    size_t size;
    OsInputData *mem_block = (OsInputData *)get_mem_block(argv[1], &size);

    check_error(-1 != sem_wait(&(mem_block->inDataReady)), "sem_wait");

    for (int i = 0; i < mem_block->arrayLen; i++)
    {
        if (is_power_of_3(mem_block->array[i]))
        {
            printf("%d ", mem_block->array[i]);
        }
    }

    putchar('\n');

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

bool is_power_of_3(int x)
{
    for (int i = 3; i < x; i *= 3)
    {
        if (x % i)
        {
            return false;
        }
    }

    // ako je x jednako 1 onda je stepen trojke
    return x == 1 || x >= 3;
}
