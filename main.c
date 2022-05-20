
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>


#define BUFSIZE 1024 // Größe des Buffers
#define ENDLOSSCHLEIFE 1
#define PORT 5678
#define NUM_OF_CHILDS 2
#define SEGSIZE sizeof(linkedList)

typedef struct node {
    int key;
    int value;
    int next_ID;
    int prev_ID;
}node;

typedef struct linkedList {
    int head_id;
    int tail_id;
}linkedList;

int getNodeValueByKey(int key , linkedList * list){

    node * shar_mem_TempNode = NULL;
    if (list->head_id != -1){
        shar_mem_TempNode = (node *)shmat(list->head_id, 0, 0);
    }
    while (shar_mem_TempNode != NULL ){
        if (shar_mem_TempNode->key == key){
            return shar_mem_TempNode->value;
        }
        if (shar_mem_TempNode->next_ID != -1){
            shar_mem_TempNode = (node *)shmat(shar_mem_TempNode->next_ID, 0, 0);
        }
        else {
            shar_mem_TempNode = NULL;
        }
    }
    return -1;
}

int getNodeIDByKey(int key , linkedList * list){
    node * shar_mem_TempNode = NULL;
    int id = -1;
    if (list->head_id != -1){
        shar_mem_TempNode = (node *)shmat(list->head_id, 0, 0);
        id = list->head_id;
    }
    while (shar_mem_TempNode != NULL){
        if (shar_mem_TempNode->key == key){
            return id;
        }
        if (shar_mem_TempNode->next_ID != -1){
            id = shar_mem_TempNode->next_ID;
            shar_mem_TempNode = (node *)shmat(shar_mem_TempNode->next_ID, 0, 0);
        }
        else {
            shar_mem_TempNode = NULL;
        }
    }
    return -1;
}

void addNodeToListEnd(int key, int value, linkedList * list){
    printf(" \n 0 \n");
    node * shar_mem_TempNode = NULL;
    int node_ID = -1;

    printf(" \n 1 \n");

    // Keine Node Key existiert
    if (list->head_id == -1) {
        printf(" \n 2 \n");
        // Shared Memory AddNode
        node_ID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0600);
        shar_mem_TempNode = (node *)shmat(node_ID, 0, 0);

        shar_mem_TempNode->key = key;
        shar_mem_TempNode->value = value;

        shar_mem_TempNode->next_ID = -1;
        shar_mem_TempNode->prev_ID = -1;

        list->head_id = node_ID;
        list->tail_id = node_ID;
        printf(" \n 3 \n");
    }
    else {
        printf(" \n 4 \n");
        // Existiert Node Key
        node_ID = getNodeIDByKey(key,list);
        printf("\n Node id : %i \n" , node_ID);
        if (node_ID != -1) {
            printf(" \n 5 \n");
            shar_mem_TempNode = (node *)shmat(node_ID, 0, 0);
            shar_mem_TempNode->value = value;
            printf(" \n 6 \n");
        }
        else {
            printf(" \n 7 \n");
            node_ID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0600);
            shar_mem_TempNode = (node *)shmat(node_ID, 0, 0);

            shar_mem_TempNode->key = key;
            shar_mem_TempNode->value = value;
            shar_mem_TempNode->next_ID = -1;
            shar_mem_TempNode->prev_ID = list->tail_id;

            node * temp = (node *)shmat(list->tail_id, 0, 0);
            temp->next_ID = node_ID;
            list->tail_id = node_ID;
            printf(" \n 8 \n");
        }
    }
    printf(" \n 9 \n");
}

/*
 * // Das Shared Memory Segment wird abgekoppelt und freigegeben.
shmdt(shar_mem_LinkedList);
shmctl(linkedList_ID, IPC_RMID, 0);
 */

void deleteNode(int key, linkedList * list){
    int node_ID = getNodeIDByKey(key,list);
    if (node_ID != -1) {

        int prev_ID = -1;
        int next_ID = -1;
        node * shar_mem_TempNode = NULL;
        node * shar_mem_TempNodeNext = NULL;
        node * shar_mem_TempNodePrev = NULL;
        shar_mem_TempNode = (node *)shmat(node_ID, 0, 0);

        // Mittel Node
        if(shar_mem_TempNode->prev_ID != -1 && shar_mem_TempNode->next_ID != -1){

             prev_ID = shar_mem_TempNode->prev_ID;
             next_ID = shar_mem_TempNode->next_ID;

             shar_mem_TempNodeNext = (node *)shmat(next_ID, 0, 0);
             shar_mem_TempNodePrev = (node *)shmat(prev_ID, 0, 0);

             shar_mem_TempNodePrev->next_ID = next_ID;
             shar_mem_TempNodeNext->prev_ID = prev_ID;
        }
        // AnfangsNode
        else if(shar_mem_TempNode->next_ID != -1) {
            next_ID = shar_mem_TempNode->next_ID;
            shar_mem_TempNodeNext = (node *)shmat(next_ID, 0, 0);
            shar_mem_TempNodeNext->prev_ID = -1;

            list->head_id = node_ID;
        }
        // End Node
        else if(shar_mem_TempNode->prev_ID != -1) {

            prev_ID = shar_mem_TempNode->prev_ID;
            shar_mem_TempNodePrev = (node *)shmat(prev_ID, 0, 0);
            shar_mem_TempNodePrev->next_ID = -1;

            list->tail_id = node_ID;
        }
        // Einzige Node
        else {
            list->head_id = -1;
            list->tail_id = -1;
        }

        shmdt(shar_mem_TempNode);
        shmctl(node_ID, IPC_RMID, 0);
    }
}

void printNodes(linkedList * list){

    node * shar_mem_TempNode = NULL;
    if (list->head_id != -1){
        shar_mem_TempNode = (node *)shmat(list->head_id, 0, 0);
    }

    printf(" List Start \n");
    while(shar_mem_TempNode != NULL){

        printf(" Key: %i | Value: %i \n", shar_mem_TempNode->key,shar_mem_TempNode->value);

        if (shar_mem_TempNode->next_ID != -1){
            shar_mem_TempNode = (node *)shmat(shar_mem_TempNode->next_ID, 0, 0);
        }
        else {
            shar_mem_TempNode = NULL;
        }
    }
    printf(" List End \n");
}



int main() {

    linkedList * shar_mem_LinkedList = NULL;
    int linkedList_ID;
    int nextNode_ID;


    int keyHolder;
    int valueHolder;

    char * helpMessage =  (" /// HELP /// \n Key = Int | Value = Int \n quit to Quit \n list to List all Data \n put(Key,Value) to put date in \n get(Key) to return the Value of given Key \n del(Key) to delete date by key \n\n");
    char str[BUFSIZE];
    printf(" Start \n");

    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    int i;
    int pid[NUM_OF_CHILDS]; /*  enthält die PIDs der Kindprozesse       */

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



    // Shared Memory LinkedList
    linkedList_ID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0600);
    shar_mem_LinkedList = (linkedList *)shmat(linkedList_ID, 0, 0);
    shar_mem_LinkedList->head_id = -1;
    shar_mem_LinkedList->tail_id = -1;




    // Der Vaterprozess erzeugt eine bestimmte Anzahl Kindprozesse
    for (i = 0; i < NUM_OF_CHILDS; i++) {
        pid[i] = fork();
        if (pid[i] == -1) {
            printf("Kindprozess konnte nicht erzeugt werden!\n");
            exit(1);
        }
    }




    if (pid[i] == 0) {

        while (ENDLOSSCHLEIFE) {

            printf(" Warte auf Verbindung \n");
            // Verbindung eines Clients wird entgegengenommen
            cfd = accept(rfd, (struct sockaddr *) &client, &client_len);


            printf(" Lesen Von Input \n");

            memset(in, '\0', BUFSIZE);

            // Lesen von Daten, die der Client schickt
            bytes_read = read(cfd, in, BUFSIZE);
            //bytes_read = recv(cfd, in, BUFSIZE, 0);


            // Zurückschicken der Daten, solange der Client welche schickt (und kein Fehler passiert)
            while (bytes_read > 0) {
                //printf(" sending back the %d bytes I received...\n", bytes_read);
                // write(cfd, in, bytes_read);

                // Check Input
                // QUIT
                if (strcmp("quit", strtok(in, "\r\n")) == 0) {
                    printf(" Quit \n ");
                    break;
                }
                    // HELP
                else if (strcmp("help", strtok(in, "\r\n")) == 0) {
                    write(cfd, helpMessage, strlen(helpMessage));
                    printf(" Help \n");
                }
                    // PUT
                else if (strcmp("put", strtok(in, "\r\n")) == 0) {
                    //Get Key
                    write(cfd, "Key : ", strlen("Key : "));
                    bytes_read = read(cfd, in, BUFSIZE);

                    keyHolder = atoi(in);

                    //Get Value
                    write(cfd, "Value : ", strlen("Value : "));
                    bytes_read = read(cfd, in, BUFSIZE);

                    valueHolder = atoi(in);

                    printf("Put Key %i with Value : %i \n ", keyHolder, valueHolder);

                    // Return Put Key Value
                    sprintf(str, "%i", keyHolder);
                    write(cfd, "\n Put Key : ", strlen("\n Put Key : "));
                    write(cfd, str, strlen(str));

                    sprintf(str, "%i", valueHolder);
                    write(cfd, " | Value : ", strlen(" | Value : "));
                    write(cfd, str, strlen(str));
                    write(cfd, "\n \n", strlen("\n \n"));


                    printf("\n test1 \n");

                    // Put in Linked list
                    addNodeToListEnd(keyHolder, valueHolder, shar_mem_LinkedList);

                    printf("\n test2 \n");

                }
                // GET
                else if (strcmp("get", strtok(in, "\r\n")) == 0) {
                    //Get Key
                    write(cfd, "Key : ", strlen("Key : "));
                    bytes_read = read(cfd, in, BUFSIZE);
                    // Store Key
                    keyHolder = atoi(in);
                    // Store Value
                    valueHolder = getNodeValueByKey(keyHolder,shar_mem_LinkedList );
                    if (valueHolder != -1) {
                        printf("Found Key %i with Value : %i \n ", keyHolder, valueHolder);
                        // Return Value
                        write(cfd, "\n Found Key : ", strlen("\n Found Key : "));
                        sprintf(str, "%i", keyHolder);
                        write(cfd, str, strlen(str));
                        write(cfd, " | Value : ", strlen(" | Value : "));
                        sprintf(str, "%i", valueHolder);
                        write(cfd, str, strlen(str));
                        write(cfd, "\n \n", strlen("\n \n"));
                    } else {
                        // Return Not Found
                        printf("Found No Key : %i ", keyHolder);
                        write(cfd, "\n No Key : ", strlen("\n No Key : "));
                        sprintf(str, "%i", keyHolder);
                        write(cfd, str, strlen(str));
                        write(cfd, "\n \n", strlen("\n \n"));
                    }
                }
                // DELETE
                else if (strcmp("del", strtok(in, "\r\n")) == 0) {
                    //Get Key
                    write(cfd, "Key : ", strlen("Key : "));
                    bytes_read = read(cfd, in, BUFSIZE);
                    // Store Key
                    keyHolder = atoi(in);
                    if ( getNodeIDByKey(keyHolder, shar_mem_LinkedList) != -1) {
                        printf("Found Key %i with Value : %i \n ", keyHolder, valueHolder);
                        deleteNode(keyHolder, shar_mem_LinkedList);
                        write(cfd, "\n Delete Key : ", strlen("\n Delete Key : "));
                        sprintf(str, "%i", keyHolder);
                        write(cfd, str, strlen(str));
                        write(cfd, "\n \n", strlen("\n \n"));
                    } else {
                        printf(" No Key Found : %i \n ", keyHolder);
                        write(cfd, "\n Key : ", strlen("\n Key : "));
                        sprintf(str, "%i", keyHolder);
                        write(cfd, str, strlen(str));
                        write(cfd, "\n Not Found ", strlen("\n Not Found "));
                        write(cfd, "\n \n", strlen("\n \n"));
                    }
                }
                // list
                else if (strcmp("list", strtok(in, "\r\n")) == 0) {
                    printf(" List Start \n");
                    write(cfd, "\n /// List Start /// \n", strlen("\n /// List Start /// \n"));

                    node * shar_mem_TempNode = NULL;
                    if (shar_mem_LinkedList->head_id != -1){
                        shar_mem_TempNode = (node *)shmat(shar_mem_LinkedList->head_id, 0, 0);
                    }

                    while(shar_mem_TempNode != NULL){

                        printf(" Key: %i | Value: %i \n", shar_mem_TempNode->key,shar_mem_TempNode->value);

                        keyHolder = shar_mem_TempNode->key;

                        valueHolder = shar_mem_TempNode->value;

                        write(cfd, "\n Key : ", strlen("\n Key : "));
                        sprintf(str, "%i", keyHolder);
                        write(cfd, str, strlen(str));

                        write(cfd, " | Value : ", strlen(" | Value : "));
                        sprintf(str, "%i", valueHolder);
                        write(cfd, str, strlen(str));

                        write(cfd, "\n", strlen("\n"));

                        if (shar_mem_TempNode->next_ID != -1){
                            shar_mem_TempNode = (node *)shmat(shar_mem_TempNode->next_ID, 0, 0);
                        }
                        else {
                            shar_mem_TempNode = NULL;
                        }
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
    }
    printf(" END \n");

    for (i = 0; i < NUM_OF_CHILDS; i++) {
        waitpid(pid[i], NULL, 0);
    }
    printf("Alle %d Kindprozesse wurden beendet.\n", NUM_OF_CHILDS);


    // Das Shared Memory Segment wird abgekoppelt und freigegeben.
    shmdt(shar_mem_LinkedList);
    shmctl(linkedList_ID, IPC_RMID, 0);

    // Rendevouz Descriptor schließen
    close(rfd);

}