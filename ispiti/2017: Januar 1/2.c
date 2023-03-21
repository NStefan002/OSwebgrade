#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
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


#define RD_END (0)
#define WR_END (1)
#define MAX (256)

int main(int argc, char **argv)
{
    check_error(2 == argc, "./2 file_path");

    int pipefds[2];
    check_error(-1 != pipe(pipefds), "pipe");

    pid_t child_pid = fork();
    check_error((pid_t)-1 != child_pid, "fork");

    if (child_pid == 0) // child
    {
        /* Preusmeravamo stdout na write end pipe-a od deteta, da bi
        komanda terminala stat pisala u write end.  */
        check_error(-1 != close(pipefds[RD_END]), "close");
        check_error(-1 != dup2(pipefds[WR_END], STDOUT_FILENO), "dup2");
        check_error(-1 != execlp("stat", "stat", argv[1], NULL), "execlp");
    }

    // parent

    /* Preusmeravamo stdin na read end pipe-a od roditelja, da bi roditelj
    mogao da formatirano cita koriscenjem scanf-a (drugo resenje je koriscenje
    funkcija fdopen i fscanf). */
    check_error(-1 != close(pipefds[WR_END]), "close");
    check_error(-1 != dup2(pipefds[RD_END], STDIN_FILENO), "dup2");

    char str[MAX] = "";
    while (strcmp(str, "Size:"))
    {
        /* Ukoliko scanf vraca EOF (-1), to znaci da je u detetu doslo
        do neke greske koja ne vraca exit status 1 (npr. ukoliko prosledimo
        nepostojeci fajl). */
        if (scanf("%s", str) == EOF)
        {
            printf("Neuspeh\n");
            check_error(-1 != close(pipefds[RD_END]), "close");
            exit(EXIT_SUCCESS);
        }
    }

    /* Kada smo naisli na string "Size:", iza njega se nalazi podatak koji
    treba da ispisemo. */
    size_t size;
    scanf("%lu", &size);
    printf("%lu\n", size);

    check_error(-1 != close(pipefds[RD_END]), "close");

    // cekamo dete da ne bismo pravili zombije
    int status = 0;
    check_error((pid_t)-1 != wait(&status), "wait");

    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
    {
        printf("Neuspeh\n");
    }

    return 0;
}
