#ifndef _HELPER_H_
#define _HELPER_H_

#include <stdio.h>

#define debug_here(a)  \
    do { \
        printf("%d: at %s:%d\n", getpid(), __FILE__, __LINE__); \
        fflush(stdout); \
    } while(0)


#define exit_msg(a, b) \
    do { \
        if (a) { \
            perror(b); \
            exit(-1); \
        } \
    } while(0)

#endif 
