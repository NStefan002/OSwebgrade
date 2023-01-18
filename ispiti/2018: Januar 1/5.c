#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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
    sem_t dataProcessed;
    char str[ARRAY_MAX];
} OsInputData;
        
void *get_mem_block(const char *shm_path, size_t *size);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./5 shm");

    size_t size = 0;
    OsInputData *mem_block = (OsInputData *)get_mem_block(argv[1], &size);

    check_error(-1 != sem_wait(&(mem_block->inDataReady)), "sem_wait");

    for (int i = 0; mem_block->str[i]; i++)
    {
        if (islower(mem_block->str[i]))
        {
            mem_block->str[i] = toupper(mem_block->str[i]);
        }
        else if (isupper(mem_block->str[i]))
        {
            mem_block->str[i] = tolower(mem_block->str[i]);
        }
    }

    check_error(-1 != sem_init(&(mem_block->dataProcessed), 1, 1), "sem_init");

    check_error(-1 != munmap(mem_block, size), "munmap");

    // check_error(-1 != shm_unlink(argv[1]), "shm_unlink");

    return 0;
}

void *get_mem_block(const char *shm_path, size_t *size)
{
    int shmfd = shm_open(shm_path, O_RDWR, 0);
    check_error(-1 != shmfd, "shm_open");

    struct stat shm_info;
    check_error(-1 != fstat(shmfd, &shm_info), "fstat");

    *size = shm_info.st_size;

    void *addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    check_error(MAP_FAILED != addr, "mmap");

    check_error(-1 != close(shmfd), "close");
    return addr;
}
