#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

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

#define pthread_check_error(pthread_err, msg)\
    do\
    {\
        int _pthread_err = pthread_err;\
        if (_pthread_err > 0)\
        {\
            errno = _pthread_err;\
            check_error(false, msg);\
        }\
    } while (0)
    
#define MAX (2086)

typedef struct 
{
    int result_array[MAX]; 
    int n;
    pthread_mutex_t lock;
} LOCKED_DATA;

typedef struct
{
    char *fpath;
    off_t start_byte;
    off_t num_of_bytes;
} THREAD_ARGS;

// globalna promenljiva u kojoj cuvamo rezultujuci niz
LOCKED_DATA result;

void *thread_func(void *arg);

int main(int argc, char **argv)
{
    check_error(3 == argc, "./1 n filepath");

    struct stat file_info;
    check_error(-1 != lstat(argv[2], &file_info), "lstat");

    int n = atoi(argv[1]);
    int m = file_info.st_size;

    pthread_t *thread_ids = (pthread_t *)malloc(n * sizeof(pthread_t));
    check_error(NULL != thread_ids, "malloc");

    THREAD_ARGS *args = malloc(n * sizeof(THREAD_ARGS));
    check_error(NULL != args, "malloc");    

    // polje n u globalnoj promenljivoj result oznacava prvo prazno mesto u
    // rezultujucem nizu na koje moze da se upise rezultat izvrsavanja niti
    result.n = 0;

    for (int i = 0; i < n; i++)
    {
        args[i].fpath = argv[2];
        args[i].num_of_bytes = m / n;
        args[i].start_byte = i * args[i].num_of_bytes;

        pthread_check_error(pthread_create(&thread_ids[i], NULL, &thread_func, &args[i]), "pthread_create"); 
    }

    for (int i = 0; i < n; i++)
    {
        pthread_check_error(pthread_join(thread_ids[i], NULL), "pthread_join");
    }

    for (int i = 0; i < result.n; i++)
    {
        printf("%d ", result.result_array[i]);
    }
    putchar('\n');

    free(thread_ids);
    free(args);
    
    return 0;
}

void *thread_func(void *arg)
{
    THREAD_ARGS info = *((THREAD_ARGS *)arg);

    FILE *f = fopen(info.fpath, "r");
    check_error(NULL != f, "fopen");

    // isto kao lseek samo za FILE umesto za fd
    check_error(-1 != fseek(f, info.start_byte, SEEK_SET), "fseek");

    // zbog oblika podniski sigurno se ne mogu pojaviti vise od n/3 puta
    int *beginnings = (int *)malloc((info.num_of_bytes / 3) * sizeof(int));
    check_error(NULL != beginnings, "malloc");
    // brojac niza beginnings
    int n = 0;

    // + 1 za terminirajucu nulu (fgets je sam postavlja)
    char *str = (char *)malloc((info.num_of_bytes + 1) * sizeof(char));
    check_error(NULL != str, "malloc");

    // mozemo da koristimo fgets jer znamo da fajl nema belina u sebi
    check_error(NULL != fgets(str, info.num_of_bytes + 1, f), "fgets");    

    // ne moramo da proveravamo zadnja dva karaktera niske jer su 
    // sve podniske duzine 3
    for (int i = 0; i < info.num_of_bytes - 2; i++)
    {
        if (!strncmp(str + i, "tag", 3) || !strncmp(str + i, "taa", 3) || !strncmp(str + i, "tga", 3))
        {
            beginnings[n++] = info.start_byte + i;
        }
    }

    free(str);
    free(beginnings);
    check_error(-1 != fclose(f), "fclose");    

    // ! kriticna sekcija
    pthread_check_error(pthread_mutex_lock(&result.lock), "mutex_lock");
    while (n)
    {
        result.result_array[result.n++] = beginnings[--n];
    }
    pthread_check_error(pthread_mutex_unlock(&result.lock), "mutex_unlock");

    return NULL;
}