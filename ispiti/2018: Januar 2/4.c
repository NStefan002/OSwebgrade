#include <stdio.h>
#include <stdlib.h>
#include <string.h> // za memset
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

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
#define MAX_LEN (4096)

typedef struct
{
    int idx; // za cuvanje rednog broja fifo fajla
    int fd;
    unsigned chars;
} EPOLL_DATA;

int main(int argc, char **argv)
{
    check_error(2 <= argc, "./4 fifo1 fifo2 ...");

    int num_of_fifos = argc - 1;
    int epoll_fd = epoll_create(num_of_fifos);
    check_error(-1 != epoll_fd, "epoll_create");

    struct epoll_event current_event;
    EPOLL_DATA *fifo_data = (EPOLL_DATA *)malloc(num_of_fifos * sizeof(EPOLL_DATA));
    check_error(NULL != fifo_data, "malloc");
    for (int i = 0; i < num_of_fifos; i++)
    {
        memset(&current_event, 0, sizeof(struct epoll_event));
        current_event.events = EPOLLIN;

        int fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(-1 != fd, "open");

        fifo_data[i].fd = fd;

        fifo_data[i].idx = i;
        current_event.data.ptr = &fifo_data[i];

        check_error(-1 != epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &current_event), "epoll_ctl");
    }

    struct epoll_event ready_fifos[MAX_EVENTS];
    while (num_of_fifos)
    {
        int num_of_ready_fifos = epoll_wait(epoll_fd, ready_fifos, MAX_EVENTS, -1);
        check_error(-1 != num_of_ready_fifos, "epoll_wait");

        for (int i = 0; i < num_of_ready_fifos; i++)
        {
            EPOLL_DATA *curr = (EPOLL_DATA *)ready_fifos[i].data.ptr;
            if (ready_fifos[i].events & EPOLLIN)
            {
                ssize_t bytes_read = 0;
                char text[MAX_LEN];
                while (0 < (bytes_read = read(curr->fd, text, MAX_LEN)))
                {
                    curr->chars += bytes_read;
                    text[bytes_read] = 0;
                }
                check_error(-1 != bytes_read, "read");
            }
            else if (ready_fifos[i].events & (EPOLLHUP | EPOLLERR))
            {
                num_of_fifos--;
            }
        }
    }

    unsigned min_chars = UINT_MAX;
    char *min_fifo = NULL;

    num_of_fifos = argc - 1;
    for (int i = 0; i < num_of_fifos; i++)
    {
        if (fifo_data[i].chars < min_chars)
        {
            min_chars = fifo_data[i].chars;
            min_fifo = argv[i + 1];
        }

        check_error(-1 != close(fifo_data[i].fd), "close");
    }

    printf("%s", min_fifo);

    free(fifo_data); fifo_data = NULL;
    check_error(-1 != close(epoll_fd), "close");

    return 0;
}
