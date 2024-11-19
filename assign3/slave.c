#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "myShm.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <child_number> <shared_memory_name>\n", argv[0]);
        exit(1);
    }

    int child_number = atoi(argv[1]);
    char *shm_name = argv[2];
    int shm_id;
    struct CLASS *shm_ptr;

    printf("Slave begins execution\n");
    printf("I am child number %d, received shared memory name %s.\n", child_number, shm_name);

    // Open existing shared memory segment
    shm_id = shmget(ftok(shm_name, 'R'), sizeof(struct CLASS), 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }

    // Attach to the shared memory segment
    shm_ptr = (struct CLASS *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (struct CLASS *)(-1)) {
        perror("shmat failed");
        exit(1);
    }

    // Write the child number into the shared memory
    int index = shm_ptr->index;
    if (index < 10) {
        shm_ptr->report[index] = child_number;
        shm_ptr->index++;
        printf("I have written my child number to slot %d and updated index to %d.\n", index, shm_ptr->index);
    }

    // Close the shared memory segment
    shmdt(shm_ptr);
    printf("Child %d closed access to shared memory and terminates.\n", child_number);

    return 0;
}
