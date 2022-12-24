#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
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

int main(int argc, char **argv)
{
    check_error(4 == argc, "./3 file_path str1 str2");

    // r+ jer treba i da citamo i da pisemo u fajl
    FILE *f = fopen(argv[1], "r+");    
    check_error(NULL != f, "fopen");
    int fd = fileno(f);
    check_error(-1 != fd, "fileno");

    char str[MAX_LEN];
    struct flock lock;
    unsigned num_of_already_locked = 0;
    while (EOF != fscanf(f, "%s", str))
    {
        size_t str_len = strlen(str);
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = ftell(f) - str_len;
        lock.l_len = str_len;
        
        if (!strcmp(str, argv[2])) // ukoliko smo pronasli trazenu rec, pokusavamo da je zamenimo
        {
            /* Pokusavamo da zakljucamo deo fajla za pisanje. */
            errno = 0;
            int fcntl_return_val = fcntl(fd, F_SETLK, &lock);
            /* fcntl ce javiti gresku ako je vec postojao write lock, 
            ali ako je postojao read lock nece prijaviti gresku, vec ce
            u polje l_type vratiti F_RDLCK */
            if (-1 == fcntl_return_val || F_RDLCK == lock.l_type)
            {
                /* Ako je neki drugi proces zakljucao deo fajla,
                onda uvecaj broj vec zakljucanih i predji na sledeci string u fajl.
                * man fcntl, u delu sa F_SETLK */

                // provera da li je doslo do neke druge greske 
                check_error(EACCES == errno || EAGAIN == errno, "fcntl");

                num_of_already_locked++;
            }
            else // fcntl je uspeo da zakljuca deo fajla i sada menjamo rec 
            {
                /* fseek je isto kao lseek samo za strimove
                * man fseek */
                check_error(-1 != fseek(f, ftell(f) - str_len, SEEK_SET), "fseek");
                
                /* Vracamo kursor ispred procitane reci i 'preko' nje upisujemo novu */
                fprintf(f, "%s", argv[3]);
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

    printf("%u\n", num_of_already_locked);

    return 0;
}