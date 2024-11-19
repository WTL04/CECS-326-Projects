#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include "myShm.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number_of_children> <shared_memory_name>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    char *shm_name = argv[2];
    int shm_id;
    struct CLASS *shm_ptr;

    printf("Master begins execution\n");

    // Create a shared memory segment
    shm_id = shmget(ftok(shm_name, 'R'), sizeof(struct CLASS), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }

    // Attach the shared memory segment
    shm_ptr = (struct CLASS *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (struct CLASS *)(-1)) {
        perror("shmat failed");
        exit(1);
    }

    // Initialize the index
    shm_ptr->index = 0;

    printf("Master created a shared memory segment named %s\n", shm_name);
    printf("Master created %d child processes to execute slave\n", n);

    for (int i = 1; i <= n; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            char child_num[10];
            snprintf(child_num, sizeof(child_num), "%d", i);
            execl("./slave", "slave", child_num, shm_name, NULL);
            perror("execl failed");
            exit(1);
        }
    }

    printf("Master waits for all child processes to terminate\n");
    for (int i = 0; i < n; i++) {
        wait(NULL); // Wait for all children to terminate
    }

    printf("Master received termination signals from all %d child processes\n", n);
    printf("Updated content of shared memory segment after access by child processes:\n");
    
    for (int i = 0; i < shm_ptr->index; i++) {
        printf("%d ", shm_ptr->report[i]);
    }
    printf("\n");

    // Detach and remove the shared memory segment
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);
    printf("Master removed shared memory segment, and is exiting\n");

    return 0;
}
