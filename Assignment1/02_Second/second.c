#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define bool int
#define true (1)
#define false (0)

typedef struct Node{
    int value;
    struct Node* next;
}Node;

Node* createNode(int val){
    Node* temp = (Node*) malloc(sizeof(Node));
    if (!temp) return NULL;
    temp->value = val;
    temp->next = NULL;
    return temp;
}

bool insertValue(Node** head, int val){
    assert(head != NULL);

    if (!(*head)) {
        *head = createNode(val);
        return true;
    }
    // if val can be inserted at 1st place
    if (val < (*head)->value) {
        Node* temp = createNode(val);
        temp->next = (*head);
        *head = temp;
        return true;
    } 
    // find the node to insert after.
    else {
        Node* first = (*head);
        
        while (first->next && first->next->value <= val) {
            first = first->next;
        }

        if (first->value == val) {
            return false;
        }
        
        Node* third = first->next;
        // first -> third
        // first -> temp -> third
        Node* temp = createNode(val);
        first->next = temp;
        temp->next = third;
    }
    return true;
}


bool deleteValue(Node** head, int val) {
    assert(head != NULL);
    if(!(*head)) return false;
    if((*head)->value == val){
        Node* temp = (*head)->next;
        free((*head));
        *head = temp;
        return true;
    }

    Node* first = (*head);
    Node* deleted = (*head)->next;

    while (deleted != NULL && deleted->value != val) {
        first = first->next;
        deleted = deleted->next;
    }
    // deleted == NULL
    // deleted->value == val
    if (deleted == NULL) {
        // fprintf(stderr, "Value (%d) not found.\n", val);
        return false;
    }

    // first -> deleted -> third
    // first -> third
    first->next = deleted->next;
    free(deleted);
    return true;
}

void printList(Node * head) {
    if (head == NULL) {
        printf("EMPTY\n");
        return;
    }
    Node * temp = head;
    while (temp != NULL) {
        printf("%d", temp->value);
        if (temp->next != NULL) {
            printf(" ");
        }
        temp = temp->next;
    }
    printf("\n");
}

void freeList(Node* head){
    while (head) {
        deleteValue(&head, head->value);
    }
}

int main(int argc, char* argv[argc + 1]){
    FILE* file = fopen(argv[1], "r");
    if (!file){
        fprintf(stderr, "Failed to open file \"%s\"\n", argv[1]);
        return EXIT_SUCCESS;
        return EXIT_FAILURE;
    }

    char operation[16];
    int value;
    int ret;
    Node* head = NULL;
    
    while(1){
        ret = fscanf(file, "%s %d", operation, &value);
        // printf("ret = %d\n", ret);
        if(ret == EOF) break;
        // fprintf(stderr, "%s %d\n", operation, value);

        if(strcmp(operation, "INSERT") == 0){
            insertValue(&head, value);
        }else if(strcmp(operation, "DELETE") == 0){
            deleteValue(&head, value);
        }else{
            fprintf(stderr, "Invalid operation\n");
        }
        printList(head);
    }

    freeList(head);
    return EXIT_SUCCESS;
}