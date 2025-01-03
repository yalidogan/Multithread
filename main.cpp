#include <iostream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <fstream>
#include <string>
#include <cstring>
#include <signal.h>

using namespace std;

#define PORT 8080 //Define the port to be used as 8080

pthread_t thread_tcp [100], thread_file[10];
int threadnoTCP = 0;
int serverFd;


//Message struct that will be sent from the client
struct message{
    char file[256];
};

//Struct to hold the necessary parameters to pass into the threads
struct request{
    int des;
    socklen_t addlen;
    sockaddr_in clientAddr;
};


void *tcpConnection(void*);

//Signal handler to terminate the infinite loop with a ctrl + c which closes the socket
void signalHandler(int sigNo){
    if(sigNo == SIGINT){
        cout << "Exiting the process" <<endl;
        close(serverFd);
        exit(1); //Exit with the exit code on to properly handle the termination
    }
}





int main() {

    //Server and client address
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;


    socklen_t addrLen = sizeof(clientAddr); //The length of the clients address

    //First create the socket
    serverFd = socket(AF_INET, SOCK_STREAM, 0);

    //Terminate the program if the socket is not created
    if(serverFd == -1){
        cout << "Failed to create socket, terminating the program" << endl;
        return 0;
    }

    cout <<"Successfully created a socket with fd: " << serverFd << endl;

    //Set the properties of the socket
    //bzero((sockaddr *) &serverAddr, sizeof(serverAddr));
    //either use this or memset really does not matter
    memset((sockaddr*) &serverAddr, 0, sizeof(serverAddr)); //Set to 0
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Give the address of the local
    serverAddr.sin_port = htons(PORT); //Assign a port from the computer for the server


    //Now bind the socket to a port on the computer
    int bindID = bind(serverFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
    if(bindID != 0){
        //Bind was unsuccessful
        cout << "Could not bind the socket to a port, terminating" << endl;
        return 0;
    }


    cout << "Successfully binded the socket to a port on the computer" << endl;

    //Now activate the port and set it to listen
    int listenID = listen(serverFd, 5);
    if(listenID != 0){
        cout << "Server could not be set to listen" << endl;
        return 0;
    }

    //To handle the according signals with the protocols
    signal(SIGINT, signalHandler);
    signal(SIGTSTP, signalHandler);

    //Create an infinite loop to constantly accept messages
    //Accept from multiple clients at once with multithreading

    while(1){
        //Accept the connection from the client first
        int connectionFd;
        connectionFd = accept(serverFd, (sockaddr*) &clientAddr, &addrLen);
        if(connectionFd < 0){
            //Error on accepting
            cout << "Could not accept the message from the client" << endl;
            return 0;
        }

         cout << "Accepted a message from a client " << endl;
        //Set up the request object for the transmission

        request* r = new request; //Create a request object
        bzero(r, sizeof(request)); //Set the request to 0 for each request for protection
        r->addlen = addrLen;
        r->clientAddr = clientAddr;
        r->des = connectionFd;

        pthread_create(&thread_tcp[threadnoTCP++], NULL, tcpConnection, (void*) r); //Set up the new thread and pass the tcpconn and its arguments
        if(threadnoTCP == 100){
            threadnoTCP = 0;
        }


    }

    return 0;
}

void* tcpConnection(void* arg){
    //TODO CHECK THIS - works
    request sock = *((request*)arg);
    message buff;

    while(1){
        //Read the buffer sent by the connected and accepted client
        read(sock.des, &buff, sizeof(buff));
        cout << "\n\n" << buff.file << endl;
    }
}
