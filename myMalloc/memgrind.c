#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>
#include "mymalloc.h"
#include <string.h>
 
void initializeHeap(void);

//used for test D
typedef struct Node {
    int value;
    struct Node *next;
} Node;

//used for test E
typedef struct TreeNode {
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode *create(int value) {
    TreeNode *node = malloc(sizeof(TreeNode));
    if (node) {
        node->value = value;
        node->left = node->right = NULL;
    }
    return node;
}

void freetree(TreeNode *root) {
    if (root != NULL) {
        freetree(root->left);
        freetree(root->right);
        free(root);
    }
}

void insert(TreeNode **root, int val) {
    if ((*root) == NULL) {
        *root = create(val);
    } else if (val < (*root)->value) {
        insert(&((*root)->left), val);
    } else {
        insert(&((*root)->right), val);
    }
}

//workload function with tests
void workload() {

    printf("Starting workload./n");
    // Test A: malloc() and free() a 1-byte object 120 times
    for (int i = 0; i < 120; i++) {
        void *ptr = malloc(1); //calling malloc for 1 byte 120 times
        free(ptr); //deallocating the byte
    }

    // Test B: malloc 120 1-byte objects, store the pointers in the array, then use free
    char *arr[120];
    for (int i = 0; i < 120; i++) {
        arr[i] = malloc(1); // continuously allocates 1 byte
    }
    for (int i = 0; i < 120; i++) {
        free(arr[i]); // continuously frees 1 byte at a time in the stored array
    }

    // Test C: Randomly allocate and deallocate
    void* ptrArray[120] = {NULL};  // Array of 120 pointers, initialized to NULL
    int allocations = 0;            // Total number of allocations performed
    int activeAllocations = 0;      // Number of currently allocated pointers
    srand(time(NULL));  // Seed random number generator

    while (allocations < 120) {
        if (activeAllocations == 0 || (rand() % 2 == 0 && activeAllocations < 120)) {
            // Allocate memory and store pointer in the first empty slot
            for (int i = 0; i < 120; i++) {
                if (ptrArray[i] == NULL) {
                    ptrArray[i] = malloc(1);  // Allocate 1-byte object
                    if (ptrArray[i] == NULL) {
                        fprintf(stderr, "Memory allocation failed!\n");
                        return;
                    }
                    allocations++;
                    activeAllocations++;
                    break;
                }
            }
        } else {
            // Deallocate a randomly chosen allocated object
            int index;
            do {
                index = rand() % 120;
            } while (ptrArray[index] == NULL);  // Ensure the pointer is not already freed

            free(ptrArray[index]);  // Free the allocated memory
            ptrArray[index] = NULL;  // Set the pointer to NULL to mark it as freed
            activeAllocations--;  // Decrease the count of active allocations
        }
    }

    // Deallocate any remaining allocated objects
    for (int i = 0; i < 120; i++) {
        if (ptrArray[i] != NULL) {
            free(ptrArray[i]);
            ptrArray[i] = NULL;
        }
    }

    // Test D: Linked List - Create a list of 10 nodes and free them all
    Node *current = NULL;
    Node *head = NULL;
    for (int i = 0; i < 10; i++) {
        Node *node = malloc(sizeof(Node));
        if (node) {
            node->value = i;
            node->next = NULL;
            if (head == NULL) {
                head = node;
            } else {
                current->next = node;
            }
            current = node;
        }
    }

    current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }

    // Test E: Binary Tree - Insert 10 nodes into the BST and free all the nodes
    TreeNode *root = NULL;
    for (int i = 0; i < 10; i++) {
        insert(&root, i);
    }

    freetree(root);

}

//main function with timing logic
int main() {

    printf("main initialized");
    initializeHeap(); //initialize custom heap

    long total = 0; //initialize total time to 0
    for (int i = 0; i < 50; i++) {
        struct timeval startTime, endTime;
        long elapsed; 

        gettimeofday(&startTime, NULL);
        workload(); //run the workload
        gettimeofday(&endTime, NULL);

        //calculate elapsed time in ms
        elapsed = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
        total += elapsed; //accumulate elapsed time
    }

    //print the average time over 50 runs
    printf("Average time for 50 runs was %ld microseconds.\n", (total / 50));
    printf("End workload.\n");

    return EXIT_SUCCESS;
}

