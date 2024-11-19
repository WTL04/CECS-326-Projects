#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "myShm.h"

// POSIX API and Memory Mapping API
#include <fcntl.h>     // For O_CREAT, O_RDWR
#include <sys/mman.h>  // For mmap, PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED

// POSIX implemetation of semaphore
#include <semaphore.h>

int main(int argc, char *argv[]) 
{
    int child_number = atoi(argv[1]);
    char *shm_name = argv[2];
    int shm_id;
    const int SIZE = 4096;
    char *shm_base; 

    printf("Slave begins execution\n");
    printf("I am child number %d, received shared memory name %s.\n", child_number, shm_name);

    /* open exisitng shared memory segment */
    shm_id = shm_open(shm_name, O_RDWR, 0666);
    if (shm_id < 0) 
    {
        perror("shm_open failed");
        exit(1);
    }
    
    /* attach pointer to shared memory segment */
    shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if (shm_base == MAP_FAILED)
    { 
        perror("mmap failed");
        exit(1);
    }

    /* structure shared memory segment to struct CLASS */
    /* program can now access members of the struct CLASS using count_ptr */
    struct CLASS *shm_ptr = (struct CLASS *) shm_base;

    sem_wait(&shm_ptr->mutex_sem);
    int y = shm_ptr->index;
    shm_ptr->report[y] = child_number;
    shm_ptr->index++;

    printf("I have written my child number in slot %d and updated index to %d\n", y, y+1);
    sem_post(&shm_ptr->mutex_sem);
    printf("Child %d closed access to shared memory and terminates\n", child_number);

    /* detach and remove shared memory */
    if (munmap(shm_base, SIZE) == -1) 
    {
        perror("munmap failed");
    }
    

    return 0;
}
