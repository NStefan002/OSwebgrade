#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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


void reverseString(char *str);
void reverse(const char *fpath, off_t p, size_t n);

int main(int argc, char **argv)
{
    check_error(argc == 4, "./3 file_path p n");

    reverse(argv[1], strtol(argv[2], NULL, 10), strtol(argv[3], NULL, 10));

    return 0;
}

void reverse(const char *fpath, off_t p, size_t n)
{
    // ispitujemo da li fajl postoji i da li je regularan
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");
    check_error(S_ISREG(file_info.st_mode), "not a reg file");

    // otvaramo fajl za citanje i pisanje
    int fd;
    check_error((fd = open(fpath, O_RDWR)) != -1, "open");

    // pomeramo ofset za p mesta i pokusavamo da procitamo n karaktera
    check_error(lseek(fd, p, SEEK_SET) != -1, "lseek");

    char *mem_buf = malloc((n + 1) * sizeof(char));
    check_error(mem_buf != NULL, "malloc");
    
    ssize_t bytes_read = read(fd, mem_buf, n);
    check_error(bytes_read != -1, "read");
    // dodajemo term nulu da bi strlen mogao da izracuna duzinu niske
    mem_buf[bytes_read] = '\0';

    // vracamo ofset na poziciju p, obrcemo tekst koji smo prociali i pokusavamo da ga ispisemo
    check_error(lseek(fd, p, SEEK_SET) != -1, "lseek");

    reverseString(mem_buf);
    
    check_error(write(fd, mem_buf, bytes_read) != -1, "write");

    check_error(close(fd) != -1, "close");
    free(mem_buf);
}

void reverseString(char *str)
{
    for (int i = 0, j = strlen(str) - 1; j > i; i++, j--)
    {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
}