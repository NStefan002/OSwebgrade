#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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


#define thread_check_error(thread_err, msg)\
   do\
   {\
       int _thread_err = thread_err;\
       if (_thread_err > 0)\
       {\
           errno = _thread_err;\
           check_error(false, msg);\
       }\
   } while (0)\


int main(int argc, char **argv)
{




    return 0;
}
