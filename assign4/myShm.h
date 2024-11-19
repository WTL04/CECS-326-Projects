/* myShm.h */
/* Header file to be used with master.c and slave.c
*/
#include <semaphore.h>

struct CLASS {
    int index; /* index to next available response slot */
    int report[10]; /* each child writes its child number here*/
    sem_t mutex_sem; /* semaphore, enforce mutal exclusion */ 
};

