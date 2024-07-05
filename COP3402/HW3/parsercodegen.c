//
//  parsercodegen.c
//  Homework3
//
//  Created by Alperen Yazmaci on 06.04.2024
//
// AUTHOR: Alperen Yazmaci

// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define the max values
#define MAX_LEXEMES 1000
#define MAX_TOKENS 1000
#define MAX_SOURCE_SIZE 10000
#define MAX_IDENTIFIER_LENGTH 11
#define MAX_NUMBER_LENGTH 5
#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_CODE_LENGTH 500 // Define MAX_CODE_LENGTH for the instruction array size

// Enum alias and definition
typedef enum {
    oddsym = 1, identsym, numbersym, plussym, minussym,
    multsym, slashsym, nulsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym, elsesym, error_token
} token_type;

// Struct aliases and definitions
typedef struct {
    char lexeme[12];
    token_type token;
    char errorMessage[50];
} Lexeme;
typedef struct {
    token_type token;
    char value[12];
} Token;

// Symbol table entry
typedef struct {
    int kind; // const = 1, var = 2
    char name[10]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
} symbol;

// Initialize reserved and special words and tokens as constants
const char *reservedWords[] = {
    "const", "var", "begin", "end", "if", "fi", "then", 
    "while", "do", "read", "write"
};
const token_type reservedTokens[] = {
    constsym, varsym, beginsym, endsym, ifsym, oddsym, thensym, whilesym, dosym, readsym, writesym
};
const char *specialSymbols[] = {
    "+", "-", "*", "/", "(", ")", "=", ",", ".", "<", ">", ";", ":=", "<>"
};
const token_type specialTokens[] = {
    plussym, minussym, multsym, slashsym, lparentsym, rparentsym, eqsym, commasym, periodsym, lessym, gtrsym, semicolonsym, becomessym
};

// Initialize the global variables
Lexeme lexemes[MAX_LEXEMES];
Token tokens[MAX_TOKENS];
int lexemeCount = 0;
int tokenCount = 0;
int current_token = 0;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
int symbol_table_index = 0;

// Code generation variables
typedef struct {
    int op;
    int l;
    int m;
} instruction;

instruction code[MAX_CODE_LENGTH];
int code_index = 0;

// File pointer for error output
FILE *parserOutputFile;

// Function prototypes
void lexicalAnalyzer(const char *source);
void addLexeme(const char *lexeme, token_type token, const char *errorMessage);
void addToken(token_type token, const char *value);
token_type getReservedWordToken(const char *word);
token_type getSpecialSymbolToken(const char *symbol);
void print_error(const char *message, const char *identifier);
void program();
void block();
void const_declaration();
int var_declaration();
void statement();
void condition();
void expression();
void term();
void factor();
int symbol_table_check(char *name);
void add_to_symbol_table(int kind, char *name, int val, int level, int addr);
void print_symbol_table(FILE *outputFile);
void emit(int op, int l, int m);
void print_code(FILE *outputFile);
void mark_symbols();

// Function to add a lexeme to the Lexeme array
void addLexeme(const char *lexeme, token_type token, const char *errorMessage) {
    strncpy(lexemes[lexemeCount].lexeme, lexeme, sizeof(lexemes[lexemeCount].lexeme) - 1);
    lexemes[lexemeCount].lexeme[sizeof(lexemes[lexemeCount].lexeme) - 1] = '\0';
    lexemes[lexemeCount].token = token;
    if (errorMessage != NULL) {
        strncpy(lexemes[lexemeCount].errorMessage, errorMessage, sizeof(lexemes[lexemeCount].errorMessage) - 1);
        lexemes[lexemeCount].errorMessage[sizeof(lexemes[lexemeCount].errorMessage) - 1] = '\0';
    } else {
        lexemes[lexemeCount].errorMessage[0] = '\0';
    }
    lexemeCount++;
}

// Function to add a token to the Token array
void addToken(token_type token, const char *value) {
    tokens[tokenCount].token = token;
    strncpy(tokens[tokenCount].value, value, sizeof(tokens[tokenCount].value) - 1);
    tokens[tokenCount].value[sizeof(tokens[tokenCount].value) - 1] = '\0';
    tokenCount++;
}

// Function to check if a string is a reserved word
token_type getReservedWordToken(const char *word) {
    for (int i = 0; i < sizeof(reservedWords) / sizeof(reservedWords[0]); i++) {
        if (strcmp(word, reservedWords[i]) == 0) {
            return reservedTokens[i];
        }
    }
    return identsym;
}

// Function to check if a string is a special symbol
token_type getSpecialSymbolToken(const char *symbol) {
    for (int i = 0; i < sizeof(specialSymbols) / sizeof(specialSymbols[0]); i++) {
        if (strcmp(symbol, specialSymbols[i]) == 0) {
            return specialTokens[i];
        }
    }
    return nulsym;
}

// The function that goes over source and lexical analyzes the source string
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
                addLexeme("/*", error_token, "Error: unclosed comment");
                continue;
            }
            continue;
        }

        // Handle identifiers and reserved words
        if (isalpha(source[i])) {
            char buffer[12] = {0};
            int j = 0;
            while (isalnum(source[i]) && j < MAX_IDENTIFIER_LENGTH) {
                buffer[j++] = source[i++];
            }
            buffer[j] = '\0'; // Ensure null-terminated
            if (isalnum(source[i])) {
                addLexeme(buffer, error_token, "Error: Identifier too long");
                while (isalnum(source[i])) i++; // Skip the rest of the identifier
                continue;
            }
            token_type token = getReservedWordToken(buffer);
            addLexeme(buffer, token, NULL);
            addToken(token, buffer);
            continue;
        }

        // Handle numbers
        if (isdigit(source[i])) {
            char buffer[6] = {0};
            int j = 0;
            while (isdigit(source[i]) && j < MAX_NUMBER_LENGTH) {
                buffer[j++] = source[i++];
            }
            buffer[j] = '\0';
            if (isdigit(source[i])) {
                addLexeme(buffer, error_token, "Error: Number too long");
                while (isdigit(source[i])) i++;
                continue;
            }
            addLexeme(buffer, numbersym, NULL);
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
        if (token == nulsym) {
            // Handle specific cases for two-character symbols
            if (buffer[0] == '<' && buffer[1] == '>') {
                token = neqsym;
            } else if (buffer[0] == '<' && buffer[1] == '=') {
                token = leqsym;
            } else if (buffer[0] == '>' && buffer[1] == '=') {
                token = geqsym;
            } else if (buffer[0] == ':' && buffer[1] == '=') {
                token = becomessym;
            } else {
                addLexeme(buffer, error_token, "Error: Invalid symbol");
                continue;
            }
        }
        addLexeme(buffer, token, NULL);
        addToken(token, buffer);
    }
}

void print_error(const char *message, const char *identifier) {
    if (identifier != NULL) {
        fprintf(parserOutputFile, "Error: %s %s\n", message, identifier);
        printf("Error: %s %s\n", message, identifier);
    } else {
        fprintf(parserOutputFile, "Error: %s\n", message);
        printf("Error: %s\n", message);
    }
    fclose(parserOutputFile); // Ensure to close the file before exiting
    exit(1);
}

// Recursive Descent Parser and Intermediate Code Generator

void program() {
    emit(7, 0, 3); // JMP to main block
    block();
    if (tokens[current_token].token != periodsym) {
        print_error("program must end with period", NULL);
    }
    emit(9, 0, 3); // HALT
}

void block() {
    const_declaration();
    int numVars = var_declaration();
    emit(6, 0, numVars + 3); // INC
    statement();
}

void const_declaration() {
    if (tokens[current_token].token == constsym) {
        do {
            current_token++;
            if (tokens[current_token].token != identsym) {
                print_error("const keyword must be followed by identifier", NULL);
            }
            char const_name[12];
            strcpy(const_name, tokens[current_token].value);

            current_token++;
            if (tokens[current_token].token != eqsym) {
                print_error("constants must be assigned with =", NULL);
            }

            current_token++;
            if (tokens[current_token].token != numbersym) {
                print_error("constants must be assigned an integer value", NULL);
            }
            int const_value = atoi(tokens[current_token].value);

            add_to_symbol_table(1, const_name, const_value, 0, 0);

            current_token++;
        } while (tokens[current_token].token == commasym);

        if (tokens[current_token].token != semicolonsym) {
            print_error("constant declarations must be followed by a semicolon", NULL);
        }
        current_token++;
    }
}

int var_declaration() {
    int numVars = 0;
    if (tokens[current_token].token == varsym) {
        do {
            numVars++;
            current_token++;
            if (tokens[current_token].token != identsym) {
                print_error("var keyword must be followed by identifier", NULL);
            }
            add_to_symbol_table(2, tokens[current_token].value, 0, 0, numVars + 2);
            current_token++;
        } while (tokens[current_token].token == commasym);

        if (tokens[current_token].token != semicolonsym) {
            print_error("variable declarations must be followed by a semicolon", NULL);
        }
        current_token++;
    }
    return numVars;
}

void statement() {
    if (tokens[current_token].token == identsym) {
        int symIdx = symbol_table_check(tokens[current_token].value);
        if (symIdx == -1) {
            print_error("undeclared identifier", tokens[current_token].value);
        }
        if (symbol_table[symIdx].kind != 2) {
            print_error("only variable values may be altered", NULL);
        }
        current_token++;
        if (tokens[current_token].token != becomessym) {
            print_error("assignment statements must use :=", NULL);
        }
        current_token++;
        expression();
        emit(4, 0, symbol_table[symIdx].addr); // STO
    } else if (tokens[current_token].token == beginsym) {
        do {
            current_token++;
            statement();
        } while (tokens[current_token].token == semicolonsym);
        if (tokens[current_token].token != endsym) {
            print_error("begin must be followed by end", NULL);
        }
        current_token++;
    } else if (tokens[current_token].token == ifsym) {
        current_token++;
        condition();
        int jpcIdx = code_index;
        emit(8, 0, 0); // JPC
        if (tokens[current_token].token != thensym) {
            print_error("if must be followed by then", NULL);
        }
        current_token++;
        statement();
        code[jpcIdx].m = code_index;
    } else if (tokens[current_token].token == whilesym) {
        current_token++;
        int loopIdx = code_index;
        condition();
        int jpcIdx = code_index;
        emit(8, 0, 0); // JPC
        if (tokens[current_token].token != dosym) {
            print_error("while must be followed by do", NULL);
        }
        current_token++;
        statement();
        emit(7, 0, loopIdx); // JMP
        code[jpcIdx].m = code_index;
    } else if (tokens[current_token].token == readsym) {
        current_token++;
        if (tokens[current_token].token != identsym) {
            print_error("read keyword must be followed by identifier", NULL);
        }
        int symIdx = symbol_table_check(tokens[current_token].value);
        if (symIdx == -1) {
            print_error("undeclared identifier", tokens[current_token].value);
        }
        if (symbol_table[symIdx].kind != 2) {
            print_error("only variable values may be altered", NULL);
        }
        current_token++;
        emit(9, 0, 2); // READ
        emit(4, 0, symbol_table[symIdx].addr); // STO
    } else if (tokens[current_token].token == writesym) {
        current_token++;
        expression();
        emit(9, 0, 1); // WRITE
    }
}

void condition() {
    if (tokens[current_token].token == oddsym) {
        current_token++;
        expression();
        emit(2, 0, 11); // ODD
    } else {
        expression();
        if (tokens[current_token].token == eqsym) {
            current_token++;
            expression();
            emit(2, 0, 5); // EQL
        } else if (tokens[current_token].token == neqsym) {
            current_token++;
            expression();
            emit(2, 0, 6); // NEQ
        } else if (tokens[current_token].token == lessym) {
            current_token++;
            expression();
            emit(2, 0, 7); // LSS
        } else if (tokens[current_token].token == leqsym) {
            current_token++;
            expression();
            emit(2, 0, 8); // LEQ
        } else if (tokens[current_token].token == gtrsym) {
            current_token++;
            expression();
            emit(2, 0, 9); // GTR
        } else if (tokens[current_token].token == geqsym) {
            current_token++;
            expression();
            emit(2, 0, 10); // GEQ
        } else {
            print_error("condition must contain comparison operator", NULL);
        }
    }
}

void expression() {
    int addop;
    if (tokens[current_token].token == plussym || tokens[current_token].token == minussym) {
        addop = tokens[current_token].token;
        current_token++;
        term();
        if (addop == minussym) {
            emit(2, 0, 2); // SUB
        }
    } else {
        term();
    }
    while (tokens[current_token].token == plussym || tokens[current_token].token == minussym) {
        addop = tokens[current_token].token;
        current_token++;
        term();
        if (addop == plussym) {
            emit(2, 0, 1); // ADD
        } else {
            emit(2, 0, 2); // SUB
        }
    }
}

void term() {
    factor();
    while (tokens[current_token].token == multsym || tokens[current_token].token == slashsym) {
        int mulop = tokens[current_token].token;
        current_token++;
        factor();
        if (mulop == multsym) {
            emit(2, 0, 3); // MUL
        } else {
            emit(2, 0, 4); // DIV
        }
    }
}

void factor() {
    if (tokens[current_token].token == identsym) {
        int symIdx = symbol_table_check(tokens[current_token].value);
        if (symIdx == -1) {
            print_error("undeclared identifier", tokens[current_token].value);
        }
        current_token++;
        if (symbol_table[symIdx].kind == 1) { // const
            emit(1, 0, symbol_table[symIdx].val); // LIT
        } else { // var
            emit(3, 0, symbol_table[symIdx].addr); // LOD
        }
    } else if (tokens[current_token].token == numbersym) {
        emit(1, 0, atoi(tokens[current_token].value)); // LIT
        current_token++;
    } else if (tokens[current_token].token == lparentsym) {
        current_token++;
        expression();
        if (tokens[current_token].token != rparentsym) {
            print_error("right parenthesis must follow left parenthesis", NULL);
        }
        current_token++;
    } else {
        print_error("arithmetic equations must contain operands, parentheses, numbers, or symbols", NULL);
    }
}

int symbol_table_check(char *name) {
    for (int i = 0; i < symbol_table_index; i++) {
        if (strcmp(symbol_table[i].name, name) == 0 && symbol_table[i].mark == 0) {
            return i;
        }
    }
    return -1;
}

void add_to_symbol_table(int kind, char *name, int val, int level, int addr) {
    symbol_table[symbol_table_index].kind = kind;
    strcpy(symbol_table[symbol_table_index].name, name);
    symbol_table[symbol_table_index].val = val;
    symbol_table[symbol_table_index].level = level;
    symbol_table[symbol_table_index].addr = addr;
    symbol_table[symbol_table_index].mark = 0;
    symbol_table_index++;
}

void mark_symbols() {
    for (int i = 0; i < symbol_table_index; i++) {
        symbol_table[i].mark = 1;
    }
}

void print_symbol_table(FILE *outputFile) {
    fprintf(outputFile, "Symbol Table:\n");
    fprintf(outputFile, "Kind | Name       | Value | Level | Address | Mark\n");
    fprintf(outputFile, "---------------------------------------------------\n");
    for (int i = 0; i < symbol_table_index; i++) {
        fprintf(outputFile, "   %d | %10s | %5d | %5d | %5d | %5d\n", symbol_table[i].kind, symbol_table[i].name, symbol_table[i].val,
               symbol_table[i].level, symbol_table[i].addr, symbol_table[i].mark);
    }
}

void emit(int op, int l, int m) {
    code[code_index].op = op;
    code[code_index].l = l;
    code[code_index].m = m;
    code_index++;
}

void print_code(FILE *outputFile) {
    fprintf(outputFile, "Generated Code:\n");
    fprintf(outputFile, "Line    OP    L    M\n");
    for (int i = 0; i < code_index; i++) {
        char op[4];
        if(code[i].op == 1) strcpy(op, "LIT");
        else if(code[i].op == 2) strcpy(op, "OPR");
        else if(code[i].op == 3) strcpy(op, "LOD");
        else if(code[i].op == 4) strcpy(op, "STO");
        else if(code[i].op == 5) strcpy(op, "CAL");
        else if(code[i].op == 6) strcpy(op, "INC");
        else if(code[i].op == 7) strcpy(op, "JMP");
        else if(code[i].op == 8) strcpy(op, "JPC");
        else if(code[i].op == 9) strcpy(op, "SYS");
        fprintf(outputFile, "%3d %6s %4d %4d\n", i, op, code[i].l, code[i].m);
    }
}

int main(int argc, char *argv[]) {
    // Exit program if there is no input file
    if (argc != 2) {
        printf("Usage: %s <source file>\n", argv[0]);
        return 1;
    }

    // Open the input file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Get the source of input file into char array sourceProgram
    char sourceProgram[MAX_SOURCE_SIZE];
    size_t sourceSize = fread(sourceProgram, 1, MAX_SOURCE_SIZE - 1, file);
    fclose(file);
    sourceProgram[sourceSize] = '\0';

    // Do lexical analysis
    lexicalAnalyzer(sourceProgram);

    // Open output.txt for writing
    FILE *outputFile = fopen("output.txt", "w");
    if (!outputFile) {
        perror("Error opening output file");
        return 1;
    }

    // Output the source program
    // printf("Source Program:\n%s\n\n", sourceProgram);
    fprintf(outputFile, "Source Program:\n%s\n\n", sourceProgram);

    // Output the lexeme table
    // printf("Lexeme Table:\n");
    fprintf(outputFile, "Lexeme Table:\n");
    // printf("\nlexeme token type\n");
    fprintf(outputFile, "\nlexeme token type\n");
    for (int i = 0; i < lexemeCount; i++) {
        if (lexemes[i].token == error_token) {
            // printf("%-15s %s\n", lexemes[i].lexeme, lexemes[i].errorMessage);
            fprintf(outputFile, "%-15s %s\n", lexemes[i].lexeme, lexemes[i].errorMessage);
        } else {
            // printf("%-15s %-5d\n", lexemes[i].lexeme, lexemes[i].token);
            fprintf(outputFile, "%-15s %-5d\n", lexemes[i].lexeme, lexemes[i].token);
        }
    }

    // Output the token list
    // printf("\nToken List:\n");
    fprintf(outputFile, "\nToken List:\n");
    for (int i = 0; i < tokenCount; i++) {
        // printf("%d", tokens[i].token);
        fprintf(outputFile, "%d", tokens[i].token);
        if (tokens[i].token == identsym || tokens[i].token == numbersym) {
            // printf(" %s", tokens[i].value);
            fprintf(outputFile, " %s", tokens[i].value);
        }
        if (i < tokenCount - 1) {
            // printf(" ");
            fprintf(outputFile, " ");
        }
    }
    // printf("\n");
    fprintf(outputFile, "\n");

    // Close the output file
    fclose(outputFile);

    // Open output1.txt for writing
    parserOutputFile = fopen("output1.txt", "w");
    if (!parserOutputFile) {
        perror("Error opening output file");
        return 1;
    }

    // Parse the program
    program();

    // Mark all symbols as available
    mark_symbols();

    // Output the generated code
    print_code(parserOutputFile);
    printf("\n");

    // Output the symbol table
    print_symbol_table(parserOutputFile);

    printf("Program parsed successfully.\n");

    fclose(parserOutputFile);

    return 0;
}
