#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define bool int
#define true 1
#define false 0
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

int findIndexByName(Node** arr, int N, char* name) {
    int s;
    for (s = 0; s < N; ++s) {
        if (0 == strcmp(arr[s]->name, name)) return s;
    }
    return -1;
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
    while(list->next && strcmp(list->next->name, name) >= 0){
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

typedef struct {
    char** array;
    int capacity;
    int head;
    int tail;
} Stack;

Stack* createStack(int capacity) {
    Stack* temp = malloc(sizeof(Stack));
    temp->array = malloc((capacity+1) * sizeof(char*));
    temp->capacity = capacity;
    temp->head = 0;
    temp->tail = 0;
    return temp;
}

void freeStack(Stack* stack) {
    free(stack->array);
    free(stack);
}

bool isEmpty(Stack* stack) {
    return stack->tail == stack->head;
}

bool isFull(Stack* stack) {
    return ( (stack->tail+1) % (stack->capacity+1) == stack->head );
}

void push(Stack* stack, char* name) {
    assert(!isFull(stack));
    stack->array[stack->tail] = name;
    stack->tail++;
    stack->tail %= stack->capacity + 1;
}

void pop(Stack* stack) {
    assert(!isEmpty(stack));
    stack->tail--;
}

char* top(Stack* stack) {
    if (isEmpty(stack)) return NULL;
    return stack->array[stack->tail-1];
}

void printStack(Stack* stack) {
    printf("--| Start of your lovely stack |--\n");
    int fakehead = stack->head;
    while (stack->tail != fakehead) {
        printf("%s ", stack->array[fakehead]);
        fakehead++;
    } 
    printf("\n");
    printf("--| End of your lovely stack |--\n");
}

bool* visited;
void DFS(Node** arr, int num, char* name) {
    // O(E) = O(V * (V-1) / 2) = O(V^2)
    Stack* stack = createStack(num*num/2);

    // start from name
    push(stack, name);

    while (!isEmpty(stack)) {
        // printStack(stack);
        // first goes
        char* u = top(stack);
        pop(stack);

        int index = findIndexByName(arr, num, u);
        assert(index >= 0 && index < num);
        if (visited[index]) continue;
        visited[index] = true;
        printf("%s ", u);

        // each friend comes
        Node* v = arr[index]->next;
        // for (Node* v in first.neighbors()) {
        while (v) {
            // printf("%s has a friend %s\n", u, v->name);
            int index_v = findIndexByName(arr, num, v->name);
            assert(index_v >= 0 && index_v < num);
            // printf("visited(%s) == %d\n", v->name, visited[index_v]);
            if (!visited[index_v]) {
                // printf("Enstack(%s)\n", v->name);
                push(stack, v->name);
            }
            v = v->next;
        }
    }

    freeStack(stack);
}

int main(int argc, char* argv[argc + 1]){
    if (argc < 2) {
        fprintf(stderr, "Usage:\n\t%s <graph>", argv[0]);
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
    char name[NAME_LEN];
    for (i = 0; i < num; ++i) {
        fscanf(file, "%s", name);
        arr[i] = createNode(name, -1, NULL);
    }

    int ret;
    char name1[NAME_LEN], name2[NAME_LEN];
    int weight;
    while(1){
        ret = fscanf(file, "%s %s %d", name1, name2, &weight);
        if(ret != 3) break;
        // printf("add edge %s %s with weight\n", name1, name2, weight);

        // A -> B
        Node* list1 = findListByName(arr, num, name1);
        assert(list1 != NULL);
        appendNode(list1, name2, weight);
        
#if defined(DEBUG)
        printf("-------------\n");
        for (i = 0; i < num; ++i) {
            printf("%s: ", arr[i]->name);
            printList(arr[i]->next);
            printf("\n");
        }
#endif
    }


    int compare(const void* a, const void* b) {
        Node* x = *((Node**)a);
        Node* y = *((Node**)b);
        return strcmp(x->name, y->name);
    }
    
    qsort(arr, num, sizeof(Node*), compare);
    
#if defined(DEBUG)
        printf("-------------\n");
        for (i = 0; i < num; ++i) {
            printf("%s: ", arr[i]->name);
            printList(arr[i]->next);
            printf("\n");
        }
#endif

    // printf("DFS starts from %s\n", name);
    visited = malloc(num * sizeof(bool));
    for (i = 0; i < num; ++i) {
        visited[i] = false;
    }

    for (i = 0; i < num; ++i) {
        if (!visited[i]) {
            DFS(arr, num, arr[i]->name);
        }
    }
    
    // printf("End DFS started from %s\n", name);
    free(visited);

    for (i = 0; i < num; ++i) {
        freeList(arr[i]);
    }
    free(arr);
    return EXIT_SUCCESS;
}
