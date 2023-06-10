#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


#define check_error(cond, msg)\
    do {\
        if (!(cond))\
        {\
            perror(msg);\
            fprintf(stderr, "File: %s\nFunc: %s\nLine: %d\n", __FILE__, __func__, __LINE__);\
            exit(EXIT_FAILURE);\
        }\
    } while (0)

#define ARRAY_MAX (1024)

typedef struct 
{
    sem_t dataProcessingFinished;
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

int command; // 0 - cekaj signal, 1 - uhvatio SIGUSR1, 2 - uhvatio SIGUSR2

void *get_mem_block(const char *shmpath, size_t *size);
void signal_handler(int signum);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./5 shm");

    size_t size;
    OsInputData *mem_block = (OsInputData *)get_mem_block(argv[1], &size);

    command = 0;
    do {
        check_error(SIG_ERR != signal(SIGUSR1, &signal_handler), "signal");
        check_error(SIG_ERR != signal(SIGUSR2, &signal_handler), "signal");
    } while (0 == command);

    if (1 == command)
    {
        for (unsigned i = 0; i < mem_block->arrayLen; i++)
        {
            mem_block->array[i] *= -1;
        }
    }
    else 
    {
        for (unsigned i = 0; i < mem_block->arrayLen; i++)
        {
            mem_block->array[i] *= 2;
        }
    }

    check_error(-1 != sem_post(&(mem_block->dataProcessingFinished)), "sem_post");

    check_error(-1 != munmap(mem_block, size), "munmap");
    
    return 0;
}

void signal_handler(int signum)
{
    if (SIGUSR1 == signum) command = 1;
    else if (SIGUSR2 == signum) command = 2;
}

void *get_mem_block(const char *shmpath, size_t *size)
{
    int shm_fd = shm_open(shmpath, O_RDWR, 0);
    check_error(-1 != shm_fd, "shm_open");

    struct stat file_info;
    check_error(-1 != fstat(shm_fd, &file_info), "fstata");

    *size = file_info.st_size;

    void *addr = mmap(NULL, sizeof(OsInputData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    check_error(MAP_FAILED != addr, "mmap");

    check_error(-1 != close(shm_fd), "fclose");

    return addr;
}
