#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;


void *get_mem_block(const char *shm_path, size_t *size);
bool has_min_of_four_1_in_binary(int x);

int main(int argc, char **argv)
{
    check_error(argc == 2, "./5 shm_path");

    size_t size;
    OsInputData *mem_block = (OsInputData *)get_mem_block(argv[1], &size);

    check_error(-1 != sem_wait(&(mem_block->inDataReady)), "sem_wait");
    
    for (unsigned i = 0; i < mem_block->arrayLen; i++)
    {
        if (has_min_of_four_1_in_binary(mem_block->array[i]))
        {
            printf("%d ", mem_block->array[i]);
        }
    }
    putchar('\n');

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

bool has_min_of_four_1_in_binary(int x)
{
    unsigned mask = 1u << (8 * sizeof(unsigned) - 1);
    unsigned num_of_1 = 0;
    while (mask)
    {
        if (x & mask)
        {
            num_of_1++;
        }
        
        mask >>= 1;
    }

    return num_of_1 >= 4;
}