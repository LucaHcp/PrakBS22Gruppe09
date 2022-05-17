#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>
#define SERVER_PORT 5678
#define SERVER_BUFF_SIZE 1024

int serverMainX (){

    char serverMessage[SERVER_BUFF_SIZE] = "You have reached the Server!";

    char in[SERVER_BUFF_SIZE];
    int input;

    int clientSocket;

    socklen_t client_len;


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
    //clientSocket = accept(serverSocket,NULL,NULL);

    //send the message
    send(clientSocket , serverMessage , sizeof(serverMessage),0);

    // Socket lauschen lassen
    int lrt = listen(serverSocket, 5);
    if (lrt < 0 ){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    while (1) {

        // Verbindung eines Clients wird entgegengenommen
        clientSocket = accept(serverSocket, (struct sockaddr *) &serverAddress, &client_len);

        // Lesen von Daten, die der Client schickt
        input = read(clientSocket, in, SERVER_BUFF_SIZE);

        // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
        while (input >= 0) {
            write(clientSocket, in, input);
            input = read(clientSocket, in, SERVER_BUFF_SIZE);

        }
        close(clientSocket);
    }

    // Close the socket
    close(serverSocket);

    return 0;

}