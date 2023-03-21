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


void *get_mem_block(const char *shm_path, size_t *size);
int compare(const void *a, const void *b);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./5 shared_mem_path");

    size_t size = 0;
    OsInputData *mem_block = (OsInputData *)get_mem_block(argv[1], &size);

    check_error(-1 != sem_wait(&(mem_block->inDataReady)), "sem_wait");

    // sortiramo niz da bismo nasli medijanu
    qsort(mem_block->array, mem_block->arrayLen, sizeof(float), &compare);

    printf("%f\n", mem_block->array[mem_block->arrayLen / 2]);

    check_error(-1 != munmap(mem_block, size), "munmap");
    // malo su cudni test primeri...
    // check_error(-1 != shm_unlink(argv[1]), "shm_unlink");

    return 0;
}

void *get_mem_block(const char *shm_path, size_t *size)
{
    /* FLAG ZA OTVARANJE DELJENJE MEMORIJE MORA DA BUDE O_RDWR,
    JER UKOLIKO JE O_RDONLY ONDA NE MOZEMO DA SPUSTAMO SEMAFOR
    (FUNKCIJA wait PRAVI SEG FAULT) !!! */
    // svejedno je koji cemo mode da stavimo jer fajl vec postoji
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

// man qsort
int compare(const void *a, const void *b)
{
    float x = *(float *)a;
    float y = *(float *)b;

    if (x < y)
    {
        return 1;
    }
    else if (x > y)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
