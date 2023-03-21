#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

#define ARRAY_MAX (1024)

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

typedef struct
{
    sem_t inDataReady;
    float array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;


void *get_mem_block(const char *shm_path, size_t *size);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./5 shm_path");

    size_t size;
    OsInputData *mem_block = (OsInputData *)get_mem_block(argv[1], &size);

    check_error(-1 != sem_wait(&(mem_block->inDataReady)), "sem_wait");

    /* pogledati kako se racuna standardna devijacija */

    float arithmetic_mean = 0.0;
    for (unsigned i = 0; i < mem_block->arrayLen; i++)
    {
        arithmetic_mean += mem_block->array[i];
    }
    arithmetic_mean /= mem_block->arrayLen;

    float std_dev = 0.0;
    for (unsigned i = 0; i < mem_block->arrayLen; i++)
    {
        std_dev += (mem_block->array[i] - arithmetic_mean) * (mem_block->array[i] - arithmetic_mean);
    }
    std_dev /= mem_block->arrayLen;
    std_dev = sqrtf(std_dev);

    printf("%f\n", std_dev);

    check_error(-1 != munmap(mem_block, size), "munmap");

    return 0;
}

void *get_mem_block(const char *shm_path, size_t *size)
{
    int shmfd = shm_open(shm_path, O_RDWR, 0);
    check_error(-1 != shmfd, "shm_open");

    struct stat file_info;
    check_error(-1 != fstat(shmfd, &file_info), "fstat");
    *size = file_info.st_size;

    void *addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    check_error(MAP_FAILED != addr, "mmap");

    check_error(-1 != close(shmfd), "close");

    return addr;
}
