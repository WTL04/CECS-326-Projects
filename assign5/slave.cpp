#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>   
#include <sys/mman.h> 
#include <semaphore.h>
#include "myShm.h"    

// I/O file
#include <fstream>

using namespace std; 

int main(int argc, char *argv[]) 
{
  int n = stoi(argv[1]);
  string shm_name = argv[2];
  int shm_id;
	const int SIZE = 4096;
	char *shm_base;

  // open named semaphore
  const char* semName = "io_semaphore";
  sem_t* io_sem = sem_open(semName, 0);
  if (io_sem == SEM_FAILED)
  {
    perror("sem_open failed");
    exit(1);
  }


	// open existing shared memory segment
	shm_id = shm_open(shm_name.c_str(), O_RDWR, 0666);
  if (shm_id < 0)
  {
    perror("shm_open failed");
    exit(1);
  }

	// attach pointer to shared memory segment
	shm_base = static_cast<char*>(mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0));
  if (shm_base == MAP_FAILED) 
  {
    perror("mmap failed");
    exit(1);
  }

	// access shared memory segment
	CLASS* shm_ptr = reinterpret_cast<CLASS*>(shm_base);

  // unnamed semaphore
	sem_wait(&shm_ptr->mutex_sem);

  int y = shm_ptr->index;
  shm_ptr->report[y] = n;
  shm_ptr->index++;

	sem_post(&shm_ptr->mutex_sem);

  // create output file, write into file, send to file stream buffer
  string fileName = "out-"+ to_string(n) +".txt";
  ofstream myOutfile(fileName);

  if (myOutfile.is_open())
  {
    myOutfile << "Slave begins execution" << endl;
    myOutfile << "I am child number " << n << ", received shared memory name " << shm_name << "." << endl;
    myOutfile << "I have written my child number in slot " << y << " and updated index to " << y + 1 << endl;
    myOutfile << "Child " << n << " closed access to shared memory and terminates" << endl;
    myOutfile.close();
  } else {
    perror("Error opening file");
  }

  // named semaphore, controls output
  sem_wait(io_sem);
  ifstream myInfile(fileName);

  if (myInfile.is_open())
  {
    cout << myInfile.rdbuf(); // outputs in terminal
    myInfile.close();
  }
  sem_post(io_sem);
  sem_close(io_sem);

  if (remove(fileName.c_str()) != 0) 
  {
    perror("Error deleting the file");
  }

	// Detach from shared memory
	if (munmap(shm_base, SIZE) == -1) {
		perror("munmap failed");
	}
	
	return 0;
}
