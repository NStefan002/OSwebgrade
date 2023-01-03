#define _XOOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
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

bool is_4_digit_number(char *s);

int main(int argc, char **argv)
{
    check_error(2 == argc, "./2 filepath");

    // fopen ce proveriti da li fajl postoji
    FILE *f = fopen(argv[1], "r+"); // treba nam i za citanje i za pisanje
    check_error(NULL != f, "fopen");

    struct flock lock;
    char str[MAX_LEN];
    while (EOF != fscanf(f, "%s", str))
    {
        if (is_4_digit_number(str))
        {
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = ftell(f) - 4;
            lock.l_len = 4; 

            errno = 0;
            int fcntl_return_val = fcntl(fileno(f), F_SETLK, &lock);
            // man 2 fcntl deo sa F_SETLK i 3. sadatak iz Jun2 2017
            /* fcntl u ovoj situaciji puca samo kada je write lock vec postojao,
            a ako je read lock postojao onda samo l_type polje menja na F_RDLCK */
            if (-1 == fcntl_return_val || F_RDLCK == lock.l_type)
            {
                /* fcntl postavlja errno na neki od navedena dva makroa samo ukoliko
                je vec postojao write lock, inace je doslo do neke druge greske */
                check_error(F_RDLCK == lock.l_type || EACCES == errno || EAGAIN == errno, "fcntl F_SETLK");
            }
            else
            {
                check_error(-1 != fseek(f, ftell(f) - 4, SEEK_SET), "fseek");
                fprintf(f, "####");
            }
        }
    }
    
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    check_error(-1 != fcntl(fileno(f), F_SETLK, &lock), "fcntl F_SETLK unlock");
    check_error(-1 != fclose(f), "fclose");
    return 0;
}

bool is_4_digit_number(char *s)
{
    int i;
    for (i = 0; s[i]; i++)
    {
        if (!isdigit(s[i]) || i >= 4)
        {
            return false;
        }
    }
    
    return i == 4;
}