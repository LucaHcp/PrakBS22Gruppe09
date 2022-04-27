//
// Created by User on 27.04.2022.
//

#ifndef PRAKTIKUM_CLIENT_H
#define PRAKTIKUM_CLIENT_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>
#define SERVER_PORT 5678
#define SERVER_BUFF_SIZE 1024

int clientMain () {

    //erstellen eines Sockets
    int networkSocket;
    networkSocket = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //return integer um zu wissen ob die Verbindung funktioniert oder nicht (value 0 = Ok) (-1 = nicht ok)
    int connectStatus = connect(networkSocket,(struct sockaddr *) &serverAddress, sizeof(serverAddress));
    // check für error
    if (connectStatus == -1){

        printf("Verbindung funktioniert nicht\n\n");
    }
    //receiving data
    char serverResponse [SERVER_BUFF_SIZE];
    recv(networkSocket, &serverResponse , sizeof(serverResponse), 0);
    //print data
    printf("Der Server sendet die Datei: %s \n", serverResponse);

    // close the socket
    close(networkSocket);

    return 0;




}
#endif //PRAKTIKUM_CLIENT_H
