#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX_LEN (256)

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

void invert(char *s, size_t len);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./4 file_path");

    FILE *f = fopen(argv[1], "r+");
    check_error(NULL != f, "fopen");

    int fd = fileno(f);
    check_error(-1 != fd, "fileno");

    unsigned num_of_unlocked = 0;
    unsigned num_of_locked = 0;
    char word[MAX_LEN];
    struct flock lock;
    while (1 == fscanf(f, "%s", word))
    {
        size_t word_len = strlen(word);
        lock.l_type = F_RDLCK | F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = ftell(f) - word_len; // ne moze biti < 0 jer je procitano bar word_len bajtova
        lock.l_len = word_len;

        errno = 0;
        int fcntl_ret_val = fcntl(fd, F_SETLK, &lock);
        if (-1 == fcntl_ret_val)
        {
            /* Ako je neki drugi proces zakljucao deo fajla,
            onda samo predji na sledeci string u fajlu, inace
            je doslo do greske prilikom poziva fcntl.
            * man fcntl, u delu sa F_SETLK */
            check_error(EACCES == errno || EAGAIN == errno, "fcntl");
            num_of_locked++;
        }
        else
        {
            num_of_unlocked++;

            check_error(-1 != fseek(f, ftell(f) - word_len, SEEK_SET), "fseek");
            invert(word, word_len);
            fprintf(f, "%s", word);
        }
    }


    // otkljucavamo sve zakljucane delove (ako se ovaj deo zaboravi, close ce otklucati)
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // ukoliko je postavljeno na nulu cita do EOF-a

    check_error(-1 != fcntl(fd, F_SETLK, &lock), "fcntl");
    // fclose ce zatvoriti i fd pa nema potrebe da zovemo close()
    check_error(-1 != fclose(f), "fclose");

    printf("%u %u\n", num_of_unlocked, num_of_locked);

    return 0;
}

void invert(char *s, size_t len)
{
    for (int i = 0, j = len - 1; i < j; i++, j--)
    {
        char tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
}
