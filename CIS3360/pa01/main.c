/*============================================================================
| Assignment: pa01 - Encrypting a plaintext file using the Hill cipher
|
| Author: Alperen Yazmaci
| Language: c, c++, Java, go, python
|
| To Compile: javac pa01.java
| gcc -o pa01 pa01.c
| g++ -o pa01 pa01.cpp
| go build pa01.go
|
| To Execute: java -> java pa01 kX.txt pX.txt
| or c++ -> ./pa01 kX.txt pX.txt
| or c -> ./pa01 kX.txt pX.txt
| or go -> ./pa01 kX.txt pX.txt
| or python -> python3 pa01.py kX.txt pX.txt
| where kX.txt is the keytext file
| and pX.txt is plaintext file
| Note:
| All input files are simple 8 bit ASCII input
| All execute commands above have been tested on Eustis
|
| Class: CIS3360 - Security in Computing - Fall 2023
| Instructor: McAlpin
| Due Date: 8 October 2023
+===========================================================================*/

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to print a 2d int array size of n*n which is the key matrix
void printKey(int **matrix, int size) {
    printf("Key matrix:\n");
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Function to make the string get rid of everythin except alphabetical letters and make those letters lowercase
void removeSpecialChars(char *str) {
    int length = strlen(str);
    int index = 0; // An integer to keep track of the modified string

    for (int i = 0; i < length; i++) {
        char c = str[i];

        // If the char c is a letter make it lowercase
        if (isalpha(c)) {
            str[index] = tolower(c);
            index++;
        }
    }

    // Null-terminate the modified string
    str[index] = '\0';
}

// Function to pad the plaintext with letter 'x' to match the key size
void padPlaintext(char *plaintext, int keySize) {
    int textLen = strlen(plaintext);
    int padLen = (textLen % keySize == 0) ? textLen : textLen + (keySize - textLen % keySize);

    // Pad with letter 'x'
    for (int i = textLen; i < padLen; i++) {
        plaintext[i] = 'x';
    }

    plaintext[padLen] = '\0'; // End the string to make sure
}

// Function to read text from a file then return the string that stores it
char* readTextFromFile(const char *filename) {
    FILE *textFile = fopen(filename, "r");
    if (textFile == NULL) {
        perror("Error opening text file");
        return NULL;
    }

    // Get the file size
    fseek(textFile, 0, SEEK_END);
    long fileSize = ftell(textFile);
    fseek(textFile, 0, SEEK_SET);

    // Dynamically allocate memory for the text string which will be returned
    char *text = (char *)malloc((fileSize + 1) * sizeof(char));
    if (text == NULL) {
        printf("Memory allocation failed for the text.\n");
        fclose(textFile);
        return NULL;
    }

    // Get the text from the file into the text string which will be returned
    if (fread(text, sizeof(char), fileSize, textFile) != fileSize) {
        printf("Error reading text from the file.\n");
        fclose(textFile);
        free(text);
        return NULL;
    }

    // End the string to make sure
    text[fileSize] = '\0';

    // Close the text file
    fclose(textFile);

    //return the string text
    return text;
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

// Function that does the matrix-vector multiplication for Hill cipher
void matrixVectorMultiply(int **matrix, int *vector, int *result, int n) {
    for (int i = 0; i < n; i++) {
        result[i] = 0;
        for (int j = 0; j < n; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
        result[i] %= 26;  // Make sure the result is in the alphabet size
    }
}

// Function to encrypt given plaintext, key size, key matrix using Hill cipher with padding
char* hillCipherEncrypt(int **keyMatrix, int keySize, const char *plaintext) {
    int textLength = strlen(plaintext);
    int blockSize = keySize;
    int paddedLength = (textLength % blockSize == 0) ? textLength : textLength + (blockSize - textLength % blockSize);
    int *numericText = (int *)malloc(paddedLength * sizeof(int));
    int *encryptedNumericText = (int *)malloc(paddedLength * sizeof(int));

    if (numericText == NULL || encryptedNumericText == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    // Convert plaintext to numeric values (a being a=0 and z being z=25)
    for (int i = 0; i < textLength; i++) {
        if (isalpha(plaintext[i])) {
            numericText[i] = tolower(plaintext[i]) - 'a';
        } else {
            numericText[i] = ('x' - 'a'); // Pad with 'x' if the current character is not a letter
        }
    }

    // Encrypt the text in blocks
    for (int i = 0; i < paddedLength; i += blockSize) {
        int *block = &numericText[i];
        int *encryptedBlock = &encryptedNumericText[i];

        matrixVectorMultiply(keyMatrix, block, encryptedBlock, keySize);
    }

    char *encryptedString = (char *)malloc(paddedLength + 1); // +1 for the '\0'
    if (encryptedString == NULL) {
        printf("Memory allocation failed.\n");
        free(numericText);
        free(encryptedNumericText);
        return NULL;
    }

    // Convert back to alphabetical values
    for (int i = 0; i < paddedLength; i++) {
        if (isalpha(plaintext[i])) {
            char encryptedChar = 'A' + encryptedNumericText[i];
            if (islower(plaintext[i])) {
                encryptedChar = tolower(encryptedChar);
            }
            encryptedString[i] = encryptedChar;
        } else {
            encryptedString[i] = plaintext[i];
        }
    }

    encryptedString[paddedLength] = '\0';

    //free the allocated memory
    free(numericText);
    free(encryptedNumericText);

    //return the encrypted text
    return encryptedString;
}

int main(int argc, char *argv[]) {
    //check if the argument count is correct, execute the program if argument count is not 3(including execution command)
    if (argc != 3) {
        printf("Usage: %s <matrix_file> <text_file>\n", argv[0]);
        return 1;
    }

    // Open the key file for reading, execute the program if key file cannot be opened
    FILE *keyFile = fopen(argv[1], "r");
    if (keyFile == NULL) {
        perror("Error opening matrix file");
        return 1;
    }

    // Read the first line to get the value of key size, execute the program if first value cannot be read
    int keySize;
    if (fscanf(keyFile, "%d", &keySize) != 1) {
        printf("Error reading the integer from the first line of the matrix file.\n");
        fclose(keyFile);
        return 1;
    }

    // Dynamically allocate memory for the keySize * keySize matrix, execute the program if memory allocation fails
    int **matrix = (int **)malloc(keySize * sizeof(int *));
    if (matrix == NULL) {
        printf("Memory allocation failed for the matrix.\n");
        fclose(keyFile);
        return 1;
    }
    for (int i = 0; i < keySize; i++) {
        matrix[i] = (int *)malloc(keySize * sizeof(int));
        if (matrix[i] == NULL) {
            printf("Memory allocation failed for the matrix.\n");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            fclose(keyFile);
            return 1;
        }
    }

    // Read the matrix values from the key file and store them in the 2d int array, execute the program if key file is empty
    for (int i = 0; i < keySize; i++) {
        for (int j = 0; j < keySize; j++) {
            if (fscanf(keyFile, "%d", &matrix[i][j]) != 1) {
                printf("Error reading matrix element at (%d, %d) from the matrix file.\n", i, j);
                fclose(keyFile);
                for (int k = 0; k < keySize; k++) {
                    free(matrix[k]);
                }
                free(matrix);
                return 1;
            }
        }
    }

    // Close the matrix file
    fclose(keyFile);

    // Print the key matrix using the printKey() function
    printf("\n");
    printKey(matrix, keySize);

    // Read the text from the text file and store it in a string, execute the program if text file is empty
    char *text = readTextFromFile(argv[2]);
    if (text == NULL) {
        for (int i = 0; i < keySize; i++) {
            free(matrix[i]);
        }
        free(matrix);
        return 1;
    }
    
    //remove all punctuation, numbers, special characters, and whitespace
    removeSpecialChars(text);
    //pad the plain text if necessary
    padPlaintext(text, keySize);

    // Print the plaintext
    printf("Plaintext:\n");
    printStringInRows(text);
    printf("\n");

    // Print the ciphertext
    printf("Ciphertext:\n");
    printStringInRows(hillCipherEncrypt(matrix, keySize, text));
    printf("\n");
    
    // Free dynamically allocated memory for the matrix and the text
    for (int i = 0; i < keySize; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(text);

    //execute the program successfully
    return 0;
}

/*=============================================================================
| I Alperen Yazmaci al506241 affirm that this program is
| entirely my own work and that I have neither developed my code together with
| any another person, nor copied any code from any other person, nor permitted
| my code to be copied or otherwise used by any other person, nor have I
| copied, modified, or otherwise used programs created by others. I acknowledge
| that any violation of the above terms will be treated as academic dishonesty.
+=============================================================================*/
