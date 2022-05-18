
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "LinkedListHeader.h"


#define BUFSIZE 1024 // Größe des Buffers
#define ENDLOSSCHLEIFE 1
#define PORT 5678

int main() {

    linkedList *myList = malloc(sizeof(struct linkedList));

    int keyHolder;
    int valueHolder;

    char * helpMessage =  (" /// HELP /// \n Key = Int | Value = Int \n quit to Quit \n list to List all Data \n put(Key,Value) to put date in \n get(Key) to return the Value of given Key \n del(Key) to delete date by key \n\n");
    char str[BUFSIZE];
    printf(" Start \n");



    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten
    char * in [BUFSIZE]; // Daten vom Client an den Server
    int bytes_read; // Anzahl der Bytes, die der Client geschickt hat


    // Socket erstellen
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    if (rfd < 0 ){
        fprintf(stderr, "socket konnte nicht erstellt werden\n");
        exit(-1);
    }


    // Socket Optionen setzen für schnelles wiederholtes Binden der Adresse
    int option = 1;
    setsockopt(rfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));


    // Socket binden
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int brt = bind(rfd, (struct sockaddr *) &server, sizeof(server));
    if (brt < 0 ){
        fprintf(stderr, "socket konnte nicht gebunden werden\n");
        exit(-1);
    }


    // Socket lauschen lassen
    int lrt = listen(rfd, 5);
    if (lrt < 0 ){
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    while (ENDLOSSCHLEIFE) {

        printf(" Warte auf Verbindung \n");
        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);


        printf(" Lesen Von Input \n");

        memset(in,'\0',BUFSIZE);

        // Lesen von Daten, die der Client schickt
        bytes_read = read(cfd, in, BUFSIZE);
        //bytes_read = recv(cfd, in, BUFSIZE, 0);


        // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
        while (bytes_read > 0) {
            //printf(" sending back the %d bytes I received...\n", bytes_read);
            // write(cfd, in, bytes_read);


            // Check Input
            // QUIT
            if (strcmp("quit",strtok(in, "\r\n")) == 0){
                printf(" Quit \n ");
                break;
            }
            // HELP
            else if (strcmp("help",strtok(in, "\r\n")) == 0){
                write(cfd, helpMessage, strlen(helpMessage));
                printf(" Help \n");
            }
            // PUT
            else if (strcmp("put",strtok(in, "\r\n")) == 0){
                //Get Key
                write(cfd, "Key : ", strlen("Key : "));
                bytes_read = read(cfd, in, BUFSIZE);

                keyHolder = atoi(in);

                //Get Value
                write(cfd, "Value : ", strlen("Value : "));
                bytes_read = read(cfd, in, BUFSIZE);

                valueHolder = atoi(in);

                printf("Put Key %i with Value : %i \n ",keyHolder,valueHolder);

                // Return Put Key Value
                sprintf(str, "%i",keyHolder);
                write(cfd, "\nPut Key : ", strlen("\n Put Key : "));
                write(cfd, str, strlen(str));

                sprintf(str, "%i",valueHolder);
                write(cfd, " | Value : ", strlen(" | Value : "));
                write(cfd, str, strlen(str));
                write(cfd, "\n \n", strlen("\n \n"));

                // Put in Linked list
                addNodeToListEnd(keyHolder,valueHolder,myList);

            }
            // GET
            else if (strcmp("get",strtok(in, "\r\n")) == 0){
                //Get Key
                write(cfd, "Key : ", strlen("Key : "));
                bytes_read = read(cfd, in, BUFSIZE);
                // Store Key
                keyHolder = atoi(in);
                // Store Value
                valueHolder = getNodeValueByKey(keyHolder,myList);
                if ( getNodeValueByKey(keyHolder,myList) != -1 ) {
                    printf("Found Key %i with Value : %i \n ",keyHolder,valueHolder);
                    // Return Value
                    write(cfd, "\n Found Key : ", strlen("\n Found Key : "));
                    sprintf(str, "%i",keyHolder);
                    write(cfd, str, strlen(str));
                    write(cfd, " | Value : ", strlen(" | Value : "));
                    sprintf(str, "%i",valueHolder);
                    write(cfd, str, strlen(str));
                    write(cfd, "\n \n", strlen("\n \n"));
                }
                else {
                    // Return Not Found
                    printf("Found No Key : %i ",keyHolder);
                    write(cfd, "\n No Key : ", strlen("\n No Key : "));
                    sprintf(str, "%i",keyHolder);
                    write(cfd, str, strlen(str));
                    write(cfd, "\n \n", strlen("\n \n"));
                }
            }
            // DELETE
            else if (strcmp("del",strtok(in, "\r\n")) == 0) {
                //Get Key
                write(cfd, "Key : ", strlen("Key : "));
                bytes_read = read(cfd, in, BUFSIZE);
                // Store Key
                keyHolder = atoi(in);
                if (getNodeByKey(keyHolder,myList) != NULL){
                    printf("Found Key %i with Value : %i \n ",keyHolder,valueHolder);
                    deleteNode(keyHolder,myList);
                    write(cfd, "\n Delete Key : ", strlen("\n Delete Key : "));
                    sprintf(str, "%i",keyHolder);
                    write(cfd, str, strlen(str));
                    write(cfd, "\n \n", strlen("\n \n"));
                }
                else{
                    printf("Found No Key %i \n ",keyHolder);
                    write(cfd, "\n Key : ", strlen("\n Key : "));
                    sprintf(str, "%i",keyHolder);
                    write(cfd, str, strlen(str));
                    write(cfd, "\n Not Found ", strlen("\n Not Found "));
                    write(cfd, "\n \n", strlen("\n \n"));
                }
            }
            // list
            else if (strcmp("list",strtok(in, "\r\n")) == 0) {
                    printf(" List Start \n");
                write(cfd, "\n /// List Start /// \n", strlen("\n /// List Start /// \n"));
                    node * nodePointer = myList->head;
                    while(nodePointer != NULL){
                        printf(" Key: %i | Value: %i \n", nodePointer->key,nodePointer->value);

                        write(cfd, "\n Key : ", strlen("\n Key : "));
                        keyHolder = nodePointer->key;
                        sprintf(str, "%i",keyHolder);
                        write(cfd, str, strlen(str));

                        write(cfd, "\n Value : ", strlen("\n Value : "));
                        valueHolder = nodePointer->value;
                        sprintf(str, "%i",valueHolder);
                        write(cfd, str, strlen(str));

                        write(cfd, "\n \n", strlen("\n"));

                        nodePointer = nodePointer->next;
                    }
                    printf(" List End \n");
                write(cfd, "\n /// List End /// \n\n", strlen("\n /// List End /// \n\n"));
            }

            printf(" Lesen Von Input \n");
            //bytes_read = recv(cfd, in, BUFSIZE,0);
            bytes_read = read(cfd, in, BUFSIZE);
        }

        printf("\n /// Verbindungsabbruch /// \n\n");
        close(cfd);
    }
    printf(" END \n");
    // Rendevouz Descriptor schließen
    close(rfd);

}