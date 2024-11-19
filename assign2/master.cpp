#include <cstddef>
#include <iostream>
#include <unistd.h>  // For getpid()
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string>

using namespace std;


// example : ./master 3
int main(int argc, char *argv[])
{
    // convert argv[1] into int
    int x = stoi(argv[1]);

    // get master PID
    pid_t masterPID = getpid();
    cout << "Master, PID " << masterPID << " begins execution" << endl;

    // get message queue ID
    key_t key = ftok("queuefile", 65); // generate unique key
    int msgid = msgget(key, 0666 | IPC_CREAT); // create msg queue with read/write perms

    if (msgid == -1) 
    {
        perror("msgget error");
        return 1;
    }

    cout << "Master acuired a message queue, id " << msgid << endl;

    // creating sender child PID
    pid_t senderPID = fork();

    if (senderPID == 0) 
    {
        // execute child using command line "./sender"
        execl("./sender", "sender", to_string(msgid).c_str(), to_string(x).c_str(), NULL);
    }
    else if (senderPID > 0)
    {
        // create child process sender
        cout << "Master created a child process to serve as sender, sender's PID " << senderPID << endl;
    }
    else 
    {
        // check errors
        perror("fork sender");
        return 1;
    }


    for (int i = 1; i<=x; ++i)
    {
        pid_t receiverPID = fork();

        if (receiverPID == 0) 
        {
            // execute child using command line "./receiver"
            execl("./receiver", "receiver", to_string(msgid).c_str(), to_string(i).c_str(), NULL);
        }
        else if (senderPID > 0)
        {
            // create child process receiver
            cout << "Master created a child processes to serve as receiver " << i << endl;
        }
        else 
        {
            // check errors
            perror("fork receiver");
            return 1;
        }
    }

    cout << "Master waits for all child processes to terminate" << endl;
    for (int i = 1; i <=x; ++i)
    {
        // wait for child processes to terminate
        wait(NULL);
    }

    cout << "Master received termination signals from all child processes, removed message queue, and terminates" << endl;
    msgctl(msgid, IPC_RMID, NULL);
    
    return 0;
}