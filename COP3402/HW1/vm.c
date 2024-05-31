//
//  vm.c
//  Homework1
//
//  Created by Alperen Yazmaci on 05.25.2024
//

//Header Files
#include <stdio.h>
#include <stdlib.h>

//Define the max ARRAY_SIZE to 500
#define ARRAY_SIZE 500

//Struct Aliases
typedef struct Instruction Instruction;

//Struct definition to store Instruction
struct Instruction{
    int OP;
    int L;
    int M;
};

//Define the P-Machine registers
int BP = 499;
int SP = 500;
int PC = 10;
Instruction IR;
int STACK[ARRAY_SIZE] = {0};
int HALT = 1;

//Function to Find base L levels down
int base(int BP, int L) {
    int arb = BP;
    while (L > 0) {
        arb = STACK[arb];
        L--;
    }
    return arb;
}

//Function that prints the current state of the machine
void print_state(const char *instr_name, int L, int M) {
    printf("%5s %2d %2d %9d %6d %5d  ", instr_name, L, M, PC, BP, SP);

    int current_bp = BP;
    for (int i = 499; i >= SP; i--) {
        if (i == current_bp && current_bp != 499) {
            printf("| ");
            current_bp = STACK[current_bp + 1];
        }
        printf("%d ", STACK[i]);
    }
    printf("\n");
}




int main(int argc, char *argv[]) {
    if (argc != 2) { //exiting if there is no input file
        return 1;
    }

    //getting input file and exiting the program if the file wasn't opened
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        return 1;
    }

    //load instructions into the STACK starting at index 10 then close the file after all instructions are loaded
    int idx = 10;
    while (fscanf(file, "%d %d %d", &STACK[idx], &STACK[idx + 1], &STACK[idx + 2]) != EOF) {
        idx += 3;
    }
    fclose(file);

    //print the header output lines
    printf("%15s %5s %5s %5s %10s\n", "", "PC", "BP", "SP", "Stack");
    printf("Initial values: %5d %6d %5d\n", PC, BP, SP);

    //loop to fetch and execute instructions in the stack
    while (HALT) {
        //fetch cycle
        IR.OP = STACK[PC];
        IR.L = STACK[PC + 1];
        IR.M = STACK[PC + 2];
        PC += 3;

        //determining instruction name then assigning it to instr_name
        const char *instr_name;
        switch (IR.OP) {
            case 1: instr_name = "LIT"; break;
            case 2: instr_name = "OPR"; break;
            case 3: instr_name = "LOD"; break;
            case 4: instr_name = "STO"; break;
            case 5: instr_name = "CAL"; break;
            case 6: instr_name = "INC"; break;
            case 7: instr_name = "JMP"; break;
            case 8: instr_name = "JPC"; break;
            case 9: instr_name = "SYS"; break;
            default: instr_name = "???"; break;
        }

        //execute cycle
        switch (IR.OP) {
            case 1: //LIT 0 M
                SP--;
                STACK[SP] = IR.M;
                break;
            case 2: //OPR 0 M
                switch (IR.M) {
                    case 0: //RTN
                        SP = BP + 1;
                        BP = STACK[SP - 2];
                        PC = STACK[SP - 3];
                        break;
                    case 1: //ADD
                        STACK[SP + 1] = STACK[SP + 1] + STACK[SP];
                        SP++;
                        break;
                    case 2: //SUB
                        STACK[SP + 1] = STACK[SP + 1] - STACK[SP];
                        SP++;
                        break;
                    case 3: //MUL
                        STACK[SP + 1] = STACK[SP + 1] * STACK[SP];
                        SP++;
                        break;
                    case 4: //DIV
                        STACK[SP + 1] = STACK[SP + 1] / STACK[SP];
                        SP++;
                        break;
                    case 5: //EQL
                        STACK[SP + 1] = STACK[SP + 1] == STACK[SP];
                        SP++;
                        break;
                    case 6: //NEQ
                        STACK[SP + 1] = STACK[SP + 1] != STACK[SP];
                        SP++;
                        break;
                    case 7: //LSS
                        STACK[SP + 1] = STACK[SP + 1] < STACK[SP];
                        SP++;
                        break;
                    case 8: //LEQ
                        STACK[SP + 1] = STACK[SP + 1] <= STACK[SP];
                        SP++;
                        break;
                    case 9: //GTR
                        STACK[SP + 1] = STACK[SP + 1] > STACK[SP];
                        SP++;
                        break;
                    case 10: //GEQ
                        STACK[SP + 1] = STACK[SP + 1] >= STACK[SP];
                        SP++;
                        break;
                }
                break;
            case 3: //LOD L M
                SP--;
                STACK[SP] = STACK[base(BP, IR.L) - IR.M];
                break;
            case 4: //STO L M
                STACK[base(BP, IR.L) - IR.M] = STACK[SP];
                SP++;
                break;
            case 5: //CAL L M
                STACK[SP - 1] = base(BP, IR.L);
                STACK[SP - 2] = BP;
                STACK[SP - 3] = PC;
                BP = SP - 1;
                PC = IR.M;
                break;
            case 6: //INC 0 M
                SP -= IR.M;
                break;
            case 7: //JMP 0 M
                PC = IR.M;
                break;
            case 8: //JPC 0 M
                if (STACK[SP] == 0) {
                    PC = IR.M;
                }
                SP++;
                break;
            case 9: //SYS 0 M
                switch (IR.M) {
                    case 1:
                        printf("Output result is: %d\n", STACK[SP]);
                        SP++;
                        break;
                    case 2:
                        SP--;
                        printf("Please enter an integer: ");
                        scanf("%d", &STACK[SP]);
                        break;
                    case 3:
                        HALT = 0;
                        break;
                }
                break;
        }

        //print the output after fetch-execute cycle is done
        print_state(instr_name, IR.L, IR.M);
    }

    //exit the program
    return 0;
}
