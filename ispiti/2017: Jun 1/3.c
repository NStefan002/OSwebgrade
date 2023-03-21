#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
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


#define UNUSED(x) ((void)x)

typedef struct
{
    // vrednost globalnog maksimuma i mutex koji obezbedjuje sinhronizaciju
    int value;
    pthread_mutex_t lock;
} LOCKED_DATA;

typedef struct
{
    // polje i, j rezultujuce matrice je proizvod i-te vrste prve i j-te kolone druge matrice
    int **mat1;
    int **mat2;
    int i;
    int j;
    // dimenzija vrste prve matrice i kolone druge matrice
    int M;
} THREAD_ARGS;


// globalna promenljiva u kojoj cuvamo maksimum rezultujuce matrice
LOCKED_DATA global_max;
// rezultujuca matrica
int **res_mat;

int **alocate_matrix(int n, int m);
void set_matrix(int **mat, int n, int m);
void print_matrix(int **mat, int n, int m);
void free_matrix(int **mat, int n);

void *thread_func(void *arg);

int main(int argc, char **argv)
{
    // ne treba nam argv
    UNUSED(argv);
    check_error(1 == argc, "no arguments needed");

    int N, M, K;
    scanf("%d%d", &N, &M);

    int **mat_NxM = alocate_matrix(N, M);
    set_matrix(mat_NxM, N, M);

    scanf("%d%d", &M, &K);
    int **mat_MxK = alocate_matrix(M, K);
    set_matrix(mat_MxK, M, K);

    res_mat = alocate_matrix(N, K);

    pthread_t **thread_ids = (pthread_t **)malloc(N * sizeof(pthread_t *));
    check_error(NULL != thread_ids, "malloc");
    for (int i = 0; i < N; i++)
    {
        thread_ids[i] = (pthread_t *)malloc(K * sizeof(pthread_t));
        check_error(NULL != thread_ids[i], "malloc");
    }

    THREAD_ARGS **args = (THREAD_ARGS **)malloc(N * sizeof(THREAD_ARGS *));
    check_error(NULL != args, "malloc");
    for (int i = 0; i < N; i++)
    {
        args[i] = (THREAD_ARGS *)malloc(K * sizeof(THREAD_ARGS));
        check_error(NULL != args[i], "malloc");
    }

    global_max.value = INT_MIN;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < K; j++)
        {
            args[i][j].i = i;
            args[i][j].j = j;
            args[i][j].mat1 = mat_NxM;
            args[i][j].mat2 = mat_MxK;
            args[i][j].M = M;

            pthread_check_error(pthread_create(&thread_ids[i][j], NULL, &thread_func, &args[i][j]), "pthread_create");
        }
    }

    // cekamo da se sve niti izvrse
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < K; j++)
        {
            pthread_check_error(pthread_join(thread_ids[i][j], NULL), "pthread_join");
        }
    }

    print_matrix(res_mat, N, K);
    printf("%d\n", global_max.value);

    // oslobadjamo svu alociranu memoriju
    free_matrix(mat_NxM, N);
    free_matrix(mat_MxK, M);
    free(thread_ids);
    free(args);
    return 0;
}

void *thread_func(void *arg)
{
    // kastujemo arg u strukturu koja nama odgovara
    THREAD_ARGS info = *((THREAD_ARGS *)arg);

    int local_res = 0;
    for (int k = 0; k < info.M; k++)
    {
        // mnozimo i-tu vrstu prve matrice sa j-tom kolonom druge
        local_res += info.mat1[info.i][k] * info.mat2[k][info.j];
    }

    // rezultat upisujemo na odgovarajuce polje u rezultujucoj matrici
    res_mat[info.i][info.j] = local_res;

    // ! kriticna sekcija
    pthread_check_error(pthread_mutex_lock(&global_max.lock), "mutex_lock");
    if (global_max.value < local_res)
    {
        global_max.value = local_res;
    }
    pthread_check_error(pthread_mutex_unlock(&global_max.lock), "mutex_unlock");

    return NULL;
}

int **alocate_matrix(int n, int m)
{
    int **mat = (int **)malloc(n * sizeof(int *));
    check_error(NULL != mat, "malloc");

    for (int i = 0; i < n; i++)
    {
        mat[i] = (int *)malloc(m * sizeof(int));
        check_error(NULL != mat[i], "malloc");
    }

    return mat;
}

void set_matrix(int **mat, int n, int m)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            scanf("%d", &mat[i][j]);
        }
    }
}

void print_matrix(int **mat, int n, int m)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            printf("%d ", mat[i][j]);
        }
        putchar('\n');
    }
}

void free_matrix(int **mat, int n)
{
    for (int i = 0; i < n; i++)
    {
        free(mat[i]);
        mat[i] = NULL;
    }
    free(mat);
    mat = NULL;
}
