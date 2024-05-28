//
//  main.c
//  Assignment3a
//
//  Created by Alperen Yazmaci on 26.09.2023.
//

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Struct Aliases
typedef struct Customer Customer;

//struct Customer to make easier tracking if a customer gets popcorn or not
struct Customer{
    char customerName[20];
    int getsPopcorn;
};

//global variables declarations
int restrictionCount;
int customerCount;
Customer * customers;
char restrictions[10][2][20];

//takes in a string and int and creates a Customer struct with the information given
Customer createCustomer(char * customerName, int getsPopcorn){
    Customer customer;
    strcpy(customer.customerName, customerName);
    customer.getsPopcorn = getsPopcorn;
    return customer;
}

//takes in an array of Customers, two integers that are indexes. Then swaps the customers on those two indexes
void swapCustomers(Customer * array, int customer1, int customer2) {
    Customer temp = array[customer1];
    array[customer1] = array[customer2];
    array[customer2] = temp;
}

//returns 1 if the array is proper, returns 0 if the array is not proper
int checkArray(Customer * array){
    
    //loops throuhg all customers in the array
    for(int i = 0; i < customerCount; i++){
        Customer currentCustomer = customers[i];
        //following if, else if, and else statements returns 0 if a customer doesn't get popcorn nor any adjacent customer of that customer doesn't get popcorn, does not return anything if the customer or adjacent customer of that customer gets popcorn
        if(currentCustomer.getsPopcorn == 1){
            
        }else if((i > 0) && ((customers[i - 1].getsPopcorn) == 1)){
            
        }else if((i < customerCount - 1) && (customers[i + 1].getsPopcorn == 1)){
            
        }else{
            return 0;
        }
        
        //checks if the customer in index i and i-1 are allowed to sit next to each other, returns 0 if they are not allowed to sit next to each other, doesn't return anything if they are allowed
        if(i > 0){
            char * cusStr1 = customers[i].customerName;
            char * cusStr2 = customers[i-1].customerName;
            for(int j = 0; j < restrictionCount; j++){
                char * restrictedStr1 = restrictions[j][0];
                char * restrictedStr2 = restrictions[j][1];
                if( ( (strcmp(cusStr1, restrictedStr1)) == 0 || (strcmp(cusStr1, restrictedStr2)) == 0 ) && ( (strcmp(cusStr2, restrictedStr1)) == 0 || (strcmp(cusStr2, restrictedStr2)) == 0 ) ){
                    return 0;
                }
            }
        }
    }
    
    //returns 1 if none of the if or else statements returned 0
    return 1;
}

//recursive function that returns the number of possible permutations
int permute(Customer * array, int start, int end){
    
    //BASE CASE
    if (start == end) {
        return checkArray(array);
    }
    
    //RECURSIVE CASE
    else {
        int count = 0;
        for (int i = start; i <= end; i++) {
            swapCustomers(array, start, i);
            count += permute(array, start + 1, end);
            swapCustomers(array, start, i);
        }
        return count;
    }
    
}

int main(int argc, const char * argv[]) {
    
    //getting the first line input
    scanf("%d %d", &customerCount, &restrictionCount);
    
    //allocating memory for an array of Customers
    customers = calloc(customerCount, sizeof(Customer));
    
    //getting the customer inputs
    for(int i = 0; i < customerCount; i++){
        char tmp[20];
        int getsPopcorn;
        scanf("%s %d", tmp, &getsPopcorn);
        customers[i] = createCustomer(tmp, getsPopcorn);
    }

    //getting the restriction inputs
    for(int i = 0; i < restrictionCount; i++){
        char tmp0[20], tmp1[20];
        scanf("%s %s", tmp0, tmp1);
        strcpy(restrictions[i][0], tmp0);
        strcpy(restrictions[i][1], tmp1);
    }
    
    //print out the output and kickstarting the recursive permute() function
    printf("%d\n", permute(customers, 0, customerCount - 1));
    
    //free the allocated memory for the array of Customers
    free(customers);
    
    return 0;
}
