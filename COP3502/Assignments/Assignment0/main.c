//
//  main.c
//  Assignment1
//
//  Created by Alperen Yazmaci on 24.08.2023.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, const char * argv[]) {
    //creating the pointers for first and second inputs and allocating them memory
    char* first_input = (char*) malloc(100001 * sizeof(char));
    char* second_input = (char*) malloc(100001 * sizeof(char));
    
    //creating the integer arrays and allocating them memory that are going to be used to count how many of each letter the inputs have
    int* first_array = (int*) malloc(27*sizeof(int));
    int* second_array = (int*) malloc(27*sizeof(int));
    
    //creating the integer we are going to use to calculate the output
    int output;
    //creating the integer that we are going to store how many whitespaces the second_input has
    int whitespace = 0;
    
    //getting the first_input
    fgets(first_input, 100001 * sizeof(char), stdin);
    
    //getting the second_input
    fgets(second_input, 100001 * sizeof(char), stdin);
    
    //counting how many of each letter the first_input has using the first_array
    int first_input_len = strlen(first_input);
    for(int i = 0; i < first_input_len; i++){
        if(first_input[i] == 'A')
            first_array[0]++;
        else if (first_input[i] == 'B')
            first_array[1]++;
        else if (first_input[i] == 'C')
            first_array[2]++;
        else if (first_input[i] == 'D')
            first_array[3]++;
        else if (first_input[i] == 'E')
            first_array[4]++;
        else if (first_input[i] == 'F')
            first_array[5]++;
        else if (first_input[i] == 'G')
            first_array[6]++;
        else if (first_input[i] == 'H')
            first_array[7]++;
        else if (first_input[i] == 'I')
            first_array[8]++;
        else if (first_input[i] == 'J')
            first_array[9]++;
        else if (first_input[i] == 'K')
            first_array[10]++;
        else if (first_input[i] == 'L')
            first_array[11]++;
        else if (first_input[i] == 'M')
            first_array[12]++;
        else if (first_input[i] == 'N')
            first_array[13]++;
        else if (first_input[i] == 'O')
            first_array[14]++;
        else if (first_input[i] == 'P')
            first_array[15]++;
        else if (first_input[i] == 'Q')
            first_array[16]++;
        else if (first_input[i] == 'R')
            first_array[17]++;
        else if (first_input[i] == 'S')
            first_array[18]++;
        else if (first_input[i] == 'T')
            first_array[19]++;
        else if (first_input[i] == 'U')
            first_array[20]++;
        else if (first_input[i] == 'V')
            first_array[21]++;
        else if (first_input[i] == 'W')
            first_array[22]++;
        else if (first_input[i] == 'X')
            first_array[23]++;
        else if (first_input[i] == 'Y')
            first_array[24]++;
        else if (first_input[i] == 'Z')
            first_array[25]++;
    }
    
    //counting how many of each letter the second_input has using the second_array
    int second_input_len = strlen(second_input);
    for(int i = 0; i < second_input_len; i++){
        if(second_input[i] == 'A')
            second_array[0]++;
        else if (second_input[i] == 'B')
            second_array[1]++;
        else if (second_input[i] == 'C')
            second_array[2]++;
        else if (second_input[i] == 'D')
            second_array[3]++;
        else if (second_input[i] == 'E')
            second_array[4]++;
        else if (second_input[i] == 'F')
            second_array[5]++;
        else if (second_input[i] == 'G')
            second_array[6]++;
        else if (second_input[i] == 'H')
            second_array[7]++;
        else if (second_input[i] == 'I')
            second_array[8]++;
        else if (second_input[i] == 'J')
            second_array[9]++;
        else if (second_input[i] == 'K')
            second_array[10]++;
        else if (second_input[i] == 'L')
            second_array[11]++;
        else if (second_input[i] == 'M')
            second_array[12]++;
        else if (second_input[i] == 'N')
            second_array[13]++;
        else if (second_input[i] == 'O')
            second_array[14]++;
        else if (second_input[i] == 'P')
            second_array[15]++;
        else if (second_input[i] == 'Q')
            second_array[16]++;
        else if (second_input[i] == 'R')
            second_array[17]++;
        else if (second_input[i] == 'S')
            second_array[18]++;
        else if (second_input[i] == 'T')
            second_array[19]++;
        else if (second_input[i] == 'U')
            second_array[20]++;
        else if (second_input[i] == 'V')
            second_array[21]++;
        else if (second_input[i] == 'W')
            second_array[22]++;
        else if (second_input[i] == 'X')
            second_array[23]++;
        else if (second_input[i] == 'Y')
            second_array[24]++;
        else if (second_input[i] == 'Z')
            second_array[25]++;
        else if (second_input[i] == ' ')
            whitespace++;
    }
    
    //calculating how many same letters are there between first_input and second_input
    //and putting the length of the second_input into the output variable to make the calculation easier
    output = second_input_len - 1;
    for(int i = 0; i < 26; i++){
        //checking if first_array has the current letter.
        //if it has the current letter, then we substract the letter difference from the variable output
        if(first_array[i] != 0){
            if(first_array[i] >= second_array[i]){
                output = output - second_array[i];
            }else{
                output = output - first_array[i];
            }
        }
    }
    
    //subtracting the whitepaces
    output = output - whitespace;
    
    //printing the output
    printf("%d\n", output);
    
    //freeing the allocated memories before exiting the program
    free(first_input);
    free(second_input);
    free(first_array);
    free(second_array);
    
    return 0;
}
