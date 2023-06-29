#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef bool
    #define bool unsigned char
    #define true (1)
    #define false (0)
#endif

// #ifdef DEBUG
//     #define Debug(x) do{ printf("[%s:%d] ", __FILE__, __LINE__); x; } while(0)
// #else
//     #define Debug(x) 
// #endif

typedef struct s_Node{
	int value;
	struct s_Node* next;
} Node;

//insert element at the end of the linked list
bool enqueue(Node** head, int val){
    // create a new node
    Node* temp = (Node*) malloc(sizeof(Node));
    temp->value = val;
    temp->next = NULL;

    // fail to alloc memory
    if (!temp) {
        fprintf(stderr, "fail to alloc memory\n");
        return false;
    }
    
    if (NULL == head) {
        return false;
    }

    // linked list is empty
    if (NULL == *head) {
        // printf("linked list is empty\n");
        *head = temp;
        return true;
    }
    
    // find the tail of the linked list
    Node* tail = *head;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = temp;
    return true;
}

// insert element at the beginning of the linked list
bool push(Node** head, int val){
    Node* temp = (Node*) malloc(sizeof(Node));
    if (!temp) return false;
    temp->value = val;
    temp->next = *head;
    *head = temp;
    return true;
}

//pop and delete the first element at the beginning of the linked list
bool pop(Node** head){
    if (!head) return false;    // protection
    if (!(*head)) return false; // if (!head) return NULL;
    Node* temp = (*head)->next; // Node* temp = head->next;
    free(*head);                // free(head);
    *head = temp;
    return true;
}

void print(Node* head) {
	if (!head) {
		printf("EMPTY\n");
		return;
	}
    Node* temp = head;
    while (temp) {
        if (temp != head) {
            // printf(" ->");
            printf(" ");
        }
        printf("%d", temp->value);
        temp = temp->next;
    }
    printf("\n");
}


void free_list(Node* head){
    while(head){
        pop(&head);
    }
    /*
    if (!head) return;
    Node* temp;
    while (head->next != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
    free(head);
    */
}


int main(int argc, char* argv[argc + 1]){
    FILE* file = fopen(argv[1], "r");
    if (!file){
        fprintf(stderr, "Failed to open file \"%s\"\n", argv[1]);
    }
    
    char operation[16];
    int value;
    int ret;
    Node* head = NULL;

    while(1){
        ret = fscanf(file, "%s %d", operation, &value);
        // printf("ret = %d\n", ret);
        if(ret == EOF) break;
        // printf("%s %d\n", operation, value);

        if(strcmp(operation, "ENQUEUE") == 0){
            enqueue(&head, value);
        }else if(strcmp(operation, "PUSH") == 0){
            push(&head, value);
        }else if(strcmp(operation, "POP") == 0){
            pop(&head);
        }else{
            fprintf(stderr, "Invalid operation\n");
        }
        print(head);
    }

    free_list(head);
    return EXIT_SUCCESS;
}