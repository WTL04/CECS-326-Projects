#include <iostream>
#include <string>
#include <sys/stat.h> // for file permission constants
#include <unistd.h> // for ftruncate
#include <cstring> // for memcpy()
#include <cstdlib> // for exit()
#include <sys/wait.h> // for wait()
#include "myShm.h"

// POSIX API and Memory Mapping API
#include <fcntl.h>
#include <sys/mman.h> 

// POSIX implementation of semaphore
#include <semaphore.h>

using namespace std;

int main(int argc, char *argv[])
{
  int n = atoi(argv[1]);
  string shm_name = argv[2];
  int shm_id;
  char* shm_base;

  // size of shared memory
  const int SIZE = 4096;

  // create/open the shared memory segment
  shm_id = shm_open(shm_name.c_str(), O_CREAT | O_RDWR, 0666);
  cout<<"Master begins execution"<<endl;

  // configure size of shared memory segment
  if (ftruncate(shm_id, SIZE) == -1)
  {
		perror("ftruncate failed");
    exit(1);
  }

  // attach/map shared memory segment
  shm_base = (char * )mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
  if (shm_base == MAP_FAILED)
  {
		perror("mmap failed");
    exit(1);
  }

  // pointer to CLASS struc
  CLASS* shm_ptr = reinterpret_cast<CLASS*>(shm_base);

  cout<<"Master created a shared memory segment named "<<shm_name<<endl;
  cout<<"Master initializes index in the shared structure to zero "<<endl;
  cout<<"Initial content of shared memory segment before access by child processes: "<<endl;

  // initialize reports to be 0
  shm_ptr->index = 0;

  for (int i = 0; i < n; i++)
  {
    shm_ptr->report[i]=0;
    cout<<shm_ptr->report[i]<<" ";
  }
  cout<<endl;

	// initialize unnamed semaphore
	sem_init(&shm_ptr->mutex_sem, 1, 1);

	// initialize named semaphore
  const char* semName = "io_semaphore";
	sem_t *io_sem = sem_open(semName, O_CREAT, 0660, 1);
	if (io_sem == SEM_FAILED)
  {
    perror("display_sem failed");
    exit(1);
  }

  // creating and executing chld processes
  cout<<"Master created "<<n<<" child processes to execute slave"<<endl;

	for (int i = 1; i <= n; i++)
	{
		pid_t pid = fork();
		if (pid == 0) 
		{
			// execute child process 
			execl("./slave", "slave", to_string(i).c_str(), shm_name.c_str(), NULL);
			perror("execl failed");
			exit(1);
		}
	}

	for (int i = 1; i <= n; i++) 
	{
		wait(NULL);
	}

	// Printing termination signal message
	cout << "Master received termination signals from all " << n << " child processes" << endl;

	/* printing updated content */
	cout << "Updated content of shared memory segment after access by child processes:" << endl;

	for (int i = 0; i < shm_ptr->index; i++) {
		cout << shm_ptr->report[i] << " ";
	}
	cout << endl;

	// close and unlink unnamed semaphore
	sem_destroy(&shm_ptr->mutex_sem);

  // close and unlink named semaphore
  sem_close(io_sem);
  sem_unlink(semName);

	/* Detach and remove the shared memory segment */
	if (munmap(shm_base, SIZE) == -1) {
    perror("munmap failed");
	}

	shm_unlink(shm_name.c_str()); // Remove shared memory segment
	cout << "Master removed shared memory segment and is exiting" << endl;

  return 0;
}
