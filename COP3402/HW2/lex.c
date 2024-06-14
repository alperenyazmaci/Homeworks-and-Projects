//
//  lex.c
//  Homework2
//
//  Created by Alperen Yazmaci on 06.04.2024
//
// AUTHOR: Alperen Yazmaci

//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Define MAX values
#define MAX_LEXEMES 1000
#define MAX_TOKENS 1000
#define MAX_SOURCE_SIZE 10000

//Enum Alias and Definition
typedef enum {
skipsym = 1, identsym, numbersym, plussym, minussym,
multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym , elsesym} token_type;

//Struct Aliases and Definitions
typedef struct {
    char lexeme[12];
    token_type token;
} Lexeme;
typedef struct {
    token_type token;
    char value[12];
} Token;

//Initialize reserved and special words and tokens as constants
const char *reservedWords[] = {
    "const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", 
    "else", "while", "do", "read", "write"
};
const token_type reservedTokens[] = {
    constsym, varsym, procsym, callsym, beginsym, endsym, ifsym, fisym, thensym, elsesym, whilesym, dosym, readsym, writesym
};
const char *specialSymbols[] = {
    "+", "-", "*", "/", "(", ")", "=", ",", ".", "<", ">", ";", ":="
};
const token_type specialTokens[] = {
    plussym, minussym, multsym, slashsym, lparentsym, rparentsym, eqsym, commasym, periodsym, lessym, gtrsym, semicolonsym, becomessym
};

//Initialize the global variables
Lexeme lexemes[MAX_LEXEMES];
Token tokens[MAX_TOKENS];
int lexemeCount = 0;
int tokenCount = 0;

void addLexeme(const char *lexeme, token_type token) {
    strcpy(lexemes[lexemeCount].lexeme, lexeme);
    lexemes[lexemeCount].token = token;
    lexemeCount++;
}

void addToken(token_type token, const char *value) {
    tokens[tokenCount].token = token;
    strcpy(tokens[tokenCount].value, value);
    tokenCount++;
}

token_type getReservedWordToken(const char *word) {
    for (int i = 0; i < sizeof(reservedWords) / sizeof(reservedWords[0]); i++) {
        if (strcmp(word, reservedWords[i]) == 0) {
            return reservedTokens[i];
        }
    }
    return identsym;
}

token_type getSpecialSymbolToken(const char *symbol) {
    for (int i = 0; i < sizeof(specialSymbols) / sizeof(specialSymbols[0]); i++) {
        if (strcmp(symbol, specialSymbols[i]) == 0) {
            return specialTokens[i];
        }
    }
    return skipsym;
}

void lexicalAnalyzer(const char *source) {
    int i = 0;
    while (source[i] != '\0') {
        if (isspace(source[i])) {
            i++;
            continue;
        }

        // Handle comments
        if (source[i] == '/' && source[i + 1] == '*') {
            i += 2;
            int comment_closed = 0;
            while (source[i] != '\0') {
                if (source[i] == '*' && source[i + 1] == '/') {
                    i += 2;
                    comment_closed = 1;
                    break;
                }
                i++;
            }
            if (!comment_closed) {
                printf("Error: Improperly enclosed comment\n");
                exit(1);
            }
            continue;
        }

        // Handle identifiers and reserved words
        if (isalpha(source[i])) {
            char buffer[12] = {0};
            int j = 0;
            while (isalnum(source[i]) && j < 11) {
                buffer[j++] = source[i++];
            }
            if (isalnum(source[i])) {
                printf("Error: Identifier too long\n");
                exit(1);
            }
            token_type token = getReservedWordToken(buffer);
            addLexeme(buffer, token);
            addToken(token, buffer);
            continue;
        }

        // Handle numbers
        if (isdigit(source[i])) {
            char buffer[6] = {0};
            int j = 0;
            while (isdigit(source[i]) && j < 5) {
                buffer[j++] = source[i++];
            }
            if (isdigit(source[i])) {
                printf("Error: Number too long\n");
                exit(1);
            }
            addLexeme(buffer, numbersym);
            addToken(numbersym, buffer);
            continue;
        }

        // Handle complex token sequences and special symbols
        char buffer[3] = {0};
        buffer[0] = source[i++];
        if ((buffer[0] == ':' && source[i] == '=') || 
            (buffer[0] == '<' && source[i] == '>') || 
            (buffer[0] == '<' && source[i] == '=') || 
            (buffer[0] == '>' && source[i] == '=')) {
            buffer[1] = source[i++];
        }
        token_type token = getSpecialSymbolToken(buffer);
        if (token == skipsym) {
            printf("Error: Invalid symbol %s\n", buffer);
            exit(1);
        }
        addLexeme(buffer, token);
        addToken(token, buffer);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <source file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char sourceProgram[MAX_SOURCE_SIZE];
    size_t sourceSize = fread(sourceProgram, 1, MAX_SOURCE_SIZE - 1, file);
    fclose(file);
    sourceProgram[sourceSize] = '\0';

    // Perform lexical analysis
    lexicalAnalyzer(sourceProgram);

    // Output the source program
    printf("Source Program:\n%s\n\n", sourceProgram);

    // Output the lexeme table
    printf("Lexeme Table:\n");
    for (int i = 0; i < lexemeCount; i++) {
        printf("%s %d\n", lexemes[i].lexeme, lexemes[i].token);
    }

    // Output the token list
    printf("Token List:\n");
    for (int i = 0; i < tokenCount; i++) {
        printf("%d %s ", tokens[i].token, tokens[i].value);
    }
    printf("\n");

    return 0;
}