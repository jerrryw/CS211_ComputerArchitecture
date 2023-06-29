#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#define bool int
#define true 1
#define false 0
#define NAME_LEN 16
#define NO_VAL (-1)

/***************************************
 * Graph / Node
 ***************************************/
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

void destroyList(Node* list) {
    if (list) {
        destroyList(list->next);
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

/***************************************
 * Stack
 ***************************************/
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

void destroyStack(Stack* stack) {
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

/***************************************
 * BellManFord
 ***************************************/
typedef struct {
    Node** graph;
    int num;
    char* source;
    Stack* tpstack;
    int* distance;
} BellManFordData;

void runBellManFordAlgorithm(BellManFordData* data) {
    Node** graph = data->graph;
    int num = data->num;
    char* source = data->source;
    Stack* tpstack = data->tpstack;
    int* distance = data->distance;
    int i;
    for (i = 0; i < num; ++i) {
        distance[i] = INT_MAX;
    }
    int s = findIndexByName(graph, num, source);
    assert(s >= 0 && s < num);
    distance[s] = 0;
    
    for (i = tpstack->tail-1; i >= 0; --i) {
        char* u = tpstack->array[i];
        // printf("%s\n", u);
        // print friends of u
        int index_u = findIndexByName(graph, num, u);
        assert(index_u >= 0 && index_u < num);
        Node* list = graph[index_u]->next;

        while (list) {
            // printf("\t%s\n", list->name);
            int index_v = findIndexByName(graph, num, list->name);
            if (distance[index_u] < INT_MAX) {
                if (distance[index_u] + list->weight < distance[index_v]) {
                    distance[index_v] = distance[index_u] + list->weight;
                }
            }
            list = list->next;
        }
    }
}

/***************************************
 * DFS
 ***************************************/
typedef struct {
    Node** graph;
    int num;
    char* source;
    Stack* tpstack;
    int* first;
    int* finish;
    int counter;
    bool hasCycle;
} DFSData;

void runDFS(DFSData* data) {
    Node** graph = data->graph;
    int num = data->num;
    char* source = data->source;
    Stack* tpstack = data->tpstack;
    int* first = data->first;
    int* finish = data->finish;

    // O(E) = O(V * (V-1) / 2) = O(V^2)
    Stack* stack = createStack(num*num/2);

    // start from name
    push(stack, source);

    while (!isEmpty(stack)) {
        // printStack(stack);
        char* u = top(stack);
        int index = findIndexByName(graph, num, u);
        assert(index >= 0 && index < num);

        // visited already -> draw black color
        if (first[index] != NO_VAL) { 
            pop(stack);
            if (finish[index] == NO_VAL) {
                finish[index] = data->counter++;
                push(tpstack, u);
            }
        } 
        // first visit -> draw grey
        else { 
            first[index] = data->counter++;
            // printf("%s ", u);
            // each friend comes
            Node* v = graph[index]->next;
            while (v) {
                // printf("%s has a friend %s\n", u, v->name);
                int index_v = findIndexByName(graph, num, v->name);
                assert(index_v >= 0 && index_v < num);
                // printf("first(%s) == %d\n", v->name, first[index_v]);

                //check if it's cycle
                if (first[index_v] != NO_VAL && finish[index_v] == NO_VAL) {
                    printf("CYCLE\n");
                    data->hasCycle = true;
                }

                if (first[index_v] == NO_VAL) {
                    // printf("Enstack(%s)\n", v->name);
                    push(stack, v->name);
                }
                v = v->next;
            }
        }
    }
    destroyStack(stack);
}

/***************************************
 * Main
 ***************************************/
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
    printf("\n"); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    int num;
    fscanf(file, "%d", &num);    
    Node** arr = malloc(num * sizeof(Node*));
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
        if(ret == EOF) break;
        assert(ret == 3);
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
    fclose(file);
    file = NULL;

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
    Stack* tpstack = createStack(num);
    int* first = malloc(num * sizeof(int));
    int* finish = malloc(num * sizeof(int));
    for (i = 0; i < num; ++i) {
        finish[i] = first[i] = NO_VAL;
    }

    DFSData dfsData;
    dfsData.graph = arr;
    dfsData.num = num;
    dfsData.tpstack = tpstack;
    dfsData.first = first;
    dfsData.finish = finish;
    dfsData.counter = 0;
    dfsData.hasCycle = false;
    
    for (i = 0; i < num; ++i) {
        if (dfsData.first[i] == NO_VAL) {
            // printf("DFS start\n");
            dfsData.source = arr[i]->name;
            runDFS(&dfsData);
        }
    }

    // printf("%5s %2s %2s\n", "name", "st", "fi");
    // for (i = 0; i < num; ++i) {
    //     printf("%5s %2d %2d\n", arr[i]->name, first[i], finish[i]);
    // }

    if (!dfsData.hasCycle) {
        FILE* queryFile = fopen(argv[2], "r");
        if (!queryFile){
            fprintf(stderr, "Failed to open file \"%s\"\n", argv[2]);
            return EXIT_SUCCESS;
        }
        int* distance = malloc(num * sizeof(int));
        while (1) {
            ret = fscanf(queryFile, "%s", name);
            if (ret != 1) break;

            BellManFordData bellManFordData;
            bellManFordData.graph = arr;
            bellManFordData.num = num;
            bellManFordData.source = name;
            bellManFordData.tpstack = dfsData.tpstack;
            bellManFordData.distance = distance;

            runBellManFordAlgorithm(&bellManFordData);
            int i;
            for (i = 0; i < num; ++i) {
                if (distance[i] == INT_MAX) {
                    printf("%s INF\n", arr[i]->name);
                } else {
                    printf("%s %d\n", arr[i]->name, distance[i]);
                }
            }
            printf("\n");
        }
        free(distance);
        distance = NULL;
        fclose(queryFile);
        queryFile = NULL;
    }


    //////////////////////////////////////
    // free zone
    //////////////////////////////////////
    free(finish);
    free(first);
    destroyStack(tpstack);

    for (i = 0; i < num; ++i) {
        destroyList(arr[i]);
    }
    free(arr);
    return EXIT_SUCCESS;
}
