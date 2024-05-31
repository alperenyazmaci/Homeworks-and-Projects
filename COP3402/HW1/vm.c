#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 500

// Define the structure for an instruction
typedef struct {
    int OP;
    int L;
    int M;
} Instruction;

// Define the P-Machine registers
int BP = 499;
int SP = 500;
int PC = 10;
Instruction IR;
int PAS[ARRAY_SIZE] = {0};
int HALT = 1;

// Function declarations
int base(int BP, int L);
void print_state(const char *instr_name, int L, int M);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s input.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Load instructions into PAS starting at location 10
    int idx = 10;
    while (fscanf(file, "%d %d %d", &PAS[idx], &PAS[idx + 1], &PAS[idx + 2]) != EOF) {
        idx += 3;
    }
    fclose(file);

    printf("PC BP SP Stack\n");
    printf("Initial values: %d %d %d\n", PC, BP, SP);

    // Main loop to fetch and execute instructions
    while (HALT) {
        // Fetch cycle
        IR.OP = PAS[PC];
        IR.L = PAS[PC + 1];
        IR.M = PAS[PC + 2];
        PC += 3;

        // Determine instruction name for printing
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

        // Execute cycle
        switch (IR.OP) {
            case 1: // LIT 0 M
                SP--;
                PAS[SP] = IR.M;
                break;
            case 2: // OPR 0 M
                switch (IR.M) {
                    case 0: // RTN
                        SP = BP + 1;
                        BP = PAS[SP - 2];
                        PC = PAS[SP - 3];
                        break;
                    case 1: // ADD
                        PAS[SP + 1] = PAS[SP + 1] + PAS[SP];
                        SP++;
                        break;
                    case 2: // SUB
                        PAS[SP + 1] = PAS[SP + 1] - PAS[SP];
                        SP++;
                        break;
                    case 3: // MUL
                        PAS[SP + 1] = PAS[SP + 1] * PAS[SP];
                        SP++;
                        break;
                    case 4: // DIV
                        PAS[SP + 1] = PAS[SP + 1] / PAS[SP];
                        SP++;
                        break;
                    case 5: // EQL
                        PAS[SP + 1] = PAS[SP + 1] == PAS[SP];
                        SP++;
                        break;
                    case 6: // NEQ
                        PAS[SP + 1] = PAS[SP + 1] != PAS[SP];
                        SP++;
                        break;
                    case 7: // LSS
                        PAS[SP + 1] = PAS[SP + 1] < PAS[SP];
                        SP++;
                        break;
                    case 8: // LEQ
                        PAS[SP + 1] = PAS[SP + 1] <= PAS[SP];
                        SP++;
                        break;
                    case 9: // GTR
                        PAS[SP + 1] = PAS[SP + 1] > PAS[SP];
                        SP++;
                        break;
                    case 10: // GEQ
                        PAS[SP + 1] = PAS[SP + 1] >= PAS[SP];
                        SP++;
                        break;
                }
                break;
            case 3: // LOD L M
                SP--;
                PAS[SP] = PAS[base(BP, IR.L) - IR.M];
                break;
            case 4: // STO L M
                PAS[base(BP, IR.L) - IR.M] = PAS[SP];
                SP++;
                break;
            case 5: // CAL L M
                PAS[SP - 1] = base(BP, IR.L); // Static link
                PAS[SP - 2] = BP; // Dynamic link
                PAS[SP - 3] = PC; // Return address
                BP = SP - 1;
                PC = IR.M;
                break;
            case 6: // INC 0 M
                SP -= IR.M;
                break;
            case 7: // JMP 0 M
                PC = IR.M;
                break;
            case 8: // JPC 0 M
                if (PAS[SP] == 0) {
                    PC = IR.M;
                }
                SP++;
                break;
            case 9: // SYS 0 M
                switch (IR.M) {
                    case 1:
                        printf("Output result is: %d\n", PAS[SP]);
                        SP++;
                        break;
                    case 2:
                        SP--;
                        printf("Please enter an integer: ");
                        scanf("%d", &PAS[SP]);
                        break;
                    case 3:
                        HALT = 0;
                        break;
                }
                break;
        }

        // Print the state after execution
        print_state(instr_name, IR.L, IR.M);
    }

    return 0;
}

// Function to find base L levels down
int base(int BP, int L) {
    int arb = BP;
    while (L > 0) {
        arb = PAS[arb];
        L--;
    }
    return arb;
}

// Print the current state of the machine
void print_state(const char *instr_name, int L, int M) {
    printf("%s %d %d %d %d %d ", instr_name, L, M, PC, BP, SP);

    // Print stack up to SP
    int current_bp = BP;
    for (int i = 499; i >= SP; i--) {
        if (i == current_bp && current_bp != 499) {
            printf("| ");
            current_bp = PAS[current_bp + 1];
        }
        printf("%d ", PAS[i]);
    }
    printf("\n");
}
