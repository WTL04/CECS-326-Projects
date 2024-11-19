### System Calls

- fork : 
    - creates new process called child
    - child is copy of parent process
    - in child, fork() returns 0
    - in parent, fork() returns i>0
    - error, fork() returns i<0
- exec : 
    - replace current process with new program
    - used to run child process
- wait : 
    - used by parent to wait for child process to terminate

### Interprocess communication (IPC)

- message queue 
    - msgget
        - used to first aquire the message queue from operating system

    - msgctl
        - used for control operations on exisitng message queue ex. remove

    - msgsnd and msgrcv
        - allows processes with permissions to send and/or recieve messages via message queue

    - gitpid
        - process obtains its own PID


### The Assignment
    - create 3 programs
        - master.cpp 
            - identifies itself as first output
            - get message queue
            - create one child to execute sender
            - create a num of child to execute receiver
                - num = num of recievers passed from commandline arg

            - outputs: its PID, message queue ID, info about each child process created.
            - waits for child processes to finish, removes message queue and quits

        - sender.cpp
            - obtain message queue id and num of recievers in exec() call

        - receiver.cpp
            - receive message queue id and its receiver number (1, 2, ..., etc based on order of creation) in exec() call
        
    - Master should create both sender and reciever

    - sender can only send messages to reciever 

    - reciever only recieves messages from sender
