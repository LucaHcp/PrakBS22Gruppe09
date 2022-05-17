/*******************************************************************************

  Ein TCP-Echo-Server als iterativer Server: Der Server schickt einfach die
  Daten, die der Client schickt, an den Client zurück.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>


#define BUFSIZE 1024 // Größe des Buffers
#define ENDLOSSCHLEIFE 1
#define PORT 5678

typedef struct node {
    int key;
    int value;
    struct node *next;
}node;

typedef struct linkedList {
    node *head;
    node *tail;

}linkedList;

int getNodeValueByKey(int key , linkedList *list){
    node * nodePointer = list->head;
    while (nodePointer != NULL){
        if (nodePointer->key == key){
            return nodePointer->value;
        }
        nodePointer = nodePointer->next;
    }
}

node* getNodeByKey(int key , linkedList *list){
    node * nodePointer = list->head;
    while (nodePointer != NULL){
        if (nodePointer->key == key){
            return nodePointer;
        }
        nodePointer = nodePointer->next;
    }
    return NULL;
}

void addNodeToListEnd(int key, int value, linkedList* list){
    if ( list->head == NULL) {
        node *nodePointer = malloc(sizeof (node));
        nodePointer->key = key;
        nodePointer->value = value;
        nodePointer->next = NULL;

        list->head = nodePointer;
        list->tail = nodePointer;
    }
    else {
        if (getNodeByKey(key,list) == NULL){
            node *nodePointer = malloc(sizeof (node));
            nodePointer->key = key;
            nodePointer->value = value;
            nodePointer->next = NULL;

            list->tail->next = nodePointer;
            list->tail = nodePointer;
        }
        else {
            node* nodePointer = getNodeByKey(key,list);
            nodePointer->value = value;
        }
    }
}


int main() {

    printf("Start \n");

    linkedList *myList = malloc(sizeof(struct linkedList));

    addNodeToListEnd(1,1,myList);
    addNodeToListEnd(2,2,myList);
    addNodeToListEnd(3,3,myList);

    printf( "Key : %i \n",myList->head->key);
    printf( "Key : %i \n",myList->head->next->key);
    printf( "Key : %i \n",myList->head->next->next->key);
    printf( "Tail : %i \n",myList->tail->key);
    printf( "Head : %i \n",myList->head ->key);

    printf("Start");


    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten
    char in[BUFSIZE]; // Daten vom Client an den Server
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

        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);

        // Lesen von Daten, die der Client schickt
        bytes_read = read(cfd, in, BUFSIZE);

        // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
        while (bytes_read > 0) {
            printf("sending back the %d bytes I received...\n", bytes_read);

            write(cfd, in, bytes_read);
            bytes_read = read(cfd, in, BUFSIZE);

        }
        printf("Close");
        close(cfd);
    }

    // Rendevouz Descriptor schließen
    close(rfd);

}