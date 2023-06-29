#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NAME_LEN 16

struct s_Node{
    char name[NAME_LEN];
    struct s_Node* next;
};
typedef struct s_Node Node;

Node* findListByName(Node** arr, int N, char* name) {
    int s;
    for (s = 0; s < N; ++s) {
        if (0 == strcmp(arr[s]->name, name)) return arr[s];
    }
    return NULL;
}

Node* createNode(char* name, Node* next) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    sprintf(newNode->name, "%s", name);
    newNode->next = next;
    // printf("Node %s(0x%p) created\n", name, newNode);
    return newNode;
}

int appendNode(Node* list, char* name) {
    // printf("[%s()] %s %s\n", __FUNCTION__, list->name, name);
    while(list->next && strcmp(list->next->name, name) <= 0){
        list = list->next;
    }
    // printf("%d\n", __LINE__);
    
    // if duplicated
    // if (0 == strcmp(list->name, name)) {
    //     return 0;
    // }

    // list -> new -> third
    Node* third = list->next;
    list->next = createNode(name, third);
    return 1;
}

void freeList(Node* list) {
    if (list) {
        freeList(list->next);
        free(list);
    }
}

void printList(Node* list) {
    Node* ptr = list;
    while(ptr) {
        if (ptr != list) {
            printf(" ");
        }
        printf("%s", ptr->name);
        // printf("%s(0x%p)", ptr->name, ptr);
        ptr = ptr->next;
    }
}

int getDegree(Node* list) {
    Node* ptr = list->next;
    int count = 0;
    while(ptr) {
        count++;
        ptr = ptr->next;
    }
    return count;
}

int main(int argc, char* argv[argc + 1]){
    if (argc < 3) {
        fprintf(stderr, "Usage:\n\t%s <graph> <query>", argv[0]);
        return EXIT_SUCCESS;
    }
    FILE* file = fopen(argv[1], "r");
    if (!file){
        fprintf(stderr, "Failed to open file \"%s\"\n", argv[1]);
        return EXIT_SUCCESS;
    }
    
    int num;
    fscanf(file, "%d", &num);    
    Node** arr = (Node**)malloc(num * sizeof(Node*));
    // printf("%d\n", num);

    int i;
    for (i = 0; i < num; ++i) {
        arr[i] = (Node*)malloc(sizeof(Node));
        fscanf(file, "%s", arr[i]->name);
        arr[i]->next = NULL;
    }

    int ret;
    char name1[NAME_LEN], name2[NAME_LEN];
    while(1){
        ret = fscanf(file, "%s %s", name1, name2);
        if(ret != 2) break;
        // printf("add edge %s %s\n", name1, name2);

        // A -> B
        Node* list1 = findListByName(arr, num, name1);
        assert(list1 != NULL);
        appendNode(list1, name2);

        // B -> A
        Node* list2 = findListByName(arr, num, name2);
        assert(list2 != NULL);
        appendNode(list2, name1);
        
        // printf("-------------\n");
        // for (i = 0; i < num; ++i) {
        //     printf("%s: ", arr[i]->name);
        //     printList(arr[i]->next);
        //     printf("\n");
        // }
    }

    // read query file
    FILE* queryFile = fopen(argv[2], "r");
    if (!queryFile){
        fprintf(stderr, "Failed to open file \"%s\"\n", argv[2]);
        return EXIT_SUCCESS;
    }

    char op[2];
    char name[NAME_LEN];
    while(1){
        ret = fscanf(queryFile, "%s %s", op, name);
        if (ret != 2) break;

        Node* list = NULL;
        int degree = -1;
        switch(op[0]){
            case 'd':
                list = findListByName(arr, num, name);
                assert(list != NULL);
                degree = getDegree(list);
                assert(degree != -1);
                printf("%d\n", degree);
                break;
            case 'a':
                list = findListByName(arr, num, name);
                assert(list != NULL);
                printList(list->next);
                printf("\n");
                break;

            default:
                fprintf(stderr, "Invalid operator %s\n", op);
                break;
        }
    }

    for (i = 0; i < num; ++i) {
        freeList(arr[i]);
    }
    free(arr);
    return EXIT_SUCCESS;
}
