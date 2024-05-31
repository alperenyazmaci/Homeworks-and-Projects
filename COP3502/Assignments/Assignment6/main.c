//
//  main.c
//  Assignment6
//
//  Created by Alperen Yazmaci on 22.11.2023.
//

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Defined values
#define MAXLEN 19
#define TABLESIZE 300007

//Struct Definitions
typedef struct item {
    char name[MAXLEN+1];
    int quantity;
    int saleprice;
} item;
typedef struct node {
    item* iPtr;
    struct node* next;
} node;
typedef struct hashtable {
    node** lists;
    int size;
} hashtable;

//a hash function that takes in a string and size of that string, returns an int
int hashfunc(char* word, int size) {
    int len = strlen(word);
    int res = 0;
    for (int i=0; i<len; i++)
    res = (1151*res + (word[i]-'a'))%size;
    return res;
}

//a function that takes in a hashtable as input and fills that hashtable with NULL values
void initializeHashTable(hashtable *hashTable) {
    hashTable->lists = (node **)malloc(TABLESIZE * sizeof(node *));
    hashTable->size = TABLESIZE;

    for (int i = 0; i < TABLESIZE; i++) {
        hashTable->lists[i] = NULL;
    }
}

//a function that takes in a hashtable, item, int * as input and inserts the given item into the hashtable and increments the int * as according to the complexity of that insertion
void insertItem(hashtable *hashTable, item *newItem, int *complexity) {
    int index = hashfunc(newItem->name, hashTable->size);

    node *newNode = (node *)malloc(sizeof(node));
    newNode->iPtr = newItem;
    newNode->next = NULL;

    if (hashTable->lists[index] == NULL){ //the list is empty, insert the new node as the first node
        hashTable->lists[index] = newNode;
        (*complexity) += 1;
    }else{ //iterate over the list to find the end of linked list and insert the new node at the end
        node *current = hashTable->lists[index];
        (*complexity) += 1;
        while (current->next != NULL) {
            current = current->next;
            (*complexity) += 1;
        }
        current->next = newNode;
    }
}

//a function that takes in a hashtable, string, int* and searches the item with the given string then returns null if the item does not exist, and returns the item if it exists. increments the int * according to the complexity of that search
node *searchItem(hashtable *hashTable, char *itemName, int *complexity) {
    int index = hashfunc(itemName, hashTable->size);

    node *current = hashTable->lists[index];
    (*complexity) += 1;

    while (current != NULL) {
        if (strcmp(current->iPtr->name, itemName) == 0) {
            return current;
        }
        current = current->next;
        (*complexity) += 1;
    }

    return NULL;
}

int main(int argc, const char * argv[]) {
    
    //initialize the hash table
    hashtable inventory;
    initializeHashTable(&inventory);
    
    //initialize the needed integers
    int numberOfCommands;
    int sumOfComplexity = 0;
    int totalCash = 100000;
    
    //getting the first line input
    scanf("%d", &numberOfCommands);
    
    for(int i = 0; i < numberOfCommands; i++){
        char command[MAXLEN + 1], itemName[MAXLEN + 1];
        
        //get the command and item input so we can get the rest of the inputs according to the command
        scanf("%s %s", command, itemName);
        
        //input command is buy
        if(strcmp(command, "buy") == 0){
            int quantity, totalPrice;
            //get the rest of the command
            scanf("%d %d", &quantity, &totalPrice);
            
            //check to see if the item exists
            int tmp = sumOfComplexity;
            node *existingItem = searchItem(&inventory, itemName, &sumOfComplexity);
            sumOfComplexity = tmp;
            
            if (existingItem == NULL) { //item doesn't exist, create a new item
                item *newItem = (item *)malloc(sizeof(item));
                strcpy(newItem->name, itemName);
                newItem->quantity = quantity;
                
                //insert the new item into the hash table
                insertItem(&inventory, newItem, &sumOfComplexity);
                tmp = sumOfComplexity;
                existingItem = searchItem(&inventory, itemName, &sumOfComplexity);
                sumOfComplexity = tmp;
            } else { //item exists, update quantity and price of the existing item
                existingItem = searchItem(&inventory, itemName, &sumOfComplexity);
                existingItem->iPtr->quantity += quantity;
            }

            //calculate the totalCash left
            totalCash -= totalPrice;
            
            //print output for buy command
            printf("%s %d %d\n", itemName, existingItem->iPtr->quantity, totalCash);
            
            
        }
        //input command is sell
        else if(strcmp(command, "sell") == 0){
            int quantity;
            //get the rest of the command
            scanf("%d", &quantity);
            
            node *existingItem = searchItem(&inventory, itemName, &sumOfComplexity);
            
            //update quantity in inventory
            existingItem->iPtr->quantity -= quantity;
            
            //calculate the totalCash left
            totalCash += existingItem->iPtr->saleprice * quantity;
            
            //print output for sell command
            printf("%s %d %d\n", itemName, existingItem->iPtr->quantity, totalCash);
            
            
        }
        //input command is change_price
        else if(strcmp(command, "change_price") == 0){
            int newPrice;
            //get the rest of the command
            scanf("%d", &newPrice);
            
            
            //search for the item in hashtable
            node *existingItem = searchItem(&inventory, itemName, &sumOfComplexity);

            //update sale price of the item
            existingItem->iPtr->saleprice = newPrice;
            
            
        }
        //input command is not a proper command
        else{
            i--;
        }
        
    }
    
    //print out the final output
    printf("%d\n%d\n", totalCash, sumOfComplexity);
    
    
    return 0;
}
