
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>

#define BUFSIZE 1024 // Größe des Buffers
#define ENDLOSSCHLEIFE 1
#define PORT 5678
#define NUM_OF_CHILDS 2
#define SEGSIZE sizeof(linkedListKeyValueStore)
#define NUMBER_OF_STRING 4
#define MAX_STRING_SIZE 40


typedef struct nodeKeyValueStore {
    int key;
    int value;
    int next_ID;
    int prev_ID;
} nodeKeyValueStore;

typedef struct linkedListKeyValueStore {
    int head_id;
    int tail_id;
} linkedListKeyValueStore;

int getNodeValueByKey(int key, linkedListKeyValueStore *list) {

    nodeKeyValueStore *shar_mem_TempNode = NULL;
    if (list->head_id != -1) {
        shar_mem_TempNode = (nodeKeyValueStore *) shmat(list->head_id, 0, 0);
    }
    while (shar_mem_TempNode != NULL) {
        if (shar_mem_TempNode->key == key) {
            return shar_mem_TempNode->value;
        }
        if (shar_mem_TempNode->next_ID != -1) {
            shar_mem_TempNode = (nodeKeyValueStore *) shmat(shar_mem_TempNode->next_ID, 0, 0);
        } else {
            shar_mem_TempNode = NULL;
        }
    }
    return -1;
}

int getNodeIDByKey(int key, linkedListKeyValueStore *list) {
    nodeKeyValueStore *shar_mem_TempNode = NULL;
    int id = -1;
    if (list->head_id != -1) {
        shar_mem_TempNode = (nodeKeyValueStore *) shmat(list->head_id, 0, 0);
        id = list->head_id;
    }
    while (shar_mem_TempNode != NULL) {
        if (shar_mem_TempNode->key == key) {
            return id;
        }
        if (shar_mem_TempNode->next_ID != -1) {
            id = shar_mem_TempNode->next_ID;
            shar_mem_TempNode = (nodeKeyValueStore *) shmat(shar_mem_TempNode->next_ID, 0, 0);
        } else {
            shar_mem_TempNode = NULL;
        }
    }
    return -1;
}

int getNodeIDByKeyAndValue(int key, linkedListKeyValueStore *list) {
    nodeKeyValueStore *shar_mem_TempNode = NULL;
    int id = -1;
    if (list->head_id != -1) {
        shar_mem_TempNode = (nodeKeyValueStore *) shmat(list->head_id, 0, 0);
        id = list->head_id;
    }
    while (shar_mem_TempNode != NULL) {
        if (shar_mem_TempNode->key == key) {
            if (shar_mem_TempNode->value == getpid()){
                return id;
            }
        }
        if (shar_mem_TempNode->next_ID != -1) {
            id = shar_mem_TempNode->next_ID;
            shar_mem_TempNode = (nodeKeyValueStore *) shmat(shar_mem_TempNode->next_ID, 0, 0);
        } else {
            shar_mem_TempNode = NULL;
        }
    }
    return -1;
}

void addOrReplaceNodeToListEnd(int key, int value, linkedListKeyValueStore *list) {
    nodeKeyValueStore *shar_mem_TempNode = NULL;
    int node_ID = -1;

    // Keine Node Key existiert
    if (list->head_id == -1) {
        // Shared Memory AddNode
        node_ID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
        shar_mem_TempNode = (nodeKeyValueStore *) shmat(node_ID, 0, 0);

        shar_mem_TempNode->key = key;
        shar_mem_TempNode->value = value;

        shar_mem_TempNode->next_ID = -1;
        shar_mem_TempNode->prev_ID = -1;

        list->head_id = node_ID;
        list->tail_id = node_ID;
    } else {
        // Existiert Node Key
        node_ID = getNodeIDByKey(key, list);
        if (node_ID != -1) {
            shar_mem_TempNode = (nodeKeyValueStore *) shmat(node_ID, 0, 0);
            shar_mem_TempNode->value = value;
        } else {
            node_ID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
            shar_mem_TempNode = (nodeKeyValueStore *) shmat(node_ID, 0, 0);

            shar_mem_TempNode->key = key;
            shar_mem_TempNode->value = value;
            shar_mem_TempNode->next_ID = -1;
            shar_mem_TempNode->prev_ID = list->tail_id;

            nodeKeyValueStore *temp = (nodeKeyValueStore *) shmat(list->tail_id, 0, 0);
            temp->next_ID = node_ID;
            list->tail_id = node_ID;
        }
    }
}

void addNodeToListEnd(int key, int value, linkedListKeyValueStore *list) {
    nodeKeyValueStore *shar_mem_TempNode = NULL;
    int node_ID = -1;

    // Keine Node Key existiert
    if (list->head_id == -1) {
        // Shared Memory AddNode
        node_ID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
        shar_mem_TempNode = (nodeKeyValueStore *) shmat(node_ID, 0, 0);

        shar_mem_TempNode->key = key;
        shar_mem_TempNode->value = value;

        shar_mem_TempNode->next_ID = -1;
        shar_mem_TempNode->prev_ID = -1;

        list->head_id = node_ID;
        list->tail_id = node_ID;
    } else {
        // Existiert Node Key
        node_ID = getNodeIDByKeyAndValue(key, list);
        if (node_ID != -1) {
            printf("Already Subbed \n ");
        } else {
            node_ID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
            shar_mem_TempNode = (nodeKeyValueStore *) shmat(node_ID, 0, 0);

            shar_mem_TempNode->key = key;
            shar_mem_TempNode->value = value;
            shar_mem_TempNode->next_ID = -1;
            shar_mem_TempNode->prev_ID = list->tail_id;

            nodeKeyValueStore *temp = (nodeKeyValueStore *) shmat(list->tail_id, 0, 0);
            temp->next_ID = node_ID;
            list->tail_id = node_ID;
        }
    }
}

void deleteNode(int key, linkedListKeyValueStore *list) {
    int node_ID = getNodeIDByKey(key, list);
    if (node_ID != -1) {

        int prev_ID = -1;
        int next_ID = -1;
        nodeKeyValueStore *shar_mem_TempNode = NULL;
        nodeKeyValueStore *shar_mem_TempNodeNext = NULL;
        nodeKeyValueStore *shar_mem_TempNodePrev = NULL;
        shar_mem_TempNode = (nodeKeyValueStore *) shmat(node_ID, 0, 0);

        // Mittel Node
        if (shar_mem_TempNode->prev_ID != -1 && shar_mem_TempNode->next_ID != -1) {

            prev_ID = shar_mem_TempNode->prev_ID;
            next_ID = shar_mem_TempNode->next_ID;

            shar_mem_TempNodeNext = (nodeKeyValueStore *) shmat(next_ID, 0, 0);
            shar_mem_TempNodePrev = (nodeKeyValueStore *) shmat(prev_ID, 0, 0);

            shar_mem_TempNodePrev->next_ID = next_ID;
            shar_mem_TempNodeNext->prev_ID = prev_ID;
        }
            // AnfangsNode
        else if (shar_mem_TempNode->next_ID != -1) {
            next_ID = shar_mem_TempNode->next_ID;
            shar_mem_TempNodeNext = (nodeKeyValueStore *) shmat(next_ID, 0, 0);
            shar_mem_TempNodeNext->prev_ID = -1;

            list->head_id = node_ID;
        }
            // End Node
        else if (shar_mem_TempNode->prev_ID != -1) {

            prev_ID = shar_mem_TempNode->prev_ID;
            shar_mem_TempNodePrev = (nodeKeyValueStore *) shmat(prev_ID, 0, 0);
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

void notifyAllSubs(){

}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

int main() {

    // Variable sem_id für die Semaphorgruppe und
    // aus technischen Gründen eine Variable marker[1].
    // Variable sem_id für die Semaphorgruppe und
    // aus technischen Gründen eine Variable marker[1].
    int sem_id1, sem_id2;
    unsigned short marker[1];

    // Es folgt das Anlegen der Semaphorgruppe. Es wird hier nur ein
    // Semaphor erzeugt

    // Gruppe 1
    sem_id1 = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644);
    if (sem_id1 == -1) {
        perror("Die Gruppe konnte nicht angelegt werden!");
        exit(1);
    }

    // Anschließend wird der Semaphor auf 1 gesetzt
    marker[0] = 1;
    semctl(sem_id1, 1, SETALL, marker);  // alle Semaphore auf 1

    // Gruppe 2
    sem_id2 = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644);
    if (sem_id2 == -1) {
        perror("Die Gruppe konnte nicht angelegt werden!");
        exit(1);
    }

    // Anschließend wird der Semaphor auf 1 gesetzt
    marker[0] = 1;
    semctl(sem_id2, 1, SETALL, marker);  // alle Semaphore auf 1

    // Anforderung des Shared Memory Segments
    int shm_id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0644);

    if (shm_id == -1) {
        perror("Das Segment konnte nicht angelegt werden!");
        exit(1);
    }

    int * shar_mem = (int *) shmat(shm_id, 0, 0);
    *shar_mem = 0;

    // Anforderung des Shared Memory Segments Count Clients
    int shm_id_client = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0644);
    if (shm_id_client == -1) {
        perror("Das Segment konnte nicht angelegt werden!");
        exit(1);
    }

    int *shar_mem_client = (int *) shmat(shm_id_client, 0, 0);
    *shar_mem_client = 0;

    int keyHolder;
    int valueHolder;

    // LinkedList für Key Value Store
    linkedListKeyValueStore *shar_mem_LinkedListKeyValueStore = NULL;
    int linkedListKeyValueStore_ID;
    int nextNode_ID;

    // Shared Memory LinkedList Key Value Store
    linkedListKeyValueStore_ID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
    shar_mem_LinkedListKeyValueStore = (linkedListKeyValueStore *) shmat(linkedListKeyValueStore_ID, 0, 0);
    shar_mem_LinkedListKeyValueStore->head_id = -1;
    shar_mem_LinkedListKeyValueStore->tail_id = -1;


    // LinkedList für Sub Store
    linkedListKeyValueStore *shar_mem_LinkedListSubStore = NULL;
    int linkedListSubStore_ID;
    int nextNodeSub_ID;

    // Shared Memory LinkedList Sub Store
    linkedListSubStore_ID = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT | 0600);
    shar_mem_LinkedListSubStore = (linkedListKeyValueStore *) shmat(linkedListSubStore_ID, 0, 0);
    shar_mem_LinkedListSubStore->head_id = -1;
    shar_mem_LinkedListSubStore->tail_id = -1;


    char *helpMessage = (" /// HELP /// \n Key = Int | Value = Int \n quit to Quit \n list to List all Data \n put(Key,Value) to put date in \n get(Key) to return the Value of given Key \n del(Key) to delete date by key \n\n");
    char str[BUFSIZE];

    int rfd; // Rendevouz-Descriptor
    int cfd; // Verbindungs-Descriptor

    struct sockaddr_in client; // Socketadresse eines Clients
    socklen_t client_len; // Länge der Client-Daten
    char *in[BUFSIZE]; // Daten vom Client an den Server
    int bytes_read; // Anzahl der Bytes, die der Client geschickt hat


    // Socket erstellen
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    if (rfd < 0) {
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
    if (brt < 0) {
        fprintf(stderr, "socket konnte nicht gebunden werden\n");
        exit(-1);
    }


    // Socket lauschen lassen
    int lrt = listen(rfd, 5);
    if (lrt < 0) {
        fprintf(stderr, "socket konnte nicht listen gesetzt werden\n");
        exit(-1);
    }

    printf(" Start \n");

    while(ENDLOSSCHLEIFE){
        printf(" Warte auf Verbindung \n");
        // Verbindung eines Clients wird entgegengenommen
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);
        int pid = fork();


            if (pid == 0) {

                int isBlocker = 0;
                struct sembuf enter, leave;
                enter.sem_num = leave.sem_num = 0;  // Semaphor 0 in der Gruppe
                enter.sem_flg = leave.sem_flg = SEM_UNDO;
                enter.sem_op = -1; // blockieren, DOWN-Operation
                leave.sem_op = 1;  // freigeben, UP-Operation

                pid = fork();

                while (ENDLOSSCHLEIFE) {
                    if (pid != 0) {
                        // WarteschlangenNachrichten Prozesse

                    } else {

                        write(cfd, "Input \n", strlen("Input \n"));

                        printf(" Lesen Von Input \n");
                        memset(in, '\0', BUFSIZE);

                        // Lesen von Daten, die der Client schickt
                        bytes_read = read(cfd, in, BUFSIZE);

                        while (bytes_read > 0) {

                            if (*shar_mem == 1 && isBlocker == 0) {
                                write(cfd, " Blocked \n", strlen(" Blocked \n"));
                                printf(" Blocked \n");
                            }
                                //BEG
                            else if (strcmp("beg", strtok(in, "\r\n")) == 0) {
                                semop(sem_id2, &enter, 1);
                                if (*shar_mem == 1 && isBlocker == 0) {
                                    write(cfd, " Blocked \n", strlen(" Blocked \n"));
                                    printf(" Blocked \n");

                                } else {
                                    *shar_mem = 1;
                                    isBlocker = 1;
                                    write(cfd, "begin \n", strlen("begin \n"));
                                    printf(" %i \n", *shar_mem);
                                }
                                semop(sem_id2, &leave, 1);
                            }
                                //END
                            else if (strcmp("end", strtok(in, "\r\n")) == 0) {
                                if (isBlocker == 1) {
                                    *shar_mem = 0;
                                    isBlocker = 0;
                                    write(cfd, "ending \n", strlen("ending \n"));
                                    printf(" %i \n", *shar_mem);
                                } else {
                                    write(cfd, " Not Blocking \n", strlen(" Not Blocking \n"));
                                    printf(" Not Blocking \n");
                                }
                            }
                                //BEG
                            else if (strcmp("sub", strtok(in, "\r\n")) == 0) {
                                semop(sem_id1, &enter, 1);

                                //Get Key
                                write(cfd, "Key : ", strlen("Key : "));
                                bytes_read = read(cfd, in, BUFSIZE);

                                keyHolder = atoi(in);

                                valueHolder = getpid();

                                printf("Sub Key %i with PId : %i \n ", keyHolder, valueHolder);

                                // Return Put Key Value
                                sprintf(str, "%i", keyHolder);
                                write(cfd, "\n Put Key : ", strlen("\n Put Key : "));
                                write(cfd, str, strlen(str));

                                sprintf(str, "%i", valueHolder);
                                write(cfd, " | Sub : ", strlen(" | Sub : "));
                                write(cfd, str, strlen(str));
                                write(cfd, "\n \n", strlen("\n \n"));

                                // Put in Linked list
                                addNodeToListEnd(keyHolder, valueHolder, shar_mem_LinkedListSubStore);

                                semop(sem_id1, &leave, 1);
                            }
                                //QUIT
                            else if (strcmp("quit", strtok(in, "\r\n")) == 0) {
                                printf(" Quit \n ");
                                printf("\n /// Verbindungsabbruch /// \n\n");
                                close(cfd);
                                kill(getpid(),SIGINT);
                            }
                                // HELP
                            else if (strcmp("help", strtok(in, "\r\n")) == 0) {
                                write(cfd, helpMessage, strlen(helpMessage));
                                printf(" Help \n");
                            }
                                // PUT
                            else if (strcmp("put", strtok(in, "\r\n")) == 0) {
                                semop(sem_id1, &enter, 1);
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

                                // Put in Linked list
                                addOrReplaceNodeToListEnd(keyHolder, valueHolder, shar_mem_LinkedListKeyValueStore);

                                // Notify All Subs
                                printf(" Notify All Subs Start \n");

                                nodeKeyValueStore *shar_mem_TempNode = NULL;
                                if (shar_mem_LinkedListSubStore->head_id != -1) {
                                    shar_mem_TempNode = (nodeKeyValueStore *) shmat(shar_mem_LinkedListSubStore->head_id, 0, 0);
                                }
                                while (shar_mem_TempNode != NULL) {
                                    if  (keyHolder == shar_mem_TempNode->key) {


                                    }
                                    if (shar_mem_TempNode->next_ID != -1) {
                                        shar_mem_TempNode = (nodeKeyValueStore *) shmat(shar_mem_TempNode->next_ID, 0, 0);
                                    } else {
                                        shar_mem_TempNode = NULL;
                                    }
                                }
                                printf(" Notify All Subs End \n");

                                semop(sem_id1, &leave, 1);
                            }
                                // GET
                            else if (strcmp("get", strtok(in, "\r\n")) == 0) {
                                semop(sem_id1, &enter, 1);
                                semop(sem_id1, &leave, 1);
                                //Get Key
                                write(cfd, "Key : ", strlen("Key : "));
                                bytes_read = read(cfd, in, BUFSIZE);
                                // Store Key
                                keyHolder = atoi(in);
                                // Store Value
                                valueHolder = getNodeValueByKey(keyHolder, shar_mem_LinkedListKeyValueStore);
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
                                semop(sem_id1, &enter, 1);
                                //Get Key
                                write(cfd, "Key : ", strlen("Key : "));
                                bytes_read = read(cfd, in, BUFSIZE);
                                // Store Key
                                keyHolder = atoi(in);
                                if (getNodeIDByKey(keyHolder, shar_mem_LinkedListKeyValueStore) != -1) {
                                    printf("Found Key %i with Value : %i \n ", keyHolder, valueHolder);
                                    deleteNode(keyHolder, shar_mem_LinkedListKeyValueStore);
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

                                notifyAllSubs(keyHolder);

                                semop(sem_id1, &leave, 1);
                            }
                                //LIST
                            else if (strcmp("list", strtok(in, "\r\n")) == 0) {
                                semop(sem_id1, &enter, 1);
                                semop(sem_id1, &leave, 1);
                                printf(" List Start \n");
                                write(cfd, "\n /// List Start /// \n", strlen("\n /// List Start /// \n"));

                                nodeKeyValueStore *shar_mem_TempNode = NULL;
                                if (shar_mem_LinkedListKeyValueStore->head_id != -1) {
                                    shar_mem_TempNode = (nodeKeyValueStore *) shmat(shar_mem_LinkedListKeyValueStore->head_id, 0, 0);
                                }

                                while (shar_mem_TempNode != NULL) {

                                    printf(" Key: %i | Value: %i \n", shar_mem_TempNode->key, shar_mem_TempNode->value);

                                    keyHolder = shar_mem_TempNode->key;

                                    valueHolder = shar_mem_TempNode->value;

                                    write(cfd, "\n Key : ", strlen("\n Key : "));
                                    sprintf(str, "%i", keyHolder);
                                    write(cfd, str, strlen(str));

                                    write(cfd, " | Value : ", strlen(" | Value : "));
                                    sprintf(str, "%i", valueHolder);
                                    write(cfd, str, strlen(str));

                                    write(cfd, "\n", strlen("\n"));

                                    if (shar_mem_TempNode->next_ID != -1) {
                                        shar_mem_TempNode = (nodeKeyValueStore *) shmat(shar_mem_TempNode->next_ID, 0, 0);
                                    } else {
                                        shar_mem_TempNode = NULL;
                                    }
                                }
                                printf(" List End \n");
                                write(cfd, "\n /// List End /// \n\n", strlen("\n /// List End /// \n\n"));
                            }
                                //SUBLIST
                            else if (strcmp("sublist", strtok(in, "\r\n")) == 0) {
                                semop(sem_id1, &enter, 1);
                                semop(sem_id1, &leave, 1);
                                printf(" SubList Start \n");
                                write(cfd, "\n /// SubList Start /// \n", strlen("\n /// SubList Start /// \n"));

                                nodeKeyValueStore *shar_mem_TempNode = NULL;
                                if (shar_mem_LinkedListSubStore->head_id != -1) {
                                    shar_mem_TempNode = (nodeKeyValueStore *) shmat(shar_mem_LinkedListSubStore->head_id, 0, 0);
                                }

                                while (shar_mem_TempNode != NULL) {

                                    printf(" Key: %i | Value: %i \n", shar_mem_TempNode->key, shar_mem_TempNode->value);

                                    keyHolder = shar_mem_TempNode->key;

                                    valueHolder = shar_mem_TempNode->value;

                                    write(cfd, "\n Key : ", strlen("\n Key : "));
                                    sprintf(str, "%i", keyHolder);
                                    write(cfd, str, strlen(str));

                                    write(cfd, " | Value : ", strlen(" | Value : "));
                                    sprintf(str, "%i", valueHolder);
                                    write(cfd, str, strlen(str));

                                    write(cfd, "\n", strlen("\n"));

                                    if (shar_mem_TempNode->next_ID != -1) {
                                        shar_mem_TempNode = (nodeKeyValueStore *) shmat(shar_mem_TempNode->next_ID, 0, 0);
                                    } else {
                                        shar_mem_TempNode = NULL;
                                    }
                                }
                                printf(" SubList End \n");
                                write(cfd, "\n /// List End /// \n\n", strlen("\n /// List End /// \n\n"));
                            }
                            else {
                                printf("No choice \n");
                            }

                                            //Start New Input LOOP//
                            //////////////////////////////////////////////////////////////

                            printf(" Lesen Von Input \n");
                            write(cfd, "Input \n", strlen("Input \n"));
                            bytes_read = read(cfd, in, BUFSIZE);

                        }
                    }
                }
            }
        }

    printf(" END \n");

    // Das Shared Memory Segment wird abgekoppelt und freigegeben.
    shmdt(shar_mem_LinkedListKeyValueStore);
    shmctl(linkedListKeyValueStore_ID, IPC_RMID, 0);
    semctl(sem_id1, 0, IPC_RMID);
    semctl(sem_id2, 0, IPC_RMID);

    // Rendevouz Descriptor schließen
    close(rfd);
}

#pragma clang diagnostic pop