#include <stdio.h>
#include <stdlib.h>
#include "LinkedListHeader.h"
int main(){

    node_t *head = NULL;
    node_t *tmp;

    for(int i = 1; i <= 10; i++){
        tmp = createNewNode(i,i);
        tmp->next = head;
        head = tmp;
    }

    printList(head);

    tmp = find_node(head,4);
    printf("Found  %d:%d ",tmp->key,tmp->value);

    return 0;
}
