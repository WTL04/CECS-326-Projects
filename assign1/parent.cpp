#include <iostream>
#include <unistd.h>  // For fork() and execvp()
#include <sys/wait.h> // For wait()
#include <vector>
#include <cstdlib>    // For exit()

using namespace std;

int main (int argc, char *argv[])
{
    vector<pid_t> childPIDs;

    for (int i = 1; i < argc; i+=2)
    {

        // fork() : Clone the calling process, creating an exact copy.
        // Return -1 for errors, 0 to the new process, and the process ID of the new process to the old process.
        pid_t pid = fork();
        if (pid<0)
        {
            cerr<<"fork failed"<<endl;
            return 1;
        }

        if (pid==0)
        {
            char* args[] = {(char*) "./child", argv[i], argv[i+1], NULL};
            execvp(args[0], args); // replacing current process with child executable aka ./child
        }
        else {
            // Parent process: Store the child PID and continue
            childPIDs.push_back(pid);
        }
    }

    for (pid_t pid : childPIDs)
    {
        int status;
        waitpid(pid, &status, 0); // waits for each child function to finish running
    }

    int pairs = (argc-1)/2; //num of name age pairs in argv
    cout<<"I have "<< pairs << " children."<< endl;
    cout<<"All child processes terminated. Parent exits."<<endl;
    return 0;
}