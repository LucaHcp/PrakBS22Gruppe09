//
// Created by User on 27.04.2022.
//

#ifndef PROJEKT_LINKEDLISTHEADER_H
#define PROJEKT_LINKEDLISTHEADER_H

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int key;
    int value;
    struct node *next;
    struct node *prev;
}node;

typedef struct linkedList {
    node *head;
    node *tail;

}linkedList;

int getNodeValueByKey(int key , linkedList * list){
    node * nodePointer = list->head;
    while (nodePointer != NULL){
        if (nodePointer->key == key){
            return nodePointer->value;
        }
        nodePointer = nodePointer->next;
    }
    return NULL;
}

node* getNodeByKey(int key , linkedList * list){
    node * nodePointer = list->head;
    while (nodePointer != NULL){
        if (nodePointer->key == key){
            return nodePointer;
        }
        nodePointer = nodePointer->next;
    }
    return NULL;
}

void addNodeToListEnd(int key, int value, linkedList * list){
    if ( list->head == NULL) {
        node *nodePointer = malloc(sizeof (node));
        nodePointer->key = key;
        nodePointer->value = value;
        nodePointer->next = NULL;
        nodePointer->prev = NULL;

        list->head = nodePointer;
        list->tail = nodePointer;
    }
    else {
        if (getNodeByKey(key,list) == NULL){
            node *nodePointer = malloc(sizeof (node));
            nodePointer->key = key;
            nodePointer->value = value;
            nodePointer->next = NULL;
            nodePointer->prev = list->tail;

            list->tail->next = nodePointer;
            list->tail = nodePointer;
        }
        else {
            printf("Found Key \n");
            node* nodePointer = getNodeByKey(key,list);
            nodePointer->value = value;
        }
    }
}

void deleteNode(int key, linkedList * list){
    if (getNodeByKey(key,list) != NULL) {
        node* nodePointer = getNodeByKey(key,list);
        if(nodePointer->prev != NULL && nodePointer->next != NULL){
            nodePointer->prev->next = nodePointer->next;
            nodePointer->next->prev = nodePointer->prev;
            nodePointer->next = NULL;
            nodePointer->prev = NULL;
        }
        else if(nodePointer->next != NULL) {
            nodePointer->next->prev = NULL;
            list->head = nodePointer->next;
            nodePointer->next = NULL;
        }
        else if(nodePointer->prev != NULL) {
            nodePointer->prev->next = NULL;
            list->tail = nodePointer->prev;
            nodePointer->prev = NULL;
        }
    }
}

void printNodes(linkedList * list){
    printf(" List Start \n");
    node * nodePointer = list->head;
    while(nodePointer != NULL){
        printf(" Key: %i | Value: %i \n", nodePointer->key,nodePointer->value);
        nodePointer = nodePointer->next;
    }
    printf(" List End \n");
}


int mainTest() {

    printf("Start \n");

    linkedList *myList = malloc(sizeof(struct linkedList));

    addNodeToListEnd(1,1,myList);
    addNodeToListEnd(2,2,myList);
    addNodeToListEnd(3,3,myList);

    printNodes(myList);

    addNodeToListEnd(2,22,myList);

    printNodes(myList);

    printf(" Node by Key:1 | Key : %i | Value %i \n", getNodeByKey(1,myList)->key, getNodeByKey(1,myList)->value);


    printf(" Delete Key : 2 \n");
    deleteNode(2,myList);

    printNodes(myList);


    printf("End \n");

    return 0;
}

#endif //PROJEKT_LINKEDLISTHEADER_H
