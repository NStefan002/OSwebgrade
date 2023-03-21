#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <limits.h>
#include <float.h>

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

#define MAX_EVENTS (10)

typedef struct 
{
    FILE *f;
    int index;
} EPOLL_DATA;

int main(int argc, char **argv)
{
    check_error(2 <= argc, "./3 fifo1 fifo2 ...");

    int num_of_fifos = argc - 1;
    int epoll_fd = epoll_create(num_of_fifos);
    check_error(-1 != epoll_fd, "epoll_create");

    EPOLL_DATA *fifo_data = (EPOLL_DATA *)calloc(num_of_fifos, sizeof(EPOLL_DATA));
    check_error(NULL != fifo_data, "calloc");

    struct epoll_event current_event;
    for (int i = 0; i < num_of_fifos; i++)
    {
        // brisanje sadrzaja eventa
        memset(&current_event, 0, sizeof(struct epoll_event));
        current_event.events = EPOLLIN;

        current_event.data.fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(-1 != current_event.data.fd, "open");

        fifo_data[i].f = fdopen(current_event.data.fd, "r");
        check_error(NULL != fifo_data[i].f, "fdopen");
        fifo_data[i].index = i;

        current_event.data.ptr = &fifo_data[i];

        check_error(-1 != epoll_ctl(epoll_fd, EPOLL_CTL_ADD, current_event.data.fd, &current_event), "epoll_ctl");
    }

    double max = -DBL_MAX;
    char *max_fifo = NULL;
    struct epoll_event ready_fifos[MAX_EVENTS];
    int num_of_fifos_cp = num_of_fifos;
    while (num_of_fifos_cp)
    {
        int num_of_ready_fifos;
        // * man epoll_wait
        check_error(-1 != (num_of_ready_fifos = epoll_wait(epoll_fd, ready_fifos, MAX_EVENTS, -1)), "epoll_wait");   

        for (int i = 0; i < num_of_ready_fifos; i++)
        {
            EPOLL_DATA *curr = (EPOLL_DATA *)ready_fifos[i].data.ptr;
            if (ready_fifos[i].events & EPOLLIN)
            {
                double x;
                double sum = 0.0;
                while (1 == fscanf(curr->f, "%lf", &x))
                {
                    sum += x;
                }
                if (max < sum)
                {
                    max = sum;
                    max_fifo = argv[curr->index + 1];
                }
            }
            else if (ready_fifos[i].events & (EPOLLHUP | EPOLLERR))
            {
                check_error(-1 != fclose(curr->f), "fclose"); // zatvara i zeljeni fd
                num_of_fifos_cp--;
            }
        }
    }

    printf("%s\n", max_fifo);

    check_error(-1 != close(epoll_fd), "close");
    for (int i = 0; i < num_of_fifos; i++)
    {
        check_error(-1 != fclose(fifo_data[i].f), "fclose");
    }
    
    free(fifo_data);

    return 0;
}