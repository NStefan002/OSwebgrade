#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <math.h>
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

#define UNUSED(x) ((void)x)

typedef struct
{
    double x;
    double y;
} POINT;

typedef struct
{
    POINT *arr;
    int n;
    int idx;
} THREAD_ARGS;

typedef struct
{
    pthread_mutex_t lock;
    double min_distance;
} LOCKED_DATA;

LOCKED_DATA global_min;

void *thread_func(void *args);
double sqr(double x);
double distance(POINT a, POINT b);
double min(double x, double y);

int main(int argc, char **argv)
{
    UNUSED(argv);
    check_error(1 == argc, "no arguments needed");

    int allocated = 100; // za realokaciju niza
    POINT *points_arr = (POINT *)malloc(allocated * sizeof(POINT));
    check_error(NULL != points_arr, "malloc");

    int n = 0;

    double x, y;
    while (EOF != scanf("%lf%lf", &x, &y))
    {
        if (n == allocated)
        {
            allocated *= 2;
            points_arr = (POINT *)realloc(points_arr, allocated * sizeof(POINT));
            check_error(NULL != points_arr, "realloc");
        }
        points_arr[n].x = x;
        points_arr[n].y = y;
        n++;
    }

    pthread_t *threads = (pthread_t *)malloc(n * sizeof(pthread_t));
    check_error(NULL != threads, "malloc");
    THREAD_ARGS *args = (THREAD_ARGS *)malloc(n * sizeof(THREAD_ARGS));
    check_error(NULL != args, "malloc");

    thread_check_error(pthread_mutex_init(&global_min.lock, NULL), "pthread_mutex_init");
    global_min.min_distance = DBL_MAX;

    for (int i = 0; i < n; i++)
    {
        args[i].arr = points_arr;
        args[i].idx = i;
        args[i].n = n;

        thread_check_error(pthread_create(&threads[i], NULL, &thread_func, &args[i]), "pthread_create");
    }

    for (int i = 0; i < n; i++)
    {
        thread_check_error(pthread_join(threads[i], NULL), "pthread_join");
    }

    printf("%lf\n", global_min.min_distance);

    free(points_arr); points_arr = NULL;
    free(args); args = NULL;
    free(threads); threads = NULL;

    return 0;
}

double distance(POINT a, POINT b)
{
    return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y));
}

double sqr(double x)
{
    return x * x;
}

double min(double x, double y)
{
    return x < y ? x : y;
}

void *thread_func(void *args)
{
    THREAD_ARGS arg = *((THREAD_ARGS *)args);
    double local_min = DBL_MAX;

    for (int i = 0; i < arg.n; i++)
    {
        if (i == arg.idx)
        {
            continue;
        }
        local_min = min(local_min, distance(arg.arr[arg.idx], arg.arr[i]));
    }

    // kriticna sekcija
    thread_check_error(pthread_mutex_lock(&global_min.lock), "mutex_lock");
    global_min.min_distance = min(global_min.min_distance, local_min);
    thread_check_error(pthread_mutex_unlock(&global_min.lock), "mutex_unlock");

    return NULL;
}
