#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
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

#define MAX_LEN (256)

bool positive_four_digit_number(const char *str);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./5 filepath");

    FILE *f = fopen(argv[1], "r+");
    check_error(NULL != f, "fopen");

    int fd = fileno(f);

    struct flock lock;
    char word[MAX_LEN];
    while (1 == fscanf(f, "%s", word))
    {
        if (positive_four_digit_number(word))
        {
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = ftell(f) - 4;
            lock.l_len = 4;

            errno = 0;
            int fcntl_ret_val = fcntl(fd, F_SETLK, &lock);
            if (-1 == fcntl_ret_val || F_RDLCK == lock.l_type)
            {
                // ako je errno neka od 2 navede vrednosti, to znaci
                // da neki drugi proces drzi lock
                check_error(EAGAIN == errno || EACCES == errno, "fcntl");
            }
            else
            {
                check_error(-1 != fseek(f, ftell(f) - 4, SEEK_SET), "fseek");
                fprintf(f, "####");
            }
        }
    }

    // otkljucavamo sve zakljucane delove (ako se ovaj deo zaboravi, close ce otklucati)
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // ! ukoliko je postavljeno na nulu cita do EOF-a

    check_error(-1 != fcntl(fd, F_SETLK, &lock), "fcntl");
    // fclose ce zatvoriti i fd pa nema potrebe da zovemo close()
    check_error(-1 != fclose(f), "fclose");

    return 0;
}

bool positive_four_digit_number(const char *str)
{
    // atoi vraca nulu kada ne uspe da pretvori string u int (man atoi)
    return strlen(str) == 4 && atoi(str) != 0;
}
