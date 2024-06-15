#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//Constants
#define MAX_STACK_SIZE 1024
typedef enum{
    INST_PUSH,  //push onto stack  
    INST_POP,   //pop off stack
    INST_ADD,   //add first 2 popped values from stack
    INST_SUB,   //subtract first 2 popped values from stack
    INST_MUL,   //multiply first 2 popped values from stack
    INST_DIV,   //divide first 2 popped values from stack
    INST_PRINT, //print popped value
    INST_DUP,   //pops top, and then pushes back into stack twice
    INST_SWAP,  //swaps front 2 values around
} InstructionSet;


typedef struct{
    InstructionSet operator; //e.g. push
    int operand;         //e.g. 5
} Instruction;


typedef struct{
    int stack[MAX_STACK_SIZE];
    int currentStackSize;
    size_t programSize;
    Instruction *instructions;
} Machine;

//macros
#define M_INST_PUSH(x) {.operator = INST_PUSH, .operand = x}//only one that needs value!
#define M_INST_POP() {.operator = INST_POP}
#define M_INST_ADD() {.operator = INST_ADD}
#define M_INST_SUB() {.operator = INST_SUB}
#define M_INST_MUL() {.operator = INST_MUL}
#define M_INST_DIV() {.operator = INST_DIV}
#define M_INST_PRINT() {.operator = INST_PRINT}
#define M_INST_DUP() {.operator = INST_DUP}
#define M_INST_SWAP() {.operator = INST_SWAP}

//running the program
Instruction program[] = {
    M_INST_PUSH(5),
    M_INST_PUSH(10),
    M_INST_SWAP(),
    M_INST_SUB(),
};
#define PROGRAM_SIZE (sizeof(program)/sizeof(program[0]))

void push(Machine *machine, int operand){   //push value
    if(machine->currentStackSize >= MAX_STACK_SIZE){
        fprintf(stderr, "stack overflow\n");
        exit(EXIT_FAILURE);
    }
    machine->stack[machine->currentStackSize] = operand;
    machine->currentStackSize++;
}
int pop(Machine *machine){             //pop the top
    if(machine->currentStackSize <= 0) {
        fprintf(stderr, "stack underflow\n");
        exit(EXIT_FAILURE);
    }
    machine->currentStackSize--;
    return machine->stack[machine->currentStackSize];
}

void printStack(Machine *machine){
    printf("stack:\n");
    for(int i = machine->currentStackSize - 1; i>=0; i--){
        printf("%d\n", machine->stack[i]);
    }
    printf("------\n");
}

void writeProgramToFile(Machine *machine, char *filename){
    FILE *f = fopen(filename, "wb");
    if(f == NULL){
        fprintf(stderr, "could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fwrite(machine->instructions, sizeof(machine->instructions[0]), PROGRAM_SIZE, f);
    printf("program written to file %s\n", filename);

    fclose(f);
}

Machine *readFromFile(Machine *machine, char *filename){
    FILE *f = fopen(filename,"r");

    if(f == NULL){
        fprintf(stderr, "could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    Instruction *instructions = malloc(sizeof(Instruction) * MAX_STACK_SIZE);
    if (instructions == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    else if (machine == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f, 0, SEEK_SET);
    fread(instructions, sizeof(instructions[0]), length/8, f);
    printf("program read from file %s\n", filename);

    machine->programSize = length/8;
    machine->instructions = instructions;
        
    fclose(f);
    return machine;
}

int main(){
    int first, second; //first and second operands, used for division and subtraction

    Machine *loadedMachine = malloc(sizeof(Machine) * MAX_STACK_SIZE);
    loadedMachine->instructions = program;
    writeProgramToFile(loadedMachine, "program.bin");
    loadedMachine = readFromFile(loadedMachine, "program.bin");

    //run program
    for(size_t i=0; i < loadedMachine->programSize; i++) {
        switch((loadedMachine->instructions)[i].operator){
            case INST_PUSH:
                push(loadedMachine, loadedMachine->instructions[i].operand);
                break;
            case INST_POP:
                pop(loadedMachine);
                break;
            case INST_ADD:
                push(loadedMachine, pop(loadedMachine) + pop(loadedMachine));
                break;
            case INST_SUB:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                push(loadedMachine, first - second);
                break;
            case INST_MUL:
                push(loadedMachine ,pop(loadedMachine) * pop(loadedMachine));
                break;
            case INST_DIV:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                if(second == 0){
                    fprintf(stderr, "cant divide by 0\n");
                    exit(EXIT_FAILURE);
                }
                push(loadedMachine ,first / second);
                break;
            case INST_PRINT:
                printf("%d\n", pop(loadedMachine));
                break;
            case INST_DUP:
                first = pop(loadedMachine);
                push(loadedMachine ,first);
                push(loadedMachine ,first);
                break;
            case INST_SWAP:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                push(loadedMachine, first);
                push(loadedMachine, second);
                break;
            default://unexpected state
                assert(0);
                break;
        }
    }
    printStack(loadedMachine);
    return 0;
}