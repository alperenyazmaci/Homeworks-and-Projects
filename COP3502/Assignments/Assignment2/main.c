//
//  main.c
//  Assignment2
//
//  Created by Alperen Yazmaci on 21.09.2023.
//

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Struct Aliases
typedef struct Customer Customer;
typedef struct Node Node;
typedef struct List List;
typedef struct Queue Queue;

//Struct Definitions
struct Customer{
    char * name;
    int numTickets;
    int lineNum;
    int arrivalTime;
};
struct Node{
    Customer * customer;
    Node * next;
};
struct List{
    Node * head;
    Node * tail;
    int numNodes;
};
struct Queue{
    List * list;
};

Customer * createCustomer(char * name, int numTickets, int lineNum, int arrivalTime){
    //create a Customer pointer and allocate memory
    Customer * customer = (Customer *) malloc(sizeof(Customer));
    
    //put in the values to customer
    customer->name = strdup(name);
    customer->numTickets = numTickets;
    customer->lineNum = lineNum;
    customer->arrivalTime = arrivalTime;
    
    //return created customer
    return customer;
}

void deleteCustomer(Customer * customer){
    //free customer->name and free customer
    free(customer->name);
    free(customer);
}

Customer createCustomerNoAlloc(char * name, int numTickets, int lineNum, int arrivalTime){
    Customer customer;
    
    //put in the values to customer
    customer.name = strdup(name);
    customer.numTickets = numTickets;
    customer.lineNum = lineNum;
    customer.arrivalTime = arrivalTime;
    
    //return created customer
    return customer;
}

void deleteCustomerNoAlloc(Customer customer){
    free(customer.name);
}

Node * createNode(char * name, int numTickets, int lineNum, int arrivalTime){
    // allocate memory for a new node in your list and
    // fill the node with the data passed into our createNode function
    Node * res = (Node *) malloc(sizeof(Node));
    res->customer = createCustomer(name, numTickets, lineNum, arrivalTime);
    res->next = NULL;
    return res;
}

List * createList(){
    // allocate memory for a new list and set the head& tail to null
    List * res = (List *) malloc(sizeof(List));
    res->head = res->tail = NULL;
    return res;
}

Queue createQueue(){
    Queue result;
    result.list = createList();
    return result;
}

void insertTail(List * list, char * name, int numTickets, int lineNum, int arrivalTime){
    if (!list) return; // if the list doesnt exist, return
    Node * node = createNode(name, numTickets, lineNum, arrivalTime); // create a node with passed in value
    if (list->tail){
        list->tail->next = node; // add the new node to our old tail's next
        list->tail = node; // update the list's tail to point to the new tail
    } else{
        list->tail = list->head = node; // if list was empty, set the new node to the tail AND head
    }
    list->numNodes++;
}

void removeHead(List * list){
    if (!list || !list->head) return; // if there is no list or no list head, then stop! or segfault
    if (list->head == list->tail) {// if the head and tail are the same node
        deleteCustomer(list->head->customer);
        free(list->head); // free the head, which effectively also removes the tail
        list->head = list->tail = NULL; // update the list to tell us we no longer have head or tail
        return;
    }
    Node * newHead = list->head->next; // set cur head's next to be the new head
    deleteCustomer(list->head->customer);
    free(list->head); // now we can free our old head
    list->head = newHead; // have list's head point to our newHead
}

void dequeue(Queue * queue){
    // queues work by first in first out so we need to remove the node
    // that was entered first, which was the head
    if (queue){
        removeHead(queue->list); // so if our queue exists, we call to remove the head
        queue->list->numNodes--;
    }
}

void enqueue(Queue * queue, char * name, int numTickets, int lineNum, int arrivalTime){
    // because queues work first in first out, we need to add any
    // new node to the back of the line. kinda like a line at disney
    if (queue){
        insertTail(queue->list, name, numTickets, lineNum, arrivalTime); // so if a queue exists, insert the new value at the end of it
    }
}

//gets the front value of the queue
Customer * peek(Queue * queue){
    return queue->list->head->customer; // return the data inside the head of the list
}

//returns 1 if queue is empty, returns 0 otherwise
int empty(Queue queue){
    if(queue.list->numNodes==0){
        return 1;
    }else{
        return 0;
    }
}

//returns the size of the queue
int size(Queue queue){
    return queue.list->numNodes;
}

void deleteList(List * l)
{
    while (l->head) { // while list has a head, we will call the removeHead function
        removeHead(l);
    }
    free(l); // we will free the list after all it's contents are removed, so we dont leak memory
}

void deleteQueue(Queue queue){
    deleteList(queue.list); // we will free the list associated with the queue
}

int getCharNum(char letter){
    for(int c = 'A'; c <= 'Z'; c++){
        if(letter == c){
            return (c - 65) % 13;
        }
    }
    return -1;
}

void printCustomer(Customer * customer){
    printf("Customer: name{%s}, numTickets{%d}, lineNum{%d}, arrivalTime{%d}\n", customer->name, customer->numTickets, customer->lineNum, customer->arrivalTime);
}

void printQueue(Queue * q){
    Node * tmp = q->list->head;
    while(tmp){
        printCustomer(tmp->customer);
        tmp = tmp->next;
    }
}


int main(int argc, const char * argv[]) {
    
    //getting the first line input
    int numCustomers, numBooths;
    scanf("%d %d", &numCustomers, &numBooths);scanf("%d %d", &numCustomers, &numBooths);
    
    //creating queues array and allocating memory for 12 queues
    Queue * queues = (Queue *) calloc(12, sizeof(Queue));
    //creating each of the 12 queues
    for(int i = 0;i < 12; i++){
        queues[i] = createQueue();
    }
    
    //getting the rest of the inputs and creating a customer according to the inputs
    for(int i = 0; i < numCustomers; i++){
        char* tmp = calloc(60, sizeof(char));
        int tmpTickets, tmpTime;
        scanf("%s %d %d", tmp, &tmpTickets, &tmpTime);
        
        int numQueue = getCharNum(tmp[0]);
        if(numQueue == 0){
            int allEmpty = 1;
            int queueNum = 0;
            int j = 0;
            while(allEmpty && j < 12){
                if(!empty(queues[j])){
                    queueNum = j;
                    allEmpty = 0;
                }
                j++;
            }
            if(!allEmpty){
                for(int k = 0; k < 12; k++){
                    if( size(queues[k]) != 0 && size(queues[k]) < size(queues[queueNum]) ){
                        queueNum = k;
                    }
                }
            }
            enqueue(&queues[queueNum], tmp, tmpTickets, queueNum, tmpTime);
        }else{
            enqueue(&queues[numQueue - 1], tmp, tmpTickets, numQueue, tmpTime);
        }
        free(tmp);
    }
    
    //getting the number of non empty queues
    int numOfNonEmptyQueues = 0;
    for(int i = 0; i < 12; i++){
        if(empty(queues[i])){
            numOfNonEmptyQueues++;
        }
    }
    
    //calculating how many queues each booth is going to have
    int remainder = numOfNonEmptyQueues % numBooths;
    int quotient = numOfNonEmptyQueues / numBooths;

    int lastQueue = 0;
    for(int i = 0; i < numBooths; i++){
        if(i==0)
            printf("Booth %d", i + 1);
        else
            printf("\nBooth %d", i + 1);
        
        if(remainder == 0){
            int count = 0;
            int queueNum[quotient];
            for(int j = lastQueue; j < 12; j++){
                if(count < quotient){
                    if(!empty(queues[j])){
                        queueNum[count] = j;
                        count++;
                    }
                    lastQueue = j + 1;
                }
            }
            
        }else{
            int count = 0;
            int queueNum[quotient + remainder];
            for(int j = lastQueue; j < 12; j++){
                if(count < quotient + remainder){
                    if(!empty(queues[j])){
                        queueNum[count] = j;
                        count++;
                    }
                    lastQueue = j + 1;
                }
            }
        }
    }
    
    
    //freeing memory allocated pointers inside of queues array
    for(int i = 0; i < 12; i++){
        printf("\n\nQueue %d: \n", i + 1);
        printQueue(&queues[i]);
        deleteQueue(queues[i]);
    }
    //freeing the memory allocated arrays
    free(queues);
    
    return 0;
}
