#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

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
    float array[ARRAY_MAX]; // ARRAY_MAX = 1024
    unsigned arrayLen;
} OsInputData;

void *create_mem_block(const char *shm_path, size_t size);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./5_writer shared_mem_path");

    OsInputData *mem_block = (OsInputData *)create_mem_block(argv[1], sizeof(OsInputData));

    check_error(-1 != sem_init(&(mem_block->inDataReady), 1, 1), "sem_init");

    unsigned n;
    scanf("%d", &n);
    mem_block->arrayLen = n;

    for (unsigned i = 0; i < n; i++)
    {
        float elem;
        scanf("%f,", &elem);
        mem_block->array[i] = elem;
    }

    check_error(-1 != munmap(mem_block, sizeof(OsInputData)), "munmap");

    return 0;
}

void *create_mem_block(const char *shm_path, size_t size)
{
    int shmfd = shm_open(shm_path, O_RDWR | O_TRUNC | O_CREAT, 0644);
    check_error(shmfd != -1, "shm_open");

    check_error(-1 != ftruncate(shmfd, size), "ftruncate");

    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    check_error(MAP_FAILED != addr, "mmap");

    check_error(-1 != close(shmfd), "close");

    return addr;   
}