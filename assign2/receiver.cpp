#include <iostream>
#include <unistd.h>  // For getpid()
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>

using namespace std;

struct msg_buffer {
    long mtype;              // Message type
    char message_content[100]; // Message content
};

// ./receiver <msgid> <receiver number>
int main(int argc, char *argv[])
{
    int y = atoi(argv[2]);
    pid_t receiverPID = getpid();
    
    // get message queue id
    int msgid = atoi(argv[1]);

    cout << "Receiver " << y << ", PID " << receiverPID << ",begins execution" << endl;
    cout << "Receiver " << y << ", PID " << receiverPID << ", received message queue id " << msgid << " through commandline argument" << endl;

    // receiving message from message queue using msgrcv
    msg_buffer message;
    ssize_t received = msgrcv(msgid, &message, sizeof(message.message_content), y, 0);
    if (received==-1) 
    {
        perror("msgrcv failed");
        return 1;
    }

    cout << "Receiver " << y << ", PID " << receiverPID << ", retrieved the following message from the message queue" << endl;
    cout << message.message_content << endl;

    // sending acknowledgement message to msg queue
    cout << "Receiver " << y << ", PID " << receiverPID << ", sent acknowledgement message into message queue" << endl;
    msg_buffer acknowledgement;
    acknowledgement.mtype = 999;
    string acknowledgementMsg = "Receiver " + to_string(y) + " acknowledges receipt of message.";
    
    // copying userMsg cstring into message_content array
    strcpy(acknowledgement.message_content, acknowledgementMsg.c_str());

    if (msgsnd(msgid, &acknowledgement, sizeof(acknowledgement.message_content), 0) == -1) 
    {
        perror("msgsnd failed");
        return 1;
    }

    cout << "Receiver " << y << ", PID " << receiverPID << ", terminates" << endl;
    return 0;
}
