#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
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
   } while (0)\

#define NUM_OF_ALPHA (26) // br slova u alfabetu

typedef struct
{
    char *file_path;
    char c;
} THREAD_ARGS;

typedef struct
{
    char max_c;
    int max_count;
    pthread_mutex_t lock;
} LOCKED_DATA;

LOCKED_DATA global_max;
int *counters;

void *thread_func(void *args);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./2 file_path");

    struct stat file_info;
    check_error(-1 != stat(argv[1], &file_info), "stat"); // da li fajl postoji

    thread_check_error(pthread_mutex_init(&(global_max.lock), NULL), "pthread_mutex_init");
    global_max.max_c = 'a';
    global_max.max_count = 0;
    counters = (int *)calloc(NUM_OF_ALPHA, sizeof(int));
    check_error(NULL != counters, "calloc");
    THREAD_ARGS args[NUM_OF_ALPHA];
    pthread_t threads[NUM_OF_ALPHA];
    for (int i = 0; i < NUM_OF_ALPHA; i++)
    {
        args[i].file_path = argv[1];
        args[i].c = 'a' + i;

        thread_check_error(pthread_create(&threads[i], 0, &thread_func, &args[i]), "pthread_create");
    }

    for (int i = 0; i < NUM_OF_ALPHA; i++)
    {
        thread_check_error(pthread_join(threads[i], NULL), "pthread_join");
    }

    for (int i = 0; i < NUM_OF_ALPHA; i++)
    {
        printf("%d ", counters[i]);
    }
    printf("\n%c\n", global_max.max_c);

    free(counters); counters = NULL;

    return 0;
}

void *thread_func(void *args)
{
    THREAD_ARGS arg = *((THREAD_ARGS *)args);
    FILE *f = fopen(arg.file_path, "r");
    check_error(NULL != f, "fopen");

    char c;
    int count = 0;
    while (EOF != (c = fgetc(f)))
    {
        if (tolower(c) == tolower(arg.c))
        {
            count++;
        }
    }

    check_error(-1 != fclose(f), "fclose");

    counters[arg.c - 'a'] = count;

    // kriticna sekcija
    thread_check_error(pthread_mutex_lock(&(global_max.lock)), "pthread_mutex_lock");
    if (global_max.max_count < count)
    {
        global_max.max_count = count;
        global_max.max_c = arg.c;
    }
    thread_check_error(pthread_mutex_unlock(&(global_max.lock)), "pthread_mutex_unlock");
    // kriticna sekcija

    return NULL;
}
