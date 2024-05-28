/*=============================================================================
| Assignment: pa02 - Calculating an 8, 16, or 32 bit
| checksum on an ASCII input file
|
| Author: Alperen Yazmaci
| Language: c, c++, Java, GO, Python
|
| To Compile: javac pa02.java
| gcc -o pa02 pa02.c
| g++ -o pa02 pa02.cpp
| go build pa02.go
| python pa02.py //Caution - expecting input parameters
|
| To Execute: java -> java pa02 inputFile.txt 8
| or c++ -> ./pa02 inputFile.txt 8
| or c -> ./pa02 inputFile.txt 8
| or go -> ./pa02 inputFile.txt 8
| or python-> python3 pa02.py inputFile.txt 8
| where inputFile.txt is an ASCII input file
| and the number 8 could also be 16 or 32
| which are the valid checksum sizes, all
| other values are rejected with an error message
| and program termination
|
| Note: All input files are simple 8 bit ASCII input
|
| Class: CIS3360 - Security in Computing - Fall 2023
| Instructor: McAlpin
| Due Date: 12 November 2023
|
+=============================================================================*/

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//function that reads the given file and returns a string of that file's content
char *readFile(const char *filename, size_t *size) {
    // Open the file
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Initialize variables
    size_t capacity = 4096;  // Initial capacity
    size_t length = 0;
    char *content = (char *)malloc(capacity);
    if (content == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return NULL;
    }

    // Read the file content in chunks
    size_t bytesRead;
    do {
        if (length + 1024 > capacity) {
            // Increase the capacity if needed
            capacity *= 2;
            char *newContent = (char *)realloc(content, capacity);
            if (newContent == NULL) {
                perror("Memory reallocation error");
                fclose(file);
                free(content);
                return NULL;
            }
            content = newContent;
        }

        bytesRead = fread(content + length, 1, 1024, file);
        length += bytesRead;
    } while (bytesRead == 1024);

    // Null-terminate the string
    content[length] = '\0';

    // Set the size parameter
    if (size != NULL) {
        *size = length;
    }

    // Close the file
    fclose(file);

    return content;
}

// Function to remove whitespace and newline characters from a string
void removeWhitespaceAndNewline(char* inputString) {
    int len = strlen(inputString);
    int j = 0; // Index for the modified string without whitespace and newline characters

    for (int i = 0; i < len; i++) {
        // Exclude whitespace and newline characters
        if (inputString[i] != ' ' && inputString[i] != '\t' && inputString[i] != '\n' && inputString[i] != '\r') {
            inputString[j++] = inputString[i];
        }
    }

    // Null-terminate the modified string
    inputString[j] = '\0';
}

// Function to split a string into chunks
char** splitStringIntoChunks(const char* inputString, int chunkSize, int numChunks) {
    int len = strlen(inputString);

    // Allocate memory for the 2D array
    char** chunks = (char**)malloc(numChunks * sizeof(char*));
    for (int i = 0; i < numChunks; i++) {
        chunks[i] = (char*)malloc((chunkSize + 1) * sizeof(char));
        memset(chunks[i], '\0', (chunkSize + 1) * sizeof(char));
    }

    // Copy chunks of the string into the 2D array and pad the last string
    for (int i = 0; i < numChunks; i++) {
        strncpy(chunks[i], inputString + i * chunkSize, chunkSize);
        // Pad the last string with 'X' if needed
        if (i == numChunks - 1 && strlen(chunks[i]) < chunkSize) {
            int padding = chunkSize - strlen(chunks[i]) + 1;
            for (int j = 0; j < padding; j++) {
                strcat(chunks[i], "X");
            }
        }
        removeWhitespaceAndNewline(chunks[i]);
    }

    return chunks;
}

// Function to calculate the checksum
unsigned long calculateChecksum(char** words, int numWords) {
    unsigned long checksum = 0;

    // Iterate through each word
    for (int i = 0; i < numWords; i++) {
        // Add the word as an unsigned binary number
        for (int j = 0; words[i][j] != '\0'; j++) {
            checksum += (unsigned long)words[i][j];
        }
    }

    // Discard overflow bits
    checksum = checksum & 0xFFFFFFFF;

    // Interpret the result as a two's complement number
    checksum = (checksum ^ 0xFFFFFFFF) + 1;

    return checksum;
}

//function that frees the 2d array
void freeStringArray(char **array, int size) {
    if (array != NULL) {
        for (int i = 0; i < size; ++i) {
            free(array[i]);
        }
        free(array);
    }
}

// Function to print a given string in rows of 80 characters per row
void printStringInRows(const char *str) {
    int rowLength = 80; // Maximum characters per row
    int strLength = strlen(str);
    
    for (int i = 0; i < strLength; i += rowLength) {
        int remainingChars = strLength - i;
        int charsInCurRow = (remainingChars >= rowLength) ? rowLength : remainingChars;

        for (int j = 0; j < charsInCurRow; j++) {
            putchar(str[i + j]);
        }

        putchar('\n');
    }
}


int main(int argc, char *argv[]) {
    //checking to see if the argument count is correct
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <checksum_size>\n", argv[0]);
        fprintf(stderr, "Valid checksum sizes are 8, 16, or 32\n");
        return 1;
    }
    
    //creating the variables needed for storing the arguments
    const char *inputFileName = argv[1];
    int checksumSize = atoi(argv[2]);
    size_t size;
    //creating a string to store the information in input file
    char *fileContent = readFile(inputFileName, &size);
    //creating a integer to store
    int inputLen = (int) strlen(fileContent);
    //print the input file
    printStringInRows(fileContent);
    
    //divide the the string into chunks that are size of the checksum and create a 2d array of those chunks
    int chunkSize = checksumSize / 8;
    int numChunks;
    if((inputLen-1) % chunkSize == 0){
        numChunks = (inputLen-1) / chunkSize;
    }else{
        numChunks = (inputLen-1) / chunkSize + 1;
    }
    char **stringChunks = splitStringIntoChunks(fileContent, chunkSize, numChunks);

    //calculate the checksum
    unsigned long checksum = calculateChecksum(stringChunks, numChunks);
    
    //print the result
    printf("%2d bit checksum is %8lx for all %4d chars\n", checksumSize, checksum, inputLen - 1);
    
    // Free the allocated memory
    freeStringArray(stringChunks, numChunks);
    free(fileContent);

    return 0;
}

/*=============================================================================
 | I Alperen Yazmaci (al506241) affirm that this program is
 | entirely my own work and that I have neither developed my code together with
 | any another person, nor copied any code from any other person, nor permitted
 | my code to be copied or otherwise used by any other person, nor have I
 | copied, modified, or otherwise used programs created by others. I acknowledge
 | that any violation of the above terms will be treated as academic dishonesty.
 +============================================================================*/
