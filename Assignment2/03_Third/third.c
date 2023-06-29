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

typedef struct {
    char** array;
    int capacity;
    int head;
    int tail;
} Queue;

Queue* createQueue(int capacity) {
    Queue* temp = malloc(sizeof(Queue));
    temp->array = malloc((capacity+1) * sizeof(char*));
    temp->capacity = capacity;
    temp->head = 0;
    temp->tail = 0;
    return temp;
}

void freeQueue(Queue* queue) {
    free(queue->array);
    free(queue);
}

bool isEmpty(Queue* queue) {
    return queue->tail == queue->head;
}

bool isFull(Queue* queue) {
    return ( (queue->tail+1) % (queue->capacity+1) == queue->head );
}

void enqueue(Queue* queue, char* name) {
    assert(!isFull(queue));
    queue->array[queue->tail] = name;
    queue->tail++;
    queue->tail %= queue->capacity + 1;
}

void dequeue(Queue* queue) {
    assert(!isEmpty(queue));
    queue->head++;
    queue->head %= queue->capacity + 1;
}

char* front(Queue* queue) {
    if (isEmpty(queue)) return NULL;
    return queue->array[queue->head];
}

void printQueue(Queue* queue) {
    printf("--| Start of your lovely queue |--\n");
    int fakehead = queue->head;
    while (queue->tail != fakehead) {
        printf("%s ", queue->array[fakehead]);
        fakehead++;
    } 
    printf("\n");
    printf("--| End of your lovely queue |--\n");
}

void BFS(Node** arr, int num, char* name) {
    // init();
    bool* visited = malloc(num * sizeof(bool));
    int i;
    for (i = 0; i < num; ++i) {
        visited[i] = false;
    }

    Queue* queue = createQueue(num*num);

    // start from name
    enqueue(queue, name);

    while (!isEmpty(queue)) {
        // printQueue(queue);
        // first goes
        char* u = front(queue);
        dequeue(queue);

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
                // printf("Enqueue(%s)\n", v->name);
                enqueue(queue, v->name);
            }
            v = v->next;
        }
    }
    printf("\n");
    freeQueue(queue);
    free(visited);
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

    char name[NAME_LEN];
    while(1){
        ret = fscanf(queryFile, "%s", name);
        if (ret != 1) break;

        // printf("BFS starts from %s\n", name);
        BFS(arr, num, name);
        // printf("End BFS started from %s\n", name);
    }  

    for (i = 0; i < num; ++i) {
        freeList(arr[i]);
    }
    free(arr);
    return EXIT_SUCCESS;
}
