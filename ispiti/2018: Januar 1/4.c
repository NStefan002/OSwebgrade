#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#define MAX_NAME_LEN (256)
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


char *fifo_name(char *fifo_path);

int main(int argc, char **argv)
{
    check_error(2 <= argc, "./4 fifo1 fifo2 ...");

    int num_of_fifos = argc - 1;
    struct pollfd *fifo_monitors = (struct pollfd *)calloc(num_of_fifos, sizeof(struct pollfd));
    check_error(NULL != fifo_monitors, "calloc");
    FILE **streams = (FILE **)malloc(num_of_fifos * sizeof(FILE *));
    check_error(NULL != streams, "malloc");

    // calloc da bi sve sume bile inicijalziovane na nulu
    unsigned *bytes_read = (unsigned *)calloc(num_of_fifos, sizeof(unsigned));

    for (int i = 0; i < num_of_fifos; i++)
    {
        fifo_monitors[i].fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(-1 != fifo_monitors[i].fd, "open");

        fifo_monitors[i].events = POLLIN;

        streams[i] = fdopen(fifo_monitors[i].fd, "r");
        check_error(NULL != streams[i], "fdopen");
    }

    int num_of_opened_fds = num_of_fifos;
    while (num_of_opened_fds)
    {
        check_error(-1 != poll(fifo_monitors, num_of_fifos, -1), "poll");

        for (int i = 0; i < num_of_fifos; i++)
        {
            if (fifo_monitors[i].revents & POLLIN)
            {
                while (EOF != fscanf(streams[i], "%*c"))
                {
                    bytes_read[i]++;
                }
            }
            else if (fifo_monitors[i].revents & (POLLERR | POLLHUP))
            {
                check_error(-1 != fclose(streams[i]), "fclose");
                num_of_opened_fds--;
            }

            fifo_monitors[i].revents = 0;
        }
    }

    free(fifo_monitors);
    free(streams);
    char *max_fifo = argv[0];
    unsigned max = 0;
    for (int i = 0; i < num_of_fifos; i++)
    {
        if (max < bytes_read[i])
        {
            max_fifo = argv[i + 1];
            max = bytes_read[i];
        }
    }

    printf("%s\n", max_fifo);

    free(bytes_read);
    return 0;
}

char *fifo_name(char *fifo_path)
{
    char *name = strrchr(fifo_path, '/');
    if (NULL == name)
    {
        return fifo_path;
    }
    else
    {
        return name + 1;
    }
}
