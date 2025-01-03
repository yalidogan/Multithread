#include <iostream>
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fstream>

using namespace std;
#define PORT 8080 //Define the port to be used as 8080

int sockfd;

struct message{
    char file[256];
};

void signalHandler(int sigNo){
    if(sigNo == SIGINT){
        cout << "Exiting the process" <<endl;
        close(sockfd);
        exit(1); //Exit with the exit code on to properly handle the termination
    }
};




int main() {

    sockaddr_in servAddr, cliAddr;

    //Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        cout << "Could not create the socket" << endl;
        return 0;
    }

    else{
        cout << "Successfully created the socket " << endl;
    }

    bzero(&servAddr, sizeof(servAddr));

    //Assign the parameters to the socket
    //Port and the IP
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
    servAddr.sin_port = htons(PORT);

    int connectID = connect(sockfd, (sockaddr *) &servAddr, sizeof(servAddr));
    if(connectID != 0){
        //Connection successfully created
        cout << "Connection to the server has failed " << endl;
        exit(0);
    }
    else{
        cout << "Connected to the server " << endl;
    }

    //Create a buffer for the message
    message buffer;

    signal(SIGINT, signalHandler);
    signal(SIGTSTP, signalHandler);

    //Set up the infinite loop for the message handling
    while(1){

        memset(buffer.file, '\0', sizeof(buffer.file));

        string message = "TEST MESSAGE";
        strcpy(buffer.file, message.c_str());

        write(sockfd, &buffer, sizeof(buffer));
        cout << "Message sent to the server" << endl;
        cout << "Sent message: " << buffer.file << endl;

    }

    return 0;
}
