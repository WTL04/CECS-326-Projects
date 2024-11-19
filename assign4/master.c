#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "myShm.h"

// POSIX API and Memory Mapping API
#include <fcntl.h>     // For O_CREAT, O_RDWR
#include <sys/mman.h>  // For mmap, PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED

int main(int argc, char *argv[]) 
{
    int n = atoi(argv[1]);
    char *shm_name = argv[2];
    int shm_id;
    char *shm_base; // pointer to shared memory segment
    const int SIZE = 4096;

    /* creating shared memory segment using POSIX Shared Memory */
    shm_id = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_id < 0) 
    {
        perror("shm_open failed");
        exit(1);
    }
    printf("Master begins execution\n");

    /* configure size of shared memory segment */
    if (ftruncate(shm_id, SIZE) == -1)
    {
        perror("ftruncate failed");
        exit(1);
    }

    /* attach/map shared memory segment */
    /* mapping allows process to have access to the shared memory */
    shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if (shm_base == MAP_FAILED) 
    { 
        perror("shm_base failed");
        exit(1);
    }

    /* structure shared memory segment to struct CLASS */
    /* program can now access members of the struct CLASS using count_ptr */
    struct CLASS *shm_ptr = (struct CLASS *) shm_base;
    
    /* printing initial contents of shared memory */
    printf("Master created a shared memory segment named %s \n", shm_name);
    printf("Master initializes index in the shared structure to zero \n");
    printf("Initial content of shared memory segment before access by child processes: \n");

    shm_ptr->index = 0;

    for (int i = 0; i < n; i++)
    {
        shm_ptr->report[i] = 0;
        printf("%d ", shm_ptr->report[i]);
    }
    printf("\n");

    /* initializing unnamed semaphore */
    sem_init(&shm_ptr->mutex_sem, 1, 1);

    /* creating and executing child processes */
    printf("Master created %d child processes to execute slave\n", n);
    for (int i = 1; i <= n; i++)
    {
        pid_t pid = fork();
        if (pid==0)
        {
            char child_num[10];
            snprintf(child_num, sizeof(child_num), "%d", i);
            execl("./slave", "slave", child_num, shm_name, NULL);
            perror("execl failed");
            exit(1);
        }
    }

    /* wait for child processes to terminate */
    printf("Master waits for all child processes to terminate\n");
    for (int i = 0; i < n; i++)
    {
        wait(NULL);
    }
    printf("Master received termination signals from all %d child processes\n", n);

    /* printing updated content */
    printf("Updated content of shared memory segment after access by child processes:\n");
    
    for (int i = 0; i < shm_ptr->index; i++) {
        printf("%d ", shm_ptr->report[i]);
    }
    printf("\n");

    /* destroy semaphore */
    sem_destroy(&shm_ptr->mutex_sem);

    /* Detach and remove the shared memory segment */
    munmap(shm_base, SIZE);         // Detach shared memory
    shm_unlink(shm_name);           // Remove shared memory segment
    printf("Master removed shared memory segment and is exiting\n");

    return 0;
}
