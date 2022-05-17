//
// Created by User on 27.04.2022.
//

#ifndef PROJEKT_LINKEDLISTHEADER_H
#define PROJEKT_LINKEDLISTHEADER_H
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    char key;
    char value;
    struct node * next;
}node;

void printList(node *head) {
    node *temporary = head;
    printf("Start");
    while (temporary != NULL) {
        printf(" %d:%d | " , temporary->key, temporary->value);
        temporary = temporary->next;
    }
    printf("\n");
}

node * createNewNode(char key, char value) {
    node *result = malloc(sizeof (node));
    result->value = value;
    result->key = key;
    result->next = NULL;
    return result;
}

node *find_node(node *head, char key){
    node  *tmp = head;
    while(tmp != NULL){
        if (tmp->key == key) return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

int put(char* key, char* value);

int get(char* key, char* res);


#endif //PROJEKT_LINKEDLISTHEADER_H
