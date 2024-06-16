#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 *  Issue Index:
 *  All known issues
 *  GOTO:
 *  - When there is only one value, it pops it and doesn't know what to do with itself
 *  - GOTO_NZ doesn't work when there isn't a 0 at the front, seem
 *  - When you use GOTO and there is a stack underflow error, it doesn't push any values that would have been in the stack already back in 
 * 
 *  NULL:
 *  - None of the NULL instructions work yet. 
*/



//Constants
#define MAX_STACK_SIZE 1024
typedef enum{
    //https://en.wikipedia.org/wiki/List_of_Java_bytecode_instructions

    //COMPLETED INSTRUCTIONS:
    INST_PUSH,          //push onto stack  
    INST_POP,           //pop off stack
    INST_ADD,           //add first 2 popped values from stack
    INST_SUB,           //subtract first 2 popped values from stack
    INST_MUL,           //multiply first 2 popped values from stack
    INST_DIV,           //divide first 2 popped values from stack
    INST_PRINT,         //print popped value
    INST_DUP,           //pops top, and then pushes back into stack twice
    INST_SWAP,          //swaps front 2 values around
    INST_COMPE,         //Compares, if they are equal, pop both and push in 1, else, push 0
    INST_COMPNE,        //Inverse of COMPE
    INST_COMPG,         //Compares if first value is greater than second value
    INST_COMPGE,        //compares if first value is greater than or equal to the second value
    INST_COMPL,         //compares if first value is less than second value
    INST_COMPLE,        //compares if first value is less than or equal to the second value
    INST_GOTO,          //Goes to the nearest selected value
    INST_NOP,           //No operation
    INST_POP2,          //pops the top two values on the stack, //!Note, pop2 does also return a underflow error if there aren't enough values on the stack
    INST_NEG,           //Negates operand
    INST_READ,          //Prints value top of stack, doesn't pop it off the stack fully
    INST_STOP,          //Ignores any further instructions

//!unfinished
    INST_PUSH_NULL,     //Pushes a null reference onto the stack
    INST_COMPE_NULL,    //Checks if top == null, if yes 1, if not 0
    INST_COMPNE_NULL,   //inverse of COMPNE_NULL
    INST_GOTO_Z,        //If top of stack == 0, go to operand
    INST_GOTO_NZ,       //If top of stack != 0, go to operand
} InstructionSet;


typedef struct{
    InstructionSet operator;    //e.g. push
    int operand;                //e.g. 2
} Instruction;


typedef struct{
    int stack[MAX_STACK_SIZE];
    int currentStackSize;
    size_t programSize;
    Instruction *instructions;
} Machine;

//List of Macros: ------------------------------------------------
#define M_INST_PUSH(x) {.operator = INST_PUSH, .operand = x}
#define M_INST_POP() {.operator = INST_POP}
#define M_INST_ADD() {.operator = INST_ADD}
#define M_INST_SUB() {.operator = INST_SUB}
#define M_INST_MUL() {.operator = INST_MUL}
#define M_INST_DIV() {.operator = INST_DIV}
#define M_INST_PRINT() {.operator = INST_PRINT}
#define M_INST_DUP() {.operator = INST_DUP}
#define M_INST_SWAP() {.operator = INST_SWAP}
#define M_INST_COMPE() {.operator = INST_COMPE}
#define M_INST_COMPNE() {.operator = INST_COMPNE}
#define M_INST_COMPG() {.operator = INST_COMPG}
#define M_INST_COMPGE() {.operator = INST_COMPGE}
#define M_INST_COMPL() {.operator = INST_COMPL}
#define M_INST_COMPLE() {.operator = INST_COMPLE}
#define M_INST_GOTO(x) {.operator = INST_GOTO, .operand = x}
#define M_INST_NOP() {.operator = INST_NOP}
#define M_INST_POP2() {.operator = INST_POP2}
#define M_INST_NEG() {.operator = INST_NEG}
#define M_INST_READ() {.operator = INST_READ}
#define M_INST_STOP() {.operator = INST_STOP}
#define M_INST_PUSH_NULL() {.operator = INST_PUSH_NULL}
#define M_INST_COMPE_NULL() {.operator = INST_COMPE_NULL}
#define M_INST_COMPNE_NULL() {.operator = INST_COMPNE_NULL}
#define M_INST_GOTO_Z(x) {.operator = INST_GOTO_Z, .operand = x}
#define M_INST_GOTO_NZ(x) {.operator = INST_GOTO_NZ, .operand = x}
//----------------------------------------------------------------

//running the program
Instruction program[] = {
    M_INST_PUSH(5),
    M_INST_PUSH(10),
    M_INST_PUSH(0),
    M_INST_STOP(),
    M_INST_PUSH(5),
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
int pop(Machine *machine){                  //pop the top
    if(machine->currentStackSize <= 0) {
        fprintf(stderr, "stack underflow\n");
        exit(EXIT_FAILURE);
    }
    machine->currentStackSize--;
    return machine->stack[machine->currentStackSize];
}

void printStack(Machine *machine){          //prints operands on the stack
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
    int gotoStore;     //Used for GOTO_Z and GOTO_NZ

    Machine *loadedMachine = malloc(sizeof(Machine) * MAX_STACK_SIZE);
    loadedMachine->instructions = program;
    writeProgramToFile(loadedMachine, "program.bin");
    loadedMachine = readFromFile(loadedMachine, "program.bin");

    //run program & instructions--------------------------------------------------------
    for(size_t i=0; i < loadedMachine->programSize; i++) {
        switch((loadedMachine->instructions)[i].operator){
            //PUSH
            case INST_PUSH:
                push(loadedMachine, loadedMachine->instructions[i].operand);
                break;

            //POP
            case INST_POP:
                pop(loadedMachine);
                break;

            //ADD
            case INST_ADD:
                push(loadedMachine, pop(loadedMachine) + pop(loadedMachine));
                break;

            //SUBTRACT
            case INST_SUB:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                push(loadedMachine, first - second);
                break;

            //MULTIPLY
            case INST_MUL:
                push(loadedMachine ,pop(loadedMachine) * pop(loadedMachine));
                break;

            //DIVIDE
            case INST_DIV:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                if(second == 0){
                    fprintf(stderr, "cant divide by 0\n");
                    exit(EXIT_FAILURE);
                }
                push(loadedMachine ,first / second);
                break;

            //PRINT
            case INST_PRINT:
                printf("%d\n", pop(loadedMachine));
                break;

            //DUPLICATE
            case INST_DUP:
                first = pop(loadedMachine);
                push(loadedMachine ,first);
                push(loadedMachine ,first);
                break;

            //SWAP
            case INST_SWAP:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                push(loadedMachine, first);
                push(loadedMachine, second);
                break;

            //COMPARE EQUAL
            case INST_COMPE:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                if(first == second){
                    push(loadedMachine, 1);
                }
                else{
                    push(loadedMachine, 0);
                }
                break;

            //COMPARE NOT EQUAL
            case INST_COMPNE:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                if(first == second){
                    push(loadedMachine, 0);
                }
                else{
                    push(loadedMachine, 1);
                }
                break;  

            //COMPARE GREATER THAN
            case INST_COMPG:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                if(first > second){
                    push(loadedMachine, 1);
                }
                else{
                    push(loadedMachine, 0);
                }
                break;  

            //COMPARE GREATER THAN OR EQUAL TO
            case INST_COMPGE:
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                if(first >= second){
                    push(loadedMachine, 1);
                }
                else{
                    push(loadedMachine, 0);
                }
                break;

            //COMPARE LESS THAN
            case INST_COMPL: 
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                if(first < second){
                    push(loadedMachine, 1);
                }
                else{
                    push(loadedMachine, 0);
                }
                break;

            //COMPARE LESS THAN OR EQUAL TO
            case INST_COMPLE:  
                first = pop(loadedMachine);
                second = pop(loadedMachine);
                if(first <= second){
                    push(loadedMachine, 1);
                }
                else{
                    push(loadedMachine, 0);
                }
                break; 

            //GOTO
            case INST_GOTO:
                first = loadedMachine->instructions[i].operand;
                second = pop(loadedMachine);
                //push(loadedMachine, second);
                for(int gotoIndex = 0; gotoIndex < loadedMachine->currentStackSize;){
                    printf("first = %d, second = %d\n", first, second);
                    if(first != second){
                        printf("First != second loop\n");
                        second = pop(loadedMachine);
                    }
                    if(first == second){
                        printf("First == second loop\n");
                        push(loadedMachine, second);
                        break;
                    }
                    else{
                        fprintf(stderr,"out of bounds\n");
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            
            //NO OPERATION
            case INST_NOP:
                //null
                break;

            //POP2
            case INST_POP2:
                pop(loadedMachine);
                pop(loadedMachine);
                break;

            //NEGATE
            case INST_NEG:
                first = pop(loadedMachine);
                push(loadedMachine, -first);
                break;

            //READ
            case INST_READ:
                first = pop(loadedMachine);
                printf("top of stack is %d\n", first);
                push(loadedMachine, first);
                break;

            //STOP
            case INST_STOP:
                i = loadedMachine->programSize;
                break;

//!----------------------------------------------------------------------------------------------------------------
            //PUSH NULL REFERENCE ONTO STACK
            case INST_PUSH_NULL:
                push(loadedMachine, NULL);
                break;

            //COMPARE NULL
            case INST_COMPE_NULL:
                first = pop(loadedMachine);
                if(first == NULL){
                    push(loadedMachine, 1);
                }
                else{
                    push(loadedMachine, 0);
                }
                break;

            //COMPARE NOT NULL
            case INST_COMPNE_NULL:
                first = pop(loadedMachine);
                if(first == NULL){
                    push(loadedMachine, 0);
                }
                else{
                    push(loadedMachine, 1);
                }
                break;

            //GOTO IF ZERO
            case INST_GOTO_Z:
                gotoStore = pop(loadedMachine);
                printf("Popped first value and stored it as %d\n", gotoStore);
                if(gotoStore == 0){
                    //COPY GOTO
                    first = loadedMachine->instructions[i].operand;
                    second = pop(loadedMachine);
                    for(int gotoIndex = 0; gotoIndex < loadedMachine->currentStackSize;){
                        if(first != second){
                            printf("First != second loop\n");
                            second = pop(loadedMachine);
                        }
                        if(first == second){
                            printf("First == second loop\n");
                            push(loadedMachine, second);
                            break;
                        }
                        else{
                            fprintf(stderr,"out of bounds\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                else{
                    //PUSH POPPED VALUE BACK IN
                    push(loadedMachine, gotoStore);
                }
                break;

            //GOTO IF NOT ZERO
            case INST_GOTO_NZ:
                gotoStore = pop(loadedMachine);
                printf("Popped first value and stored it as %d\n", gotoStore);
                if(gotoStore == 0){
                    push(loadedMachine, gotoStore);
                }
                else{
                    first = loadedMachine->instructions[i].operand;
                    second = pop(loadedMachine);
                    for(int gotoIndex = 0; gotoIndex < loadedMachine->currentStackSize;){
                        if(first != second){
                            printf("First != second loop\n");
                            second = pop(loadedMachine);
                        }
                        if(first == second){
                            printf("First == second loop\n");
                            push(loadedMachine, second);
                            break;
                        }
                        else{
                            fprintf(stderr,"out of bounds\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                break;
//!-------------------------------------------------------------------------------------------------------------------------------
            //DEFAULT
            default://unexpected state
                assert(0);
                break;
        }
    }
    //END-OF-SWITCH-AND-FOR-LOOP--------------------------------------------------------

    printStack(loadedMachine);
    return 0;
}