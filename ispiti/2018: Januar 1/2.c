#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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

#define thread_check_error(thread_err, msg)\
    do\
    {\
        int _thread_err = thread_err;\
        if (_thread_err > 0)\
        {\
            errno = _thread_err;\
            check_error(false, msg);\
        }\
    } while (0)

typedef struct
{
    int sum;
    pthread_mutex_t lock;
} LOCKED_DATA;

typedef struct
{
    char *W;
    char *fpath;
    off_t start;
    off_t end;
} THREAD_ARGS;

LOCKED_DATA global_sum;

void *thread_func(void *args);

int main(int argc, char **argv)
{
    check_error(4 == argc, "./2 filepath W K");

    struct stat file_info;
    check_error(-1 != stat(argv[1], &file_info), "stat");

    int n = file_info.st_size;
    int k = atoi(argv[3]);

    pthread_t *thread_ids = (pthread_t *)malloc(k * sizeof(pthread_t));
    check_error(NULL != thread_ids, "malloc");

    THREAD_ARGS *args = (THREAD_ARGS *)malloc(k * sizeof(THREAD_ARGS));
    check_error(NULL != args, "malloc");

    global_sum.sum = 0;
    for (int i = 0; i < k; i++)
    {
        args[i].fpath = argv[1];
        args[i].W = argv[2];
        args[i].start = i * n / k;
        args[i].end = (i + 1) * n / k;
        thread_check_error(pthread_create(&thread_ids[i], NULL, &thread_func, &args[i]), "pthread_create");
    }

    for (int i = 0; i < k; i++)
    {
        thread_check_error(pthread_join(thread_ids[i], NULL), "pthread_join");
    }

    printf("%d\n", global_sum.sum);

    free(thread_ids);
    free(args);
    return 0;
}

void *thread_func(void *args)
{
    /* Prosledjujemo putanju do fajla i svaka nit ga zasebno otvara,
    jer ako prosledjujemo FILE *f onda se moze desiti da vise niti
    pozove fseek odjednom i da sve dobiju pogresan offset. */
    THREAD_ARGS arg = *((THREAD_ARGS *)args);
    FILE *f = fopen(arg.fpath, "r");
    check_error(NULL != f, "fopen");

    check_error(-1 != fseek(f, arg.start, SEEK_SET), "fseek");

    int local_sum = 0;
    char *s = (char *)malloc((arg.end - arg.start + 1) * sizeof(char));
    check_error(NULL != s, "malloc");

    int i = 0;
    while (EOF != (s[i] = fgetc(f)) && i < arg.end - arg.start)
    {
        i++;
    }

    s[i] = '\0';    

    int len_W = strlen(arg.W);
    for (int i = 0; i < arg.end - arg.start; i++)
    {
        if (!strncmp(s + i, arg.W, len_W))
        {
            local_sum++;
        }
    }
    
    // ! kriticna sekcija 
    thread_check_error(pthread_mutex_lock(&global_sum.lock), "mutex_lock");
    global_sum.sum += local_sum;
    thread_check_error(pthread_mutex_unlock(&global_sum.lock), "mutex_unlock");
    // ! kraj kriticne sekcije

    check_error(-1 != fclose(f), "fclose");

    return NULL;
}