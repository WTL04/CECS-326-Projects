#include <iostream>
#include <unistd.h>  // For getpid()
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>

using namespace std;

struct msg_buffer
{
    long mtype; // setting message type for receiver to filter which to receive
    char message_content[100]; // buffer for message text
};


// ./sender <msgid> <numReceivers>
int main(int argc, char *argv[])
{
    // get num of receivers
    int x = atoi(argv[2]); 

    pid_t senderPID = getpid();
    cout << "Sender, PID " << senderPID << " , begins execution" << endl;

    // get message queue id
    int msgid = atoi(argv[1]);
    cout << "Sender, PID " << senderPID << " , received message queue id " << msgid << " through commandline argument" << endl;

    // get user input message and which receiver the message is for
    
    for (int i = 1; i<=x; ++i)
    {
        string userMsg;
        int receiverChoice;
    
        cout << "Sender, PID " << senderPID << ": Please input your message" << endl;
        getline(cin, userMsg);

        cout << "Sender, PID " << senderPID << ", which receiver is this message for?" << endl;
        cin >> receiverChoice;
        cin.ignore();

        msg_buffer message;
        message.mtype = receiverChoice;

        // copying userMsg cstring into message_content array
        strcpy(message.message_content, userMsg.c_str());

        // msgsnd(msg queue id, pointer to buffer, size of message, behavior)
        if (msgsnd(msgid, &message, sizeof(message.message_content), 0) == -1) 
        {
            perror("msgsnd failed");
            return 1;
        }

    }


    // receiving acknowledgements from receivers
    for (int i = 1; i <= x; ++i)
    {
        msg_buffer acknowledgement;

        ssize_t received = msgrcv(msgid, &acknowledgement, sizeof(acknowledgement.message_content), 999, 0);
        if (received==-1) 
        {
            perror("msgrcv failed");
            return 1;
        }
        cout << "Sender, PID " << senderPID << ", receives message: " << acknowledgement.message_content << endl;
    }




    return 0;
}