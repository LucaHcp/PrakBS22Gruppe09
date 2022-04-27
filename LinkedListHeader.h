//
// Created by User on 27.04.2022.
//

#ifndef PROJEKT_LINKEDLISTHEADER_H
#define PROJEKT_LINKEDLISTHEADER_H
#include <stdio.h>
#include <stdlib.h>

struct node {
    int key;
    int value;
    struct node* next;
};
typedef struct node node_t;

void printList(node_t *head) {
    node_t *temporary = head;
    printf("Start");
    while (temporary != NULL) {
        printf(" %d:%d | " , temporary->key, temporary->value);
        temporary = temporary->next;
    }
    printf("\n");
}

node_t *createNewNode(int key, int value) {
    node_t *result = malloc(sizeof (node_t));
    result->value = value;
    result->key = key;
    result->next = NULL;
    return result;
}

node_t *find_node(node_t *head, int key){
    node_t  *tmp = head;
    while(tmp != NULL){
        if (tmp->key == key) return tmp;
        tmp = tmp->next;
    }
    return NULL;
}
#endif //PROJEKT_LINKEDLISTHEADER_H
