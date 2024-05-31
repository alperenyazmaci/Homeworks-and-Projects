//
//  main.c
//  Assignment5
//
//  Created by Alperen Yazmaci on 13.11.2023.
//

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 19

//Struct Definitions
typedef struct Customer {
    char name[MAXLEN + 1];
    int points;
} Customer;
typedef struct Treenode {
    Customer * cPtr;
    int size;
    struct Treenode * left;
    struct Treenode * right;
} Treenode;

// creates and allocates memory to a Customer * with the given int and char * then returns the Customer *
Customer * createCustomer(int points, char * name) {
    Customer * cus = (Customer *) malloc(sizeof(Customer));
    cus->points = points;
    strcpy(cus->name, name);
    return cus;
}

// creates and allocates memory to a Treenode * with the given int and char * then returns the Treenode *
Treenode * createTreenode(int points, char * name) {
    Treenode * ret = (Treenode *) malloc(sizeof(Treenode));
    ret->cPtr = createCustomer(points, name);
    ret->size = 1;
    ret->left = NULL;
    ret->right = NULL;
    return ret;
}

// Insert a node into a BST and return the resulting root
Treenode * insert(Treenode * root, int points, char * name, int contains) {
    // Base Case: empty tree
    if (root == NULL) {
        //print the name and points
        printf("%s %d\n", name, points);
        // Create the root and return it
        return createTreenode(points, name);
    }
    
    // Recursive
    if (strcmp(root->cPtr->name, name) == 0) {
        // value is already in the tree. Add the points to the already existing node
        root->cPtr->points += points;
        if (root->cPtr->points < 0) {
            root->cPtr->points = 0;
        }
        printf("%s %d\n", name, root->cPtr->points);
        return root;
    }
    if (strcmp(root->cPtr->name, name) > 0) {
        // root->cPtr->name is smaller (in ASCII) than name. The name goes into the left side of the tree
        root->size += contains;
        root->left = insert(root->left, points, name, contains);
    } else {
        // root->cPtr->name is greater (in ASCII) than name. The value goes into the right side
        root->size += contains;
        root->right = insert(root->right, points, name, contains);
    }
    // Return original root
    return root;
}


// Returns 1 if the Customer with the given values is in the binary search tree, returns 0 otherwise
int contains(Treenode * root, char * name) {
    // Base Case: Empty tree case
    if (root == NULL) {
        return 0;
    }
    if (strcmp(root->cPtr->name, name) == 0) {
        return 1;
    }
    
    if (strcmp(root->cPtr->name, name) < 0) {
        // root->cPtr->name is greater (in ASCII) than name
        return contains(root->right, name);
    }else{
        // root->cPtr->name is smaller (in ASCII) than name
        return contains(root->left, name);
    }
}

//Returns the Treenode if it exists, otherwise returns NULL
Treenode * find(Treenode *root, char * name){
    // Base Case: Empty tree case
    if (root == NULL) {
        return NULL;
    }
    if (strcmp(root->cPtr->name, name) == 0) {
        return root;
    }
    
    if (strcmp(root->cPtr->name, name) < 0) {
        // root->cPtr->name is greater (in ASCII) than name
        return find(root->right, name);
    }else{
        // root->cPtr->name is smaller (in ASCII) than name
        return find(root->left, name);
    }
}

// Returns 1 if the Customer with the given values is in the binary search tree, returns 0 otherwise
// Also increments the depth every recursion and passes the searched customers points to the int* points
int search(Treenode * root, char * name, int * depth, int * points) {
    // Base Case: Empty tree case
    if (root == NULL) {
        return 0;
    }
    if (strcmp(root->cPtr->name, name) == 0) {
        *points = root->cPtr->points;
        return 1;
    }
    
    if (strcmp(root->cPtr->name, name) < 0) {
        // root->cPtr->name is greater (in ASCII) than name
        (*depth)++;
        return search(root->right, name, depth, points);
    }else{
        // root->cPtr->name is smaller (in ASCII) than name
        (*depth)++;
        return search(root->left, name, depth, points);
    }
}

// Remove a value from a BST and return the resulting root of the tree
Treenode * delete(Treenode * root, int * i) {
    if(root == NULL){
        
    }
    if(*i == 0){ //check if the node that is going to be deleted has right or left child or both or neither
        (*i)++;
        if(root->left != NULL && root->right != NULL){ //the node that needs to be deleted has left and right children
            Treenode *tmp = delete(root->left, i);
            *(root->cPtr) = *(tmp->cPtr);
            free(tmp->cPtr);
            free(tmp);
        }else if(root->left != NULL){ //the node that needs to be deleted has only left child
            return root->left;
        }else if (root->right != NULL){ //the node that needs to be deleted has only right child
            return root->right;
        }else{ //the node that needs to be deleted does not have any children
            root = NULL;
        }
    }else{ //i is not 0 so we are searching for the maximum node in the left of the node to be deleted
        if(root->right->right != NULL){
            (*i)++;
            return delete(root->right, i);
        }else if(root->right != NULL){
            (*i)++;
            Treenode * tmp = root->right;
            root->right = NULL;
            return tmp;
        }else{
            return root;
        }
    }
    
    //returns the node that should be replaced with the node that is being deleted if it has not been already returned in the if statements
    return root;
}

//recalculates each nodes size value
int recalculateSize(Treenode * root){
    //BASE CASE
    if (root == NULL) {
        return 0;
    }

    // RECURSION
    int leftSize = recalculateSize(root->left);
    int rightSize = recalculateSize(root->right);

    // Update the current root's size
    root->size = leftSize + rightSize + 1;

    // Return the size of current root
    return root->size;
}

//returns the number of nodes that have smaller customer names (in ASCII) than the given name
int countSmaller(Treenode * root, char * name){
    //BASE CASE
    if(root == NULL || root->cPtr == NULL)
        return 0;
    if(strcmp(root->cPtr->name, name) == 0){
        if(root->left != NULL)
            return root->left->size;
        else
            return 0;
    }else if(strcmp(root->cPtr->name, name) < 0){
        int n = 1 + root->left->size;
        n += countSmaller(root->right, name);
        return n;
    }else{
        return countSmaller(root->left, name);
    }
}

//copies every root to a given Customer array
void copyCustomersToArray(Treenode * root, Customer * customers, int * i){
    if(root != NULL){
        customers[*i] = *(root->cPtr);
        (*i)++;
        copyCustomersToArray(root->left, customers, i);
        copyCustomersToArray(root->right, customers, i);
    }
}

//merge sorts the given customerArray with length n
void sortCustomersArray(Customer * customerArray, int n){
    // Base case
    if (n <= 1) return;
    // Compute half of the array
    int n2 = n / 2;
    sortCustomersArray(customerArray, n2);
    sortCustomersArray(customerArray + n2, n - n2);
    Customer * tmp = (Customer *) malloc(sizeof(Customer) * n);
    int fptr = 0; // front of the first half
    int bptr = n2; // front of the back half
    for (int i = 0; i < n; i++) {
        if (fptr == n2) {
            // front is empty
            tmp[i] = customerArray[bptr];
            bptr++;
        } else if (bptr == n) {
            // back is empty
            tmp[i] = customerArray[fptr];
            fptr++;
        } else if (customerArray[fptr].points < customerArray[bptr].points) {
            // front was smaller than the back
            tmp[i] = customerArray[fptr];
            fptr++;
        } else {
            // back was good enough
            tmp[i] = customerArray[bptr];
            bptr++;
        }
    }
    // Move the temp values into the original array
    for (int i = 0; i < n; i++) {
        customerArray[i] = tmp[i];
    }
    // Free the temp array
    free(tmp);
}

//prints customers in the customerArray from last to first
void printCustomers(Customer * customerArray, int n){
    for(int i = n-1; i >= 0; i--){
        printf("%s %d\n", customerArray[i].name, customerArray[i].points);
    }
}

// Frees all the memory that the given root's Treenode is using
void freeTreenode(Treenode * root){
    if(root != NULL){
        freeTreenode(root->left);
        freeTreenode(root->right);
        free(root->cPtr);
        free(root);
    }
}

int main(int argc, const char * argv[]) {
    
    int n;
    Treenode *root = NULL;
    
    //getting the first line input
    scanf("%d", &n);
    
    for(int i = 0; i < n; i++){
        char command[MAXLEN + 1], name[MAXLEN + 1];
        int points;
        int depth = 0;
        
        scanf("%s", command);
        
        //input command is add
        if(strcmp(command, "add") == 0){
            scanf("%s %d", name, &points);
            if(contains(root, name)){
                //do not increment the sizes if the value already exists since there is not going to be an insert
                root = insert(root, points, name, 0);
            }else{
                //increment the sizes if the value doesn't exist since there is going to be an insert
                root = insert(root, points, name, 1);
            }
        }
        //input command is sub
        else if(strcmp(command, "sub") == 0){
            scanf("%s %d", name, &points);
            if(contains(root, name)){ //if the Customer exists
                root = insert(root, (-1) * points, name, 0);
            }else{ //if the Customer doesn't exist
                printf("%s not found\n", name);
            }
        }
        //input command is del
        else if(strcmp(command, "del") == 0){
            scanf("%s", name);
            Treenode * tmp = find(root, name);
            int i = 0;
            if(contains(root, name)){
                *(tmp) = *(delete(tmp, &i));
                root->size = recalculateSize(root);
                printf("%s deleted\n", name);
            }else{
                printf("%s not found\n", name);
            }
        }
        //input command is search
        else if(strcmp(command, "search") == 0){
            scanf("%s", name);
            if(search(root, name, &depth, &points)){ // customer with the input name exists
                printf("%s %d %d\n", name, points, depth);
            }else{ // customer with the input name doesn't exist
                printf("%s not found\n", name);
            }
        }
        //input command is count_smaller
        else if(strcmp(command, "count_smaller") == 0){
            scanf("%s", name);
            printf("%d\n", countSmaller(root, name));
        }
        //input command is not a proper command
        else{
            i--;
        }
    }
    
    Customer * customerArray = (Customer *) malloc(sizeof(Customer) * root->size);
    int i = 0;
    //copies every treenode's customer pointer into customerArray
    copyCustomersToArray(root, customerArray, &i);
    //merge sorts customers according to their points
    sortCustomersArray(customerArray, root->size);
    //loops through the array to check if there is any customers with same points, if there is any it sorts them alphabetically
    for(int i = 0; i < root->size - 1; i++){
        if(customerArray[i].points == customerArray[i + 1].points){
            if(strcmp(customerArray[i].name, customerArray[i + 1].name) < 0){
                Customer tmp = customerArray[i];
                customerArray[i] = customerArray[i + 1];
                customerArray[i + 1] = tmp;
            }
        }
    }
    //prints the customerArray
    printCustomers(customerArray, root->size);
    
    //free the memory
    free(customerArray);
    freeTreenode(root);
    
    return 0;
}
