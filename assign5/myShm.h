#ifndef MYSHM_H   // Include guard to prevent multiple inclusions
#define MYSHM_H

#include <semaphore.h>  // For POSIX semaphores

// Structure for shared memory
struct CLASS {
    int index;            // Index to next available response slot
    int report[10];       // Array where each child writes its child number
    sem_t mutex_sem;      // Semaphore to enforce mutual exclusion
    sem_t* io_sem;
};

#endif // MYSHM_H
