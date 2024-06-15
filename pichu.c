#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//Constants
#define MAX_STACK_SIZE 1024
typedef enum{
    INST_PUSH,  
    INST_POP,   
    INST_ADD,   
    INST_SUB,   
    INST_MUL,   
    INST_DIV,   
    INST_PRINT, 
} instructionSet;

typedef struct{
    instructionSet type;
    int operand;
} instruction;

typedef struct{
    //where stack is stored
    int stack[MAX_STACK_SIZE];
    int currentStackSize;
    size_t programSize;
    //instruction program;
    instruction *instructions[];
} Machine;

//macros
#define M_INST_PUSH(x) {.type = INST_PUSH, .operand = x}//only one that needs value!
#define M_INST_POP() {.type = INST_POP}
#define M_INST_ADD() {.type = INST_ADD}
#define M_INST_SUB() {.type = INST_SUB}
#define M_INST_MUL() {.type = INST_MUL}
#define M_INST_DIV() {.type = INST_DIV}
#define M_INST_PRINT() {.type = INST_PRINT}

instruction program[] = {
    M_INST_PUSH(6),
    M_INST_PUSH(7),
    M_INST_ADD(),
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
    else{
        fwrite(machine->instructions, sizeof(machine->instructions[0]), PROGRAM_SIZE, f);
        printf("program written to file %s\n", filename);
    }
    fclose(f);
}

Machine *readFromFile(Machine *machine, char *filename){
    FILE *f = fopen(filename,"r");
    if(f == NULL){
        fprintf(stderr, "could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    else{
        instruction *instructions = malloc(sizeof(instruction) * MAX_STACK_SIZE);
        if (instructions == NULL) {
            fprintf(stderr, "memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        Machine *machine = malloc(sizeof(Machine));
        if (machine == NULL) {
            fprintf(stderr, "memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        fseek(f, 0, SEEK_END);
        int length = ftell(f);
        fread(instructions, sizeof(instruction), length/sizeof(instruction), f);
        printf("program read from file %s\n", filename);

        machine->programSize = length/sizeof(instruction);
        return machine;
    }
    fclose(f);
}

int main(){
    int first;
    int second;
    Machine *loadedMachine = malloc(sizeof(Machine));
    *loadedMachine->instructions = malloc(sizeof(program));
    memcpy(loadedMachine->instructions, program, sizeof(program));
    writeProgramToFile(loadedMachine, "program.bin");
    loadedMachine = readFromFile(loadedMachine, "program.bin");
    for(size_t i=0; i < loadedMachine->programSize; i++) {
        switch((*loadedMachine->instructions)[i].type){
            case INST_PUSH:
                push(loadedMachine, (*loadedMachine->instructions)[i].operand);
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
                push(loadedMachine, second - first);
                break;
            case INST_MUL:
                push(loadedMachine ,pop(loadedMachine) * pop(loadedMachine));
                break;
            case INST_DIV:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                push(loadedMachine ,second / first);
                break;
            case INST_PRINT:
                printf("%d\n", pop(loadedMachine));
                break;
            default://unexpected state
                assert(0);
                break;
        }
    }
    printStack(loadedMachine);
    return 0;
}