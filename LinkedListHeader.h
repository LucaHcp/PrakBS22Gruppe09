//
// Created by User on 27.04.2022.
//

#ifndef PROJEKT_LINKEDLISTHEADER_H
#define PROJEKT_LINKEDLISTHEADER_H

#include <stdio.h> 
#include <stdlib.h>


int getNodeValueByKey(int key , linkedList * list){
    node * nodePointer = list->head;
    while (nodePointer != NULL){
        if (nodePointer->key == key){
            return nodePointer->value;
        }
        nodePointer = nodePointer->next;
    }
    return -1;
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

linkedList * addNodeToListEnd(int key, int value, linkedList * list){
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
        else {
            list->head = NULL;
            list->tail = NULL;
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

#endif //PROJEKT_LINKEDLISTHEADER_H
