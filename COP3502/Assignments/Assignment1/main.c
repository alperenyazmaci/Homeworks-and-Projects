//
//  main.c
//  Assignment1
//
//  Created by Alperen Yazmaci on 8.09.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INPUT_SIZE 32
#define ROW_SIZE 100000
#define ROW_NUMBER 100000

typedef struct Reservation Reservation;
typedef struct Row Row;
typedef struct Theater Theater;

struct Reservation {
    int start, end; // Start and end of the reservation
    char * name; // Name of the person who reserved this range
};

struct Row {
    Reservation * array; // The array of reservations
    int max_reservations; // the spots in the array
    int num_reservations; // occupied spots in the array
};

struct Theater {
    Row * row; // The array of rows in the theater
};

void initializeRow(Row * initializedRow){
    initializedRow->array = NULL; //initializes reservation array to be null
    initializedRow->max_reservations = ROW_SIZE;//initializes max_reservations to be the size of defined value ROW_SIZE
    initializedRow->num_reservations = 0;//initializes num_reservations to be 0
}

void freeRow(Row * row){
    for(int i = 0; i < row->num_reservations; i++){ //loops through all the reservations
        free(row->array[i].name);//frees the name in the Reservation array
    }
    if(row->array != NULL)//frees Reservation array if it exists and was allocated memory
        free(row->array);
}

void createReservation(Reservation * res, int start, int end, char * name){
    res->start = start;//initializes start
    res->end = end;//initializes end
    res->name = strdup(name);//initializes name
}

char* lookup(char * input, Theater * theater, int rowNum, int seatNum){
    if(seatNum < 0 || rowNum < 0){
        char buf[INPUT_SIZE];//char array to be buffer for input and token
        strcpy(buf, input);//copying input into buf
        char * token;//creating a char pointer for tokenizing
        
        token = strtok(buf, " ");//get the first token
        
        int i = 0; //creating a variable to count which word the while loop is on in the input
        while (token) {
            token = strtok(NULL, " ");//getting and passing the next word into token using strtok()
            if(i == 0)//assigning token to rowNum if token is the first token
                sscanf(token, "%d", &rowNum);
            if(i == 1)//assigning token to seatNum if token is the second token
                sscanf(token, "%d", &seatNum);
            i++;//incrementing i for the next loop cycle
        }
    }
    
    //decrementing to synchronize user's input with c arrays. rowNum and seatNum because user's row start at 1, but arrays in c start at 0
    rowNum--;
    seatNum--;
    
    // checking if num reservation is equal to 0 to avoid unnecessary processing
    if(theater->row[rowNum].num_reservations == 0){
        return "No one";
    }else{
        for(int i = 0; i < theater->row[rowNum].num_reservations; i++){ //looping through ever reservation
            //checking if the seatNum in the rowNum is already reserved and returning name of the reservation if it is
            if( (seatNum >= theater->row[rowNum].array[i].start) && (seatNum <= theater->row[rowNum].array[i].end) ){
                return theater->row[rowNum].array[i].name;
            }
        }
    }
    
    //returning no one since seatNum in the rowNum is not reserved
    return "No one";
    
}

int makePurchase(char * input, Theater * theater){
    //creating needed variables
    int rowNum, start, end;
    char * name;
    
    char buf[INPUT_SIZE];//char array to be buffer for input and token
    strcpy(buf, input);//copying input into buf
    char * token;//creating a char pointer for tokenizing
    
    token = strtok(buf, " ");//get the first token
    
    int i = 0; //creating a variable to count which word the while loop is on in the input
    while (token) {
        token = strtok(NULL, " ");//getting and passing the next word into token using strtok()
        if(i == 0)//assigning token to rowNum if token is the first token
            sscanf(token, "%d", &rowNum);
        if(i == 1)//assigning token to start if token is the second token
            sscanf(token, "%d", &start);
        if(i == 2)//assigning token to end if token is the third token
            sscanf(token, "%d", &end);
        if(i == 3)//assigning token to name using strdup() if token is the fourth token
            name = strdup(token);
        i++;//incrementing i for the next loop cycle
    }
    
    //looping from start to end to return 0 (FAILURE) if the seat is reserved
    for(int j = start; j <= end; j++){
        //checking if the seat is reserved
        if( (strcmp(lookup(input, theater, rowNum, j), "No one")) != 0 )
            return 0;
    }
    
    
    //decrementing to synchronize user's input with c arrays. rowNum, start, end because user's row start at 1, but arrays in c start at 0
    rowNum--;
    start--;
    end--;
    
    if(theater->row[rowNum].num_reservations > 0){ //checking if there is any reservations
        //decrementing the seat amount that is going to be reserved
        theater->row[rowNum].max_reservations =  theater->row[rowNum].max_reservations - (end - start + 1);
        //incrementing the reservations
        theater->row[rowNum].num_reservations++;
        //reallocating memory to Reservation array
        theater->row[rowNum].array = (Reservation *) realloc(theater->row[rowNum].array, (theater->row[rowNum].num_reservations + 1) * sizeof(Reservation));
        //creating reservation using createReservation()
        createReservation(&theater->row[rowNum].array[theater->row[rowNum].num_reservations -1], start, end, name);
        
        return 1;//returning 1(SUCCESS)
    }else{
        //decrementing the seat amount that is going to be reserved
        theater->row[rowNum].max_reservations =  theater->row[rowNum].max_reservations - (end - start + 1);
        //incrementing the reservations
        theater->row[rowNum].num_reservations = 1;
        //allocating memory to Reservation array which was null before
        theater->row[rowNum].array = (Reservation *) malloc(sizeof(Reservation));
        //creating reservation using createReservation()
        createReservation(&theater->row[rowNum].array[theater->row[rowNum].num_reservations -1], start, end, name);
        
        return 1;//returning 1(SUCCESS)
    }
    
    //returning 0(FAILURE)
    return 0;
    
}


char * takeInput(){
    char * input = (char *) calloc(INPUT_SIZE, sizeof(char));//memory allocating to input for INPUT_SIZE
    fgets(input, INPUT_SIZE, stdin);//taking input from the user into the variable input
    input[strcspn(input, "\r\n")] = 0;//removes the line break at the end of the input
    return input;//returning input
}

char * getFirstWordInput(char * input){
    
    char buf[INPUT_SIZE];//char array to be buffer for input and token
    strcpy(buf, input);//copying input into buf
    char * token;//creating a char pointer for tokenizing
    
    token = strtok(buf, " ");//get the first token
    
    char * inputFirstWord = strdup(token);//string duplicating token to inputFirstWord to return it
    
    return inputFirstWord;//returns the first word of the input
    
}

int main(int argc, const char * argv[]) {
    
    //creating a Theater struct and allocating memory for it
    Theater *theater = (Theater *) malloc(sizeof(Theater));
    //allocating memory for the array in theater
    theater->row = (Row *) calloc(ROW_NUMBER, sizeof(Row));
    //looping through all the rows to initialize them using initializeRow()
    for(int i = 0; i < ROW_NUMBER; i++){
        initializeRow(&theater->row[i]);
    }
    
    
    int purchaseSuccess;//int for assigning makePurchase() return value to print "SUCCESS" or "FAILURE" depending on the value
    bool quit = false;//boolean that will store true if the input is "QUIT", stores false if the input is not "QUIT"
    char * input;//char pointer for input to be stored in
    char * inputFirstWord;//char pointer for input's first word to be stored in
    
    while(!quit){ //goes through the loop if quit = false
        
        input = takeInput();//taking input
        inputFirstWord = getFirstWordInput(input);//getting first word of the input
        if(strcmp(inputFirstWord, "QUIT") == 0){ //checking if first word is "QUIT"
            quit = true;//passing true to quit to make loop end
        }else if(strcmp(inputFirstWord, "LOOKUP") == 0){ //checking if first word is "LOOKUP"
            quit = false;//passing false to quit to make loop continue
            (printf)("%s\n", lookup(input, theater, -1, -1));
        }else if(strcmp(inputFirstWord, "BUY") == 0){ //checking if first word is "BUY"
            quit = false;//passing false to quit to make loop continue
            purchaseSuccess = makePurchase(input, theater);//calling makePurchase function and passing the resulting int value into purchaseSuccess
            if(purchaseSuccess == 1){
                printf("SUCCESS\n");
            }else if(purchaseSuccess == 0){
                printf("FAILURE\n");
            }
        }
        
        //freeing input and inputFirstWord before starting a new loop cycle or ending the loop
        free(input);
        free(inputFirstWord);
        
        
    }
    
    //looping through all the rows and calling freeRow() on each row to free all the memory allocated
    for(int i = 0; i < ROW_NUMBER; i++){
        freeRow(&theater->row[i]);
//        free(theater->row[i].array);
    }
    //freeing row array in the theater
    free(theater->row);
    //freeing theater
    free(theater);
    
    return 0;//quitting the program
    
}
