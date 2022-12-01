#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
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
    } while (0);


void osMoveFile(const char *srcpath, const char *destpath);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./4 srcpath destpath");

    osMoveFile(argv[1], argv[2]);

    return 0;
}

void osMoveFile(const char *srcpath, const char *destpath)
{
    // srcpath i destpath treba da budu razliciti
    // man 3 realpath
    char *src_p = realpath(srcpath, NULL);
    char *dest_p = realpath(destpath, NULL);

    printf("%s\n%s\n", src_p, dest_p);

    // moramo da proverimo da li su src_p i dest_p razliciti od NULL inace ce doci do Seg Fault kod strcmp
    check_error(src_p != NULL, "srcpath must be provided");
    // dest_p ce biti NULL ukoliko fajl na tom path-u ne postoji
    if (dest_p != NULL)
    {
        // strcmp treba da vrati vrednost razlicitu od 0 ako su razliciti srcpath i destpath
        check_error(strcmp(src_p, dest_p), "same file");
    }
    // u suprotnom znamo da taj fajl ne postoji

    // oslobadjamo memoriju koju je alocirala fja realpath
    free(src_p);
    free(dest_p);
    
    int fd_src;
    // mod za citanje (kao fopen sa "r")
    int flags_src = O_RDONLY;
    check_error((fd_src = open(srcpath, flags_src)) != -1, "open1");

    // citamo permissions za src (da bi dest imao iste)
    struct stat file_info;
    // man 2 fstat
    check_error(fstat(fd_src, &file_info) != -1, "fstat");
    
    mode_t mode = file_info.st_mode;
    
    int fd_dest;
    // flegovi za citanje, pravljenje fajla ukoliko ne postoji i brisanje sadrzaja fajla
    int flags_dest = O_WRONLY | O_CREAT | O_TRUNC;

    // postavljamo umask na 0 da bismo mogli da postavimo zeljeni mod
    mode_t old_mask = umask(0);
    // ukoliko imamo fleg O_CREAT moramo navesti koje permisije ce imati fajl (npr. -rwxrwxrwx), inace ce
    // biti postavljene na default vrednosti (-rw-r--r--)
    check_error((fd_dest = open(destpath, flags_dest, mode)) != -1, "open2");
    // vracamo umask na staru vrednosts
    umask(old_mask);
    
    size_t buffer_size = 1u << 13; // 8KB 
    char *mem_buffer = (char *)malloc(buffer_size * sizeof(char));
    check_error(mem_buffer != NULL, "malloc");
    
    ssize_t bytes_read;
    // citamo iz src i upisujemo u dest
    while ((bytes_read = read(fd_src, mem_buffer, buffer_size)) > 0)
    {
        check_error(write(fd_dest, mem_buffer, bytes_read) == bytes_read, "write");
    }

    // oslobadjanje memorije i zatvaranje file descriptora
    free(mem_buffer);
    close(fd_src);
    close(fd_dest);

    // ukoliko je poslednji poziv read-a vratio -1 to znaci da je doslo do greske
    check_error(bytes_read == 0, "read");

    // brisemo src fajl
    check_error(unlink(srcpath) != -1, "unlink");
}