#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#define DAY_TO_SEC (60 * 60 * 24)

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
    

// [1]
bool osIsPublicFile(const char *fpath);
// [2]
void osMkPublicDir(const char *dname);
// [3]
unsigned osNumOfDaysFileModified(const char *fpath);
// [4]
void osMoveFile(const char *srcpath, const char *destpath);
// [5]
void osRecursiveSearch(const char *currpath, const char *destpath);

int main(int argc, char **argv)
{
    check_error(argc == 3, "./5 dirpath backuppath");

    osMkPublicDir(argv[2]);

    char *dest_dirpath = realpath(argv[2], NULL);
    osRecursiveSearch(argv[1], dest_dirpath);
    free(dest_dirpath);

    return 0;
}

void osRecursiveSearch(const char *currpath, const char *destpath)
{
    // printf("CURR : %s\n", currpath);
    struct stat file_info;
    check_error(stat(currpath, &file_info) != -1, "stat");

    /* ukoliko trenutni fajl nije direktorijum, ispitujemo poslednje  */
    if (!S_ISDIR(file_info.st_mode))
    {
        /* u zadatku se trazi da se obradjuju samo regularni fajlovi koji imaju rw 
        prava pristupa za others, ukoliko to nije ispunjeno ne obradjujemo dati fajl */
        if (!S_ISREG(file_info.st_mode) || !osIsPublicFile(currpath))
        {
            return;
        }
        
        if (osNumOfDaysFileModified(currpath) <= 30)
        {
            char *real_currpath = realpath(currpath, NULL);
            int n = strrchr(real_currpath, '/') - real_currpath;
            
            char *file_destpath = malloc((strlen(destpath) + strlen(real_currpath + n) + 1) * sizeof(char));
            check_error(file_destpath != NULL, "malloc");

            strcpy(file_destpath, destpath);
            // ukljucuje '/'
            strcat(file_destpath, real_currpath+n);

            osMoveFile(real_currpath, file_destpath);
            free(real_currpath);
            free(file_destpath);
        }
        else
        {
            check_error(unlink(currpath) != -1, "unlink");
        }

        // izlazak iz rekurzije
        return;
    }
    
    // citamo iz direktorijuma
    DIR *dir = opendir(currpath);
    check_error(dir != NULL, "opendir");

    // menjamo working directory na trenutni direktorijum
    check_error(chdir(currpath) != -1, "chdir");

    struct dirent *dir_entry = NULL;
    errno = 0;
    
    while ((dir_entry = readdir(dir)) != NULL)
    {
        // preskacemo . i .. da bismo izbegli beskonacnu rekurziju
        
        // ukoliko nismo naisli na . ili .. rekurzivno pozivamo funkciju i obradjujemo trenutni fajl
        if (strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, ".."))
        {
            osRecursiveSearch(dir_entry->d_name, destpath);
            errno = 0;
        }
    }

    // proveravamo da li je bilo gresaka prilikom poziva fje readdir
    check_error(errno != EBADF, "readdir");

    // vracamo se u prethodni direktorijum (izlazimo iz onog koji smo upravo obradili)
    check_error(chdir("..") != -1, "chdir");
    
    // zatvaramo stream
    check_error(closedir(dir) != -1, "closedir");
}

bool osIsPublicFile(const char *fpath)
{
    struct stat statbuf;
    check_error(lstat(fpath, &statbuf) != -1, "lstat");

    check_error(S_ISREG(statbuf.st_mode), "not a regular file");

    return ((statbuf.st_mode & S_IROTH) && (statbuf.st_mode & S_IWOTH)); 
}

void osMkPublicDir(const char *dname)
{
    mode_t old_umask = umask(0);
    check_error(mkdir(dname, 0777) != -1, "mkdir");
    umask(old_umask);
}

unsigned osNumOfDaysFileModified(const char *fpath)
{
    struct stat file_info;
    check_error(stat(fpath, &file_info) != -1, "stat");

    time_t now = time(NULL);
    unsigned time_since_last_mod = now - file_info.st_mtime;

    return time_since_last_mod / DAY_TO_SEC;
}

void osMoveFile(const char *srcpath, const char *destpath)
{
    // srcpath i destpath treba da budu razliciti
    // man 3 realpath
    char *src_p = realpath(srcpath, NULL);
    char *dest_p = realpath(destpath, NULL);

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