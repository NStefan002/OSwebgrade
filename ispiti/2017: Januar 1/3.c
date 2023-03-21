#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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


typedef struct
{
    // vrednost globalnog minimuma i mutex koji obezbedjuje sinhronizaciju
    float value;
    pthread_mutex_t lock;
} LOCKED_DATA;

typedef struct
{
    // broj elemenata u vrsti i vrsta koju nit treba da obradi
    int m;
    float *row;
} THREAD_ARGS;


// globalna promenljiva u kojoj cuvamo minimum matrice
LOCKED_DATA global_min;

void *thread_func(void *arg);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./3 file_path");

    FILE *f = fopen(argv[1], "r");
    check_error(NULL != f, "fopen");

    int n, m;
    fscanf(f, "%d %d", &n, &m);

    // alokacija prostora za matricu
    float **mat = (float **)malloc(n * sizeof(float *));
    check_error(NULL != mat, "malloc");

    for (int i = 0; i < n; i++)
    {
        mat[i] = (float *)malloc(m * sizeof(float));
        check_error(NULL != mat[i], "malloc");
    }

    // ucitavanje matrice iz fajla
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            fscanf(f, "%f", &mat[i][j]);
        }
    }

    // niz koji cuva id svih niti (ima ih onoliko koliko ima vrsta u matrici)
    pthread_t *thread_ids = (pthread_t *)malloc(n * sizeof(pthread_t));
    check_error(NULL != thread_ids, "malloc");

    // niz koji cuva argumente za svaku nit
    THREAD_ARGS *args = (THREAD_ARGS *)malloc(n * sizeof(THREAD_ARGS));
    check_error(NULL != args, "malloc");

    // inicijalizujemo globalni minimum na prvi element u matrici
    global_min.value = mat[0][0];
    for (int i = 0; i < n; i++)
    {
        args[i].m = m;
        args[i].row = mat[i];
        pthread_check_error(pthread_create(&thread_ids[i], NULL, &thread_func, &args[i]), "pthread_create");
    }

    // cekamo sve niti da se izvrse
    for (int i = 0; i < n; i++)
    {
        // ne treba nam povratna vrednost, zato NULL u drugi argument
        pthread_check_error(pthread_join(thread_ids[i], NULL), "pthread_join");
    }

    printf("%f\n", global_min.value);

    // oslobadjamo svu alociranu memoriju i zatvaramo file stream
    for (int i = 0; i < n; i++)
    {
        free(mat[i]);
    }
    free(mat);
    free(thread_ids);
    free(args);
    check_error(-1 != fclose(f), "fclose");
    return 0;
}

void *thread_func(void *arg)
{
    // kastujemo arg u strukturu koja nama odgovara
    THREAD_ARGS mat_info = *((THREAD_ARGS *)arg);

    // ! prvo nalazimo minimum za vrstu pa ga onda uporedjujemo sa globalnim minimumom
    // (da bi kriticna sekcija bila sto manja)
    float local_min = mat_info.row[0];
    for (int i = 1; i < mat_info.m; i++)
    {
        if (local_min > mat_info.row[i])
        {
            local_min = mat_info.row[i];
        }
    }

    // ! kriticna sekcija
    pthread_check_error(pthread_mutex_lock(&global_min.lock), "mutex_lock");
    if (global_min.value > local_min)
    {
        global_min.value = local_min;
    }
    pthread_check_error(pthread_mutex_unlock(&global_min.lock), "mutex_unlock");

    return NULL;
}
