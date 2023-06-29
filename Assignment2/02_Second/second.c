#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NAME_LEN 16

struct s_Node{
    char name[NAME_LEN];
    int weight;
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

Node* createNode(char* name, int weight, Node* next) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    sprintf(newNode->name, "%s", name);
    newNode->weight = weight;
    newNode->next = next;
    // printf("Node %s(0x%p) created\n", name, newNode);
    return newNode;
}

int appendNode(Node* list, char* name, int weight) {
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
    list->next = createNode(name, weight, third);
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
    Node** outgraph = (Node**)malloc(num * sizeof(Node*));
    Node** ingraph = (Node**)malloc(num * sizeof(Node*));
    // printf("num = %d\n", num);

    int i;
    char name[NAME_LEN];
    int weight;
    for (i = 0; i < num; ++i) {
        fscanf(file, "%s", name);
        outgraph[i] = createNode(name, weight, NULL);
        ingraph[i] = createNode(name, weight, NULL);
    }
    
    int ret;
    char name1[NAME_LEN], name2[NAME_LEN];
    while(1){
        ret = fscanf(file, "%s %s %d", name1, name2, &weight);
        if(ret != 3) break;
        // printf("add edge %s %s\n", name1, name2);

        // A -> B
        Node* list1 = findListByName(outgraph, num, name1);
        assert(list1 != NULL);
        appendNode(list1, name2, weight);
        
        // B -> A
        Node* list2 = findListByName(ingraph, num, name2);
        assert(list2 != NULL);
        appendNode(list2, name1, weight);

        // printf("------| out graph |-------\n");
        // for (i = 0; i < num; ++i) {
        //     printf("%s: ", outgraph[i]->name);
        //     printList(outgraph[i]->next);
        //     printf("\n");
        // }
        // printf("------| in graph |-------\n");
        // for (i = 0; i < num; ++i) {
        //     printf("%s: ", ingraph[i]->name);
        //     printList(ingraph[i]->next);
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
    while(1){
        ret = fscanf(queryFile, "%s %s", op, name);
        if (ret != 2) break;
        // printf("%s %s\n", op, name);

        Node* list = NULL;
        int degree = -1;
        switch(op[0]){
            case 'i':
                list = findListByName(ingraph, num, name);
                assert(list != NULL);
                degree = getDegree(list);
                assert(degree != -1);
                printf("%d\n", degree);
                break;
            case 'o':
                list = findListByName(outgraph, num, name);
                assert(list != NULL);
                degree = getDegree(list);
                assert(degree != -1);
                printf("%d\n", degree);
                break;
            case 'a':
                list = findListByName(outgraph, num, name);
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
        freeList(outgraph[i]);
    }
    for (i = 0; i < num; ++i) {
        freeList(ingraph[i]);
    }
    free(outgraph);
    free(ingraph);
    return EXIT_SUCCESS;
}
