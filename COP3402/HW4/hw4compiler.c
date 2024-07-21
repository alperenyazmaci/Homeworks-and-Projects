//  hw4compiler.c
//  Homework4
//
//  Created by Alperen Yazmaci
//
// AUTHOR: Alperen Yazmaci

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
#define MAX_CODE_LENGTH 500

// Enum alias and definition
typedef enum {
    oddsym = 1, identsym, numbersym, plussym, minussym,
    multsym, slashsym, nulsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym, elsesym, fisym, error_token
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
    int kind; // const = 1, var = 2, proc = 3
    char name[10]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
} symbol;

// Initialize reserved and special words and tokens as constants
const char *reservedWords[] = {
    "const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", 
    "else", "while", "do", "read", "write"
};
const token_type reservedTokens[] = {
    constsym, varsym, procsym, callsym, beginsym, endsym, ifsym, fisym, thensym, elsesym, whilesym, dosym, readsym, writesym
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

// Function prototypes
void lexicalAnalyzer(const char *source);
void addLexeme(const char *lexeme, token_type token, const char *errorMessage);
void addToken(token_type token, const char *value);
token_type getReservedWordToken(const char *word);
token_type getSpecialSymbolToken(const char *symbol);
void print_error(int code, const char *message, const char *identifier, FILE *errorFile);
void program(FILE *errorFile);
void block(FILE *errorFile, int level);
void const_declaration(FILE *errorFile, int level);
int var_declaration(FILE *errorFile, int level);
void statement(FILE *errorFile, int level);
void condition(FILE *errorFile, int level);
void expression(FILE *errorFile, int level);
void term(FILE *errorFile, int level);
void factor(FILE *errorFile, int level);
int symbol_table_check(char *name, int level);
int symbol_table_check_declaration(char *name, int level);
void add_to_symbol_table(int kind, char *name, int val, int level, int addr);
void print_symbol_table(FILE *outputFile);
void emit(int op, int l, int m);
void print_code(FILE *outputFile);
void mark_symbols(int level);

// Error messages
const char *error_messages[] = {
    "Use = instead of :=.",
    "= must be followed by a number.",
    "Identifier must be followed by =.",
    "const, var, procedure must be followed by identifier.",
    "Semicolon or comma missing.",
    "Incorrect symbol after procedure declaration.",
    "Statement expected.",
    "Incorrect symbol after statement part in block.",
    "Period expected.",
    "Semicolon between statements missing.",
    "Undeclared identifier.",
    "Assignment to constant or procedure is not allowed.",
    "Assignment operator expected.",
    "call must be followed by an identifier.",
    "Call of a constant or variable is meaningless.",
    "then expected.",
    "Semicolon or end expected.",
    "do expected.",
    "Incorrect symbol following statement.",
    "Relational operator expected.",
    "Expression must not contain a procedure identifier.",
    "Right parenthesis missing.",
    "The preceding factor cannot begin with this symbol.",
    "An expression cannot begin with this symbol.",
    "This number is too large.",
    "Identifier too long.",
    "Invalid symbol."
};

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

void print_error(int code, const char *message, const char *identifier, FILE *errorFile) {
    if (identifier != NULL) {
        printf("***** Error number %d, %s %s\n", code, message, identifier);
        fprintf(errorFile, "***** Error number %d, %s %s\n", code, message, identifier);
    } else {
        printf("***** Error number %d, %s\n", code, message);
        fprintf(errorFile, "***** Error number %d, %s\n", code, message);
    }
    exit(1);
}

// Recursive Descent Parser and Intermediate Code Generator

void program(FILE *errorFile) {
    emit(7, 0, 3); // JMP to main block
    block(errorFile, 0);
    if (tokens[current_token].token != periodsym) {
        print_error(9, error_messages[8], NULL, errorFile); // Period expected
    }
    emit(9, 0, 3); // HALT
}

void block(FILE *errorFile, int level) {
    int tx0 = symbol_table_index; // Save current table index
    int dx = 3; // Data allocation index

    symbol_table[symbol_table_index].addr = code_index;
    emit(7, 0, 0); // JMP to main block, address to be filled later

    do {
        if (tokens[current_token].token == constsym) {
            const_declaration(errorFile, level);
        }
        if (tokens[current_token].token == varsym) {
            dx += var_declaration(errorFile, level);
        }
        while (tokens[current_token].token == procsym) {
            current_token++;
            if (tokens[current_token].token == identsym) {
                add_to_symbol_table(3, tokens[current_token].value, 0, level, code_index);
                current_token++;
            } else {
                print_error(4, error_messages[3], NULL, errorFile); // const, var, procedure must be followed by identifier
            }
            if (tokens[current_token].token != semicolonsym) {
                print_error(5, error_messages[4], NULL, errorFile); // Semicolon or comma missing
            }
            current_token++;
            block(errorFile, level + 1);
            if (tokens[current_token].token != semicolonsym) {
                print_error(5, error_messages[4], NULL, errorFile); // Semicolon or comma missing
            }
            current_token++;
        }
    } while (tokens[current_token].token == constsym || tokens[current_token].token == varsym || tokens[current_token].token == procsym);

    symbol_table[tx0].addr = code_index;
    code[symbol_table[tx0].addr].m = code_index;

    emit(6, 0, dx); // INC

    statement(errorFile, level);
    emit(2, 0, 0); // OPR, return

    mark_symbols(level);
}

void const_declaration(FILE *errorFile, int level) {
    if (tokens[current_token].token == constsym) {
        do {
            current_token++;
            if (tokens[current_token].token != identsym) {
                print_error(4, error_messages[3], NULL, errorFile); // const, var, procedure must be followed by identifier
            }
            char const_name[12];
            strcpy(const_name, tokens[current_token].value);

            if (symbol_table_check_declaration(const_name, level) != -1) {
                print_error(6, error_messages[5], const_name, errorFile); // Incorrect symbol after procedure declaration
            }

            current_token++;
            if (tokens[current_token].token != eqsym) {
                print_error(3, error_messages[2], NULL, errorFile); // Identifier must be followed by =
            }

            current_token++;
            if (tokens[current_token].token != numbersym) {
                print_error(2, error_messages[1], NULL, errorFile); // = must be followed by a number
            }
            int const_value = atoi(tokens[current_token].value);

            add_to_symbol_table(1, const_name, const_value, level, 0);

            current_token++;
        } while (tokens[current_token].token == commasym);

        if (tokens[current_token].token != semicolonsym) {
            print_error(5, error_messages[4], NULL, errorFile); // Semicolon or comma missing
        }
        current_token++;
    }
}

int var_declaration(FILE *errorFile, int level) {
    int numVars = 0;
    if (tokens[current_token].token == varsym) {
        do {
            numVars++;
            current_token++;
            if (tokens[current_token].token != identsym) {
                print_error(4, error_messages[3], NULL, errorFile); // const, var, procedure must be followed by identifier
            }
            if (symbol_table_check_declaration(tokens[current_token].value, level) != -1) {
                print_error(6, error_messages[5], tokens[current_token].value, errorFile); // Incorrect symbol after procedure declaration
            }
            add_to_symbol_table(2, tokens[current_token].value, 0, level, numVars + 2);
            current_token++;
        } while (tokens[current_token].token == commasym);

        if (tokens[current_token].token != semicolonsym) {
            print_error(5, error_messages[4], NULL, errorFile); // Semicolon or comma missing
        }
        current_token++;
    }
    return numVars;
}

void statement(FILE *errorFile, int level) {
    if (tokens[current_token].token == identsym) {
        int symIdx = symbol_table_check(tokens[current_token].value, level);
        if (symIdx == -1) {
            print_error(11, error_messages[10], tokens[current_token].value, errorFile); // Undeclared identifier
        }
        if (symbol_table[symIdx].kind != 2) {
            print_error(12, error_messages[11], NULL, errorFile); // Assignment to constant or procedure is not allowed
        }
        current_token++;
        if (tokens[current_token].token != becomessym) {
            print_error(13, error_messages[12], NULL, errorFile); // Assignment operator expected
        }
        current_token++;
        expression(errorFile, level);
        emit(4, level - symbol_table[symIdx].level, symbol_table[symIdx].addr); // STO
    } else if (tokens[current_token].token == beginsym) {
        do {
            current_token++;
            statement(errorFile, level);
        } while (tokens[current_token].token == semicolonsym);
        if (tokens[current_token].token != endsym) {
            print_error(17, error_messages[16], NULL, errorFile); // Semicolon or end expected
        }
        current_token++;
    } else if (tokens[current_token].token == ifsym) {
        current_token++;
        condition(errorFile, level);
        int jpcIdx = code_index;
        emit(8, 0, 0); // JPC
        if (tokens[current_token].token != thensym) {
            print_error(16, error_messages[15], NULL, errorFile); // then expected
        }
        current_token++;
        statement(errorFile, level);
        code[jpcIdx].m = code_index;
        if (tokens[current_token].token == elsesym) {
            int jmpIdx = code_index;
            emit(7, 0, 0); // JMP
            current_token++;
            statement(errorFile, level);
            code[jmpIdx].m = code_index;
        }
        if (tokens[current_token].token != fisym) {
            print_error(19, error_messages[18], NULL, errorFile); // Incorrect symbol following statement
        }
        current_token++;
    } else if (tokens[current_token].token == whilesym) {
        current_token++;
        int loopIdx = code_index;
        condition(errorFile, level);
        int jpcIdx = code_index;
        emit(8, 0, 0); // JPC
        if (tokens[current_token].token != dosym) {
            print_error(18, error_messages[17], NULL, errorFile); // do expected
        }
        current_token++;
        statement(errorFile, level);
        emit(7, 0, loopIdx); // JMP
        code[jpcIdx].m = code_index;
    } else if (tokens[current_token].token == readsym) {
        current_token++;
        if (tokens[current_token].token != identsym) {
            print_error(14, error_messages[13], NULL, errorFile); // call must be followed by an identifier
        }
        int symIdx = symbol_table_check(tokens[current_token].value, level);
        if (symIdx == -1) {
            print_error(11, error_messages[10], tokens[current_token].value, errorFile); // Undeclared identifier
        }
        if (symbol_table[symIdx].kind != 2) {
            print_error(12, error_messages[11], NULL, errorFile); // Assignment to constant or procedure is not allowed
        }
        current_token++;
        emit(9, 0, 2); // READ
        emit(4, level - symbol_table[symIdx].level, symbol_table[symIdx].addr); // STO
    } else if (tokens[current_token].token == writesym) {
        current_token++;
        expression(errorFile, level);
        emit(9, 0, 1); // WRITE
    } else if (tokens[current_token].token == callsym) {
        current_token++;
        if (tokens[current_token].token != identsym) {
            print_error(14, error_messages[13], NULL, errorFile); // call must be followed by an identifier
        }
        int symIdx = symbol_table_check(tokens[current_token].value, level);
        if (symIdx == -1) {
            print_error(11, error_messages[10], tokens[current_token].value, errorFile); // Undeclared identifier
        }
        if (symbol_table[symIdx].kind != 3) {
            print_error(15, error_messages[14], tokens[current_token].value, errorFile); // Call of a constant or variable is meaningless
        }
        current_token++;
        emit(5, level - symbol_table[symIdx].level, symbol_table[symIdx].addr); // CAL
    }
}

void condition(FILE *errorFile, int level) {
    if (tokens[current_token].token == oddsym) {
        current_token++;
        expression(errorFile, level);
        emit(2, 0, 11); // ODD
    } else {
        expression(errorFile, level);
        if (tokens[current_token].token == eqsym) {
            current_token++;
            expression(errorFile, level);
            emit(2, 0, 5); // EQL
        } else if (tokens[current_token].token == neqsym) {
            current_token++;
            expression(errorFile, level);
            emit(2, 0, 6); // NEQ
        } else if (tokens[current_token].token == lessym) {
            current_token++;
            expression(errorFile, level);
            emit(2, 0, 7); // LSS
        } else if (tokens[current_token].token == leqsym) {
            current_token++;
            expression(errorFile, level);
            emit(2, 0, 8); // LEQ
        } else if (tokens[current_token].token == gtrsym) {
            current_token++;
            expression(errorFile, level);
            emit(2, 0, 9); // GTR
        } else if (tokens[current_token].token == geqsym) {
            current_token++;
            expression(errorFile, level);
            emit(2, 0, 10); // GEQ
        } else {
            print_error(20, error_messages[19], NULL, errorFile); // Relational operator expected
        }
    }
}

void expression(FILE *errorFile, int level) {
    int addop;
    if (tokens[current_token].token == plussym || tokens[current_token].token == minussym) {
        addop = tokens[current_token].token;
        current_token++;
        term(errorFile, level);
        if (addop == minussym) {
            emit(2, 0, 2); // SUB
        }
    } else {
        term(errorFile, level);
    }
    while (tokens[current_token].token == plussym || tokens[current_token].token == minussym) {
        addop = tokens[current_token].token;
        current_token++;
        term(errorFile, level);
        if (addop == plussym) {
            emit(2, 0, 1); // ADD
        } else {
            emit(2, 0, 2); // SUB
        }
    }
}

void term(FILE *errorFile, int level) {
    factor(errorFile, level);
    while (tokens[current_token].token == multsym || tokens[current_token].token == slashsym) {
        int mulop = tokens[current_token].token;
        current_token++;
        factor(errorFile, level);
        if (mulop == multsym) {
            emit(2, 0, 3); // MUL
        } else {
            emit(2, 0, 4); // DIV
        }
    }
}

void factor(FILE *errorFile, int level) {
    if (tokens[current_token].token == identsym) {
        int symIdx = symbol_table_check(tokens[current_token].value, level);
        if (symIdx == -1) {
            print_error(11, error_messages[10], tokens[current_token].value, errorFile); // Undeclared identifier
        }
        current_token++;
        if (symbol_table[symIdx].kind == 1) { // const
            emit(1, 0, symbol_table[symIdx].val); // LIT
        } else { // var
            emit(3, level - symbol_table[symIdx].level, symbol_table[symIdx].addr); // LOD
        }
    } else if (tokens[current_token].token == numbersym) {
        emit(1, 0, atoi(tokens[current_token].value)); // LIT
        current_token++;
    } else if (tokens[current_token].token == lparentsym) {
        current_token++;
        expression(errorFile, level);
        if (tokens[current_token].token != rparentsym) {
            print_error(22, error_messages[21], NULL, errorFile); // Right parenthesis missing
        }
        current_token++;
    } else {
        print_error(23, error_messages[22], NULL, errorFile); // The preceding factor cannot begin with this symbol
    }
}

int symbol_table_check(char *name, int level) {
    for (int i = symbol_table_index - 1; i >= 0; i--) {
        if (strcmp(symbol_table[i].name, name) == 0 && symbol_table[i].level <= level && symbol_table[i].mark == 0) {
            return i;
        }
    }
    return -1;
}

int symbol_table_check_declaration(char *name, int level) {
    for (int i = symbol_table_index - 1; i >= 0; i--) {
        if (strcmp(symbol_table[i].name, name) == 0 && symbol_table[i].level == level) {
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

void mark_symbols(int level) {
    for (int i = symbol_table_index - 1; i >= 0; i--) {
        if (symbol_table[i].level == level) {
            symbol_table[i].mark = 1;
        }
    }
}

void print_symbol_table(FILE *outputFile) {
    printf("Symbol Table:\n");
    printf("Kind | Name       | Value | Level | Address | Mark\n");
    printf("---------------------------------------------------\n");
    fprintf(outputFile, "Symbol Table:\n");
    fprintf(outputFile, "Kind | Name       | Value | Level | Address | Mark\n");
    fprintf(outputFile, "---------------------------------------------------\n");
    for (int i = 0; i < symbol_table_index; i++) {
        printf("   %d | %10s | %5d | %5d | %5d | %5d\n", symbol_table[i].kind, symbol_table[i].name, symbol_table[i].val,
               symbol_table[i].level, symbol_table[i].addr, symbol_table[i].mark);
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
    printf("Generated Code:\n");
    printf("Line    OP    L    M\n");
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
        printf("%3d %6s %4d %4d\n", i, op, code[i].l, code[i].m);
        fprintf(outputFile, "%3d %6s %4d %4d\n", i, op, code[i].l, code[i].m);
    }
}

void output_elf_file() {
    FILE *elfFile = fopen("elf.txt", "w");
    if (!elfFile) {
        perror("Error opening elf.txt file");
        exit(1);
    }

    for (int i = 0; i < code_index; i++) {
        fprintf(elfFile, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
    }

    fclose(elfFile);
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
    FILE *outputFile = fopen("lexOutput.txt", "w");
    if (!outputFile) {
        perror("Error opening output file");
        return 1;
    }

    // Output the source program
    fprintf(outputFile, "Source Program:\n%s\n\n", sourceProgram);

    // Output the lexeme table
    fprintf(outputFile, "Lexeme Table:\n");
    fprintf(outputFile, "\nlexeme token type\n");
    for (int i = 0; i < lexemeCount; i++) {
        if (lexemes[i].token == error_token) {
            fprintf(outputFile, "%-15s %s\n", lexemes[i].lexeme, lexemes[i].errorMessage);
        } else {
            fprintf(outputFile, "%-15s %-5d\n", lexemes[i].lexeme, lexemes[i].token);
        }
    }

    // Output the token list
    fprintf(outputFile, "\nToken List:\n");
    for (int i = 0; i < tokenCount; i++) {
        fprintf(outputFile, "%d", tokens[i].token);
        if (tokens[i].token == identsym || tokens[i].token == numbersym) {
            fprintf(outputFile, " %s", tokens[i].value);
        }
        if (i < tokenCount - 1) {
            fprintf(outputFile, " ");
        }
    }
    fprintf(outputFile, "\n");

    // Close the output file
    fclose(outputFile);

    FILE *outputFile2 = fopen("output.txt", "w");
    if (!outputFile2) {
        perror("Error opening output file");
        return 1;
    }
    
    printf("Source Program:\n%s\n\n", sourceProgram);

    // Parse the program
    program(outputFile2);

    // Mark all symbols as unavailable
    mark_symbols(0);


    printf("No errors, program is syntactically correct\n\n");

    // Output the generated code
    print_code(outputFile2);
    printf("\n");

    output_elf_file();

    // Output the symbol table
    // print_symbol_table(outputFile2);

    // Close the output file
    fclose(outputFile2);

    return 0;
}
