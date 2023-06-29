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
 * PriorityQueue
 ***************************************/
typedef struct {
    int index;
    int distance;
} PriorityQueueElement;

typedef struct {
    PriorityQueueElement* array;
    int capacity;
    int tail;
} PriorityQueue;

PriorityQueue* createPriorityQueue(int capacity) {
    PriorityQueue* temp = malloc(sizeof(PriorityQueue));
    temp->array = malloc(capacity * sizeof(PriorityQueueElement));
    temp->capacity = capacity;
    temp->tail = 0;
    return temp;
}

void destroyPriorityQueue(PriorityQueue* pq) {
    free(pq->array);
    free(pq);
}

bool isEmpty(PriorityQueue* pq) {
    return pq->tail == 0;
}

bool isFull(PriorityQueue* pq) {
    return pq->tail == pq->capacity;
}

void push(PriorityQueue* pq, int index, int distance) {
    assert(!isFull(pq));
    pq->array[pq->tail].index = index;
    pq->array[pq->tail].distance = distance;
    pq->tail++;
}

void swap(PriorityQueueElement* a, PriorityQueueElement* b) {
    int tmp = a->index;
    a->index = b->index;
    b->index = tmp;
    
    tmp = a->distance;
    a->distance = b->distance;
    b->distance = tmp;
}

void extractMin(PriorityQueue* pq, PriorityQueueElement* ret) {
    assert(!isEmpty(pq));
    // find the minimal
    int i = 0;
    int min_i = 0;
    for (i = 1; i < pq->tail; ++i) {
        if (pq->array[i].distance < pq->array[min_i].distance){
            min_i = i;
        }
    }
    // swap with the last element
    swap(&pq->array[pq->tail - 1], &pq->array[min_i]);

    ret->index = pq->array[pq->tail - 1].index;
    ret->distance = pq->array[pq->tail - 1].distance;

    // tail--
    pq->tail--;
}

void printPriorityQueue(PriorityQueue* pq) {
    printf("--| Start of your lovely pq |--\n");
    int fakehead = 0;
    while (pq->tail != fakehead) {
        printf("(%d:%d)", pq->array[fakehead].index, pq->array[fakehead].distance);
        fakehead++;
    } 
    printf("\n");
    printf("--| End of your lovely pq |--\n");
}

/***************************************
 * Dijkstra
 ***************************************/
// Dijkstra
typedef struct {
    Node** graph;
    int numV;
    int numE;
    char* source;
    int* distance;
} DijkstraData;

void runDijkstra(DijkstraData* data) {
    Node** graph = data->graph;
    int numV = data->numV;
    int numE = data->numE;
    char* source = data->source;
    int* distance = data->distance;

    PriorityQueue* priorityQueue = createPriorityQueue(numE);

    int i;
    for (i = 0; i < numV; ++i) {
        distance[i] = INT_MAX;
    }
    
    int index = findIndexByName(graph, numV, source);
    distance[index] = 0;
    push(priorityQueue, index, distance[index]);
    
    while (!isEmpty(priorityQueue)) {
        // next closest step
        PriorityQueueElement u;
        // printPriorityQueue(priorityQueue);
        extractMin(priorityQueue, &u);
        int index_u = u.index;
        // printPriorityQueue(priorityQueue);

        // iterate through every friends of u
        Node* list = graph[index_u]->next;
        while (list) {
            int index_v = findIndexByName(graph, numV, list->name);
            assert(distance[index_u] != INT_MAX);

            // update the distance if it's better
            int newDistance = distance[index_u] + list->weight;
            if (newDistance < distance[index_v]) {
                distance[index_v] = newDistance;
                push(priorityQueue, index_v, distance[index_v]);
            }

            list = list->next;
        }
    }

    destroyPriorityQueue(priorityQueue);
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
    int numE = 0;
    while(1){
        ret = fscanf(file, "%s %s %d", name1, name2, &weight);
        if(ret == EOF) break;
        assert(ret == 3);
        numE++;
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

    FILE* queryFile = fopen(argv[2], "r");
    if (!queryFile){
        fprintf(stderr, "Failed to open file \"%s\"\n", argv[2]);
        return EXIT_SUCCESS;
    }
    int* distance = malloc(num * sizeof(int));
    while (1) {
        ret = fscanf(queryFile, "%s", name);
        if (ret != 1) break;

        DijkstraData dijkstraData;
        dijkstraData.graph = arr;
        dijkstraData.numV = num;
        dijkstraData.numE = numE;
        dijkstraData.source = name;
        dijkstraData.distance = distance;

        runDijkstra(&dijkstraData);
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

    //////////////////////////////////////
    // free zone
    //////////////////////////////////////
    for (i = 0; i < num; ++i) {
        destroyList(arr[i]);
    }
    free(arr);
    return EXIT_SUCCESS;
}
