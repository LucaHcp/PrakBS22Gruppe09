#include <stdio.h>
#include <stdlib.h>
#include "LinkedListHeader.h"
#include "Client.h"
#include "Server.h"


int main(){

    //serverMain();
    //clientMain();

    char serverMessage[SERVER_BUFF_SIZE] = "You have reached the Server!";

    // create the server socket
    int serverSocket;
    serverSocket = socket(AF_INET,SOCK_STREAM,0);

    //Define the server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //Bind the socket to our specified IP and port
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    listen(serverSocket,5);

    //Create Integer hold the client socket
    int clientSocket;
    clientSocket = accept(serverSocket,NULL,NULL);

    printf("TEST1");

    //send the message
    send(clientSocket , serverMessage , sizeof(serverMessage),0);

    printf("TEST2");

    char quit[4] = "quit";

    while(1) {
        printf("TEST3");
        char input[SERVER_BUFF_SIZE];
        read(serverSocket,&input,SERVER_BUFF_SIZE);

    }

    // Close the socket
    close(serverSocket);

    return 0;

    return 0;
}
