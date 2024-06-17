#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pichu.h"

//running the program
Instruction program[] = {
    M_INST_PUSH(1),
    M_INST_PUSH(4),
    M_INST_PUSH(6),
    M_INST_PUSH(8),
    M_INST_INDEX_DUP(0),
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

void IndexSwap(Machine *machine, int index){
    if(index > machine->currentStackSize || index < 0){
        fprintf(stderr, "index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    int temp = machine->stack[index];
    machine->stack[index] = pop(machine);
    push(machine,temp);
}

void IndexDuplicate(Machine *machine, int index){
    if(index > machine->currentStackSize || index < 0){
        fprintf(stderr, "index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    push(machine, machine->stack[index]);
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
    int stackStore[MAX_STACK_SIZE]; //used to store previous values within the stack to restore during GOTO operation failures

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

            //CLEAR
            case INST_CLEAR:
                while(loadedMachine->currentStackSize > 0){
                    pop(loadedMachine);
                }
                break;
            
            //CONSTANT -1
            case INST_CONST_M1:
                push(loadedMachine, -1);
                break;

            //CONSTANT 0
            case INST_CONST_0:
                push(loadedMachine, 0);
                break;

            //CONSTANT 1
            case INST_CONST_1:
                push(loadedMachine, 1);
                break;

            //CONSTANT 2
            case INST_CONST_2:
                push(loadedMachine, 2);
                break;

            //CONSTANT 3
            case INST_CONST_3:
                push(loadedMachine, 3);
                break;

            //CONSTANT 4
            case INST_CONST_4:
                push(loadedMachine, 4);
                break;

            //CONSTANT 5 
            case INST_CONST_5:
                push(loadedMachine, 5);
                break;


/*//!-----------------------------------------------------------------
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
*///!-----------------------------------------------------------------

            //GOTO
            //NOTE: I have commented out the printf checks for this instruction
            case INST_GOTO:
                first = loadedMachine->instructions[i].operand;
                second = pop(loadedMachine);
                int popIterator =0;
                stackStore[0] = second; //stores initial popped value
                //printf("stackStore[0] = %d\n", stackStore[0]);
                for(int gotoIndex = 0; gotoIndex < loadedMachine->currentStackSize; popIterator++){
                    //printf("first = %d, second = %d\n", first, second);
                    if(first != second){
                        //printf("First != second loop\n");
                        second = pop(loadedMachine);
                        stackStore[popIterator+1] = second;
                        //printf("%d\n", stackStore[popIterator]);
                    }
                    else if(first == second){
                        //printf("First == second loop\n");
                        push(loadedMachine, second);
                        break; //want to break here so it doesn't readd the stack
                    }
                    else{
                        fprintf(stderr,"out of bounds\n");
                        //exit loop and go into the other loop outside the for loop
                        //exit(EXIT_FAILURE);
                    }
                }
                //this re adds the values back into the stack
                if(first != second) {
                //printf("popIterator = %d\n", popIterator);
                    for(int forLoopIterator = popIterator; forLoopIterator > -1; forLoopIterator--){
                        printf("for loop iterator :%d\n", forLoopIterator);
                        push(loadedMachine, stackStore[forLoopIterator]);
                    }
                }
                //printf("exited for loop\n");
                break;

            //GOTO IF ZERO
            case INST_GOTO_Z:
                gotoStore = pop(loadedMachine);
                printf("Popped first value and stored it as %d\n", gotoStore);
                if(gotoStore == 0){
                    //COPY GOTO
                    first = loadedMachine->instructions[i].operand;
                    second = pop(loadedMachine);
                    int popIterator =0;
                    stackStore[0] = second; //stores initial popped value
                    //printf("stackStore[0] = %d\n", stackStore[0]);
                    for(int gotoIndex = 0; gotoIndex < loadedMachine->currentStackSize; popIterator++){
                        //printf("first = %d, second = %d\n", first, second);
                        if(first != second){
                            //printf("First != second loop\n");
                            second = pop(loadedMachine);
                            stackStore[popIterator+1] = second;
                            //printf("%d\n", stackStore[popIterator]);
                        }
                        else if(first == second){
                            //printf("First == second loop\n");
                            push(loadedMachine, second);
                            break; //want to break here so it doesn't readd the stack
                        }
                        else{
                            fprintf(stderr,"out of bounds\n");
                            //exit loop and go into the other loop outside the for loop
                            //exit(EXIT_FAILURE);
                        }
                    }
                    //this re adds the values back into the stack
                    if(first != second) {
                    //printf("popIterator = %d\n", popIterator);
                        for(int forLoopIterator = popIterator; forLoopIterator > -1; forLoopIterator--){
                            printf("for loop iterator :%d\n", forLoopIterator);
                            push(loadedMachine, stackStore[forLoopIterator]);
                        }
                        push(loadedMachine, gotoStore);
                    }
                    //printf("exited for loop\n");
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
                if(gotoStore != 0){
                    //COPY GOTO
                    first = loadedMachine->instructions[i].operand;
                    second = pop(loadedMachine);
                    int popIterator = 0;
                    stackStore[0] = second; //stores initial popped value
                    //printf("stackStore[0] = %d\n", stackStore[0]);
                    for(int gotoIndex = 0; gotoIndex < loadedMachine->currentStackSize; popIterator++){
                        //printf("first = %d, second = %d\n", first, second);
                        if(first != second){
                            //printf("First != second loop\n");
                            second = pop(loadedMachine);
                            stackStore[popIterator+1] = second;
                            //printf("%d\n", stackStore[popIterator]);
                        }
                        else if(first == second){
                            //printf("First == second loop\n");
                            push(loadedMachine, second);
                            break; //want to break here so it doesn't readd the stack
                        }
                        else{
                            fprintf(stderr,"out of bounds\n");
                            //exit loop and go into the other loop outside the for loop
                            //exit(EXIT_FAILURE);
                        }
                    }
                    //this re adds the values back into the stack
                    if(first != second) {
                    //printf("popIterator = %d\n", popIterator);
                        for(int forLoopIterator = popIterator; forLoopIterator > -1; forLoopIterator--){
                            printf("for loop iterator :%d\n", forLoopIterator);
                            push(loadedMachine, stackStore[forLoopIterator]);
                        }
                        push(loadedMachine, gotoStore);
                    }
                    //printf("exited for loop\n");
                }
                else{
                    //PUSH POPPED VALUE BACK IN
                    push(loadedMachine, gotoStore);
                }
                break;

            //REPLACE WITH
            case INST_CHANGE_TO:
                int changeTo = loadedMachine->instructions[i].operand;
                printf("Change to is %d\n", changeTo);
                break;

            //MODIFY
            case INST_MOD:
                int mod = loadedMachine->instructions[i].operand;
                printf("Modifying %d to %d\n", mod, changeTo);

                //first = pop(loadedMachine);
                int modifyStore[MAX_STACK_SIZE];
                modifyStore[0] = pop(loadedMachine);
                first = modifyStore[0];
                printf("First outside pop = %d\n", first);

                printf("ModifyStore %d\n", modifyStore[0]);

                int stackIterator = 1;
                for(int modLoop = loadedMachine->currentStackSize; modLoop != 0; modLoop--, stackIterator++){
                    first = pop(loadedMachine);
                    printf("stackIterator = %d\n", stackIterator);
                    modifyStore[stackIterator] = first;
                    printf("%d\n", first);
                    printf("ModifyStore %d\n", modifyStore[stackIterator]);

                    if(first == mod){
                        printf("in first == mod, %d\n", first);
                        push(loadedMachine, changeTo);
                        //push the stored stack back
                        for(int i = stackIterator; i != 0; i--){
                            printf("pushing %d at %d\n", modifyStore[i-1], i-1);
                            push(loadedMachine, modifyStore[i-1]);
                        }
                        break;
                    }
                    else{
                        //Do nothing
                    }
                }
                //read everything back into stack when nothing found
                if(first != mod){
                    for(int forLoopIterator = stackIterator; forLoopIterator > 0; forLoopIterator--){
                        printf("for loop iterator :%d\n", forLoopIterator);
                        push(loadedMachine, modifyStore[forLoopIterator-1]);
                    }                
                }
                printf("Exited loop\n");
                break; 

            //MODIFY ALL
            case INST_MOD_ALL:
                int modAll = loadedMachine->instructions[i].operand;
                printf("Modifying all %d to %d\n", modAll, changeTo);
                int modifyAllStore[MAX_STACK_SIZE];

                first = pop(loadedMachine);
                modifyAllStore[0] = first;
                printf("First outside pop = %d/%d\n", first, modifyAllStore[0]);

                stackIterator = 1;
                for(int modLoop = loadedMachine->currentStackSize; modLoop != 0; modLoop--, stackIterator++){
                    first = pop(loadedMachine);
                    printf("stackIterator = %d\n", stackIterator);
                    modifyAllStore[stackIterator] = first;
                    printf("%d\n", first);
                    printf("ModifyStore %d\n", modifyAllStore[stackIterator]);   

                    if(first == modAll){
                        printf("first==modAll loop ,first:%d\n", first);
                        modifyAllStore[stackIterator] = changeTo;
                        printf("ModifyStore %d\n", modifyAllStore[stackIterator]);
                    }
                    else{
                        //Do nothing
                    }
                }
                //add everything back into the stack
                for(int i = stackIterator; i > 0; i--){
                    printf("Entered 2nd for loop");
                    push(loadedMachine, modifyAllStore[i-1]);
                }
                printf("Exited loop\n");
                break; 

            //FLIPS STACK
            case INST_FLIP:
                int flipStore[MAX_STACK_SIZE];
                stackIterator = 0;

                //puts stack into flipStore
                for(int i = loadedMachine->currentStackSize; i != 0; i--, stackIterator++){
                    flipStore[stackIterator] = pop(loadedMachine);
                    printf("%d\n", flipStore[stackIterator]);
                }

                //puts flipStore back into stack
                for(int i = 1; i != stackIterator+1; i++){
                    printf("pushing %d at %d\n", flipStore[i-1], i-1);
                    push(loadedMachine, flipStore[i-1]);
                }
                break;

            //GOTO CONDITION
            case INST_CONDITION:
                int condition = loadedMachine->instructions[i].operand;
                printf("GOTO condition is set to %d\n", condition);
                break;

            //GOTO IF CONDITION = TRUE
            case INST_GOTO_C:
                gotoStore = pop(loadedMachine);
                printf("Popped first value and stored it as %d\n", gotoStore);
                if(gotoStore == condition){
                    //COPY GOTO
                    first = loadedMachine->instructions[i].operand;
                    second = pop(loadedMachine);
                    int popIterator =0;
                    stackStore[0] = second; //stores initial popped value
                    //printf("stackStore[0] = %d\n", stackStore[0]);
                    for(int gotoIndex = 0; gotoIndex < loadedMachine->currentStackSize; popIterator++){
                        //printf("first = %d, second = %d\n", first, second);
                        if(first != second){
                            //printf("First != second loop\n");
                            second = pop(loadedMachine);
                            stackStore[popIterator+1] = second;
                            //printf("%d\n", stackStore[popIterator]);
                        }
                        else if(first == second){
                            //printf("First == second loop\n");
                            push(loadedMachine, second);
                            break; //want to break here so it doesn't readd the stack
                        }
                        else{
                            fprintf(stderr,"out of bounds\n");
                            //exit loop and go into the other loop outside the for loop
                            //exit(EXIT_FAILURE);
                        }
                    }
                    //this re adds the values back into the stack
                    if(first != second) {
                    //printf("popIterator = %d\n", popIterator);
                        for(int forLoopIterator = popIterator; forLoopIterator > -1; forLoopIterator--){
                            printf("for loop iterator :%d\n", forLoopIterator);
                            push(loadedMachine, stackStore[forLoopIterator]);
                        }
                        push(loadedMachine, gotoStore);
                    }
                    //printf("exited for loop\n");
                }
                else{
                    //PUSH POPPED VALUE BACK IN
                    push(loadedMachine, gotoStore);
                }
                break;

            //GOTO IF CONDITION = FALSE
            case INST_GOTO_NC:
                gotoStore = pop(loadedMachine);
                printf("Popped first value and stored it as %d\n", gotoStore);
                if(gotoStore != condition){
                    //COPY GOTO
                    first = loadedMachine->instructions[i].operand;
                    second = pop(loadedMachine);
                    int popIterator = 0;
                    stackStore[0] = second; //stores initial popped value
                    //printf("stackStore[0] = %d\n", stackStore[0]);
                    for(int gotoIndex = 0; gotoIndex < loadedMachine->currentStackSize; popIterator++){
                        //printf("first = %d, second = %d\n", first, second);
                        if(first != second){
                            //printf("First != second loop\n");
                            second = pop(loadedMachine);
                            stackStore[popIterator+1] = second;
                            //printf("%d\n", stackStore[popIterator]);
                        }
                        else if(first == second){
                            //printf("First == second loop\n");
                            push(loadedMachine, second);
                            break; //want to break here so it doesn't readd the stack
                        }
                        else{
                            fprintf(stderr,"out of bounds\n");
                            //exit loop and go into the other loop outside the for loop
                            //exit(EXIT_FAILURE);
                        }
                    }
                    //this re adds the values back into the stack
                    if(first != second) {
                    //printf("popIterator = %d\n", popIterator);
                        for(int forLoopIterator = popIterator; forLoopIterator > -1; forLoopIterator--){
                            printf("for loop iterator :%d\n", forLoopIterator);
                            push(loadedMachine, stackStore[forLoopIterator]);
                        }
                        push(loadedMachine, gotoStore);
                    }
                    //printf("exited for loop\n");
                }
                else{
                    //PUSH POPPED VALUE BACK IN
                    push(loadedMachine, gotoStore);
                }                
                break;

            //INCREMENT TOP
            case INST_INC:
                first = pop(loadedMachine);
                first++;
                push(loadedMachine, first);
                break;

            //DECREMENT TOP
            case INST_DEC:
                first = pop(loadedMachine);
                first--;
                push(loadedMachine, first);
                break;

            //INCREMENT ALL
            case INST_INC_ALL:
                int incAllStore[MAX_STACK_SIZE];
                stackIterator = 0;
                for(int i = loadedMachine->currentStackSize; i != 0; i--, stackIterator++){
                    incAllStore[stackIterator] = pop(loadedMachine) + 1;
                    printf("at %d, = %d\n",stackIterator, incAllStore[stackIterator]);
                }
                for(int i = stackIterator; i != 0; i--){
                    //printf("i = %d\n",i);
                    push(loadedMachine, incAllStore[i-1]);
                }
                break;

            //DECREMENT ALL
            case INST_DEC_ALL:
                int decAllStore[MAX_STACK_SIZE];
                stackIterator = 0;
                for(int i = loadedMachine->currentStackSize; i != 0; i--, stackIterator++){
                    decAllStore[stackIterator] = pop(loadedMachine) - 1;
                    printf("at %d, = %d\n",stackIterator, decAllStore[stackIterator]);
                }
                for(int i = stackIterator; i != 0; i--){
                    //printf("i = %d\n",i);
                    push(loadedMachine, decAllStore[i-1]);
                }
                break;

            //INDEX SWAP
            case INST_INDEX_SWAP:
                IndexSwap(loadedMachine, loadedMachine->instructions[i].operand);
                break;

            //INDEX DUPLICATE
            case INST_INDEX_DUP:
                IndexDuplicate(loadedMachine, loadedMachine->instructions[i].operand);
                break;

            //DEFAULT
            default: //unexpected state
                assert(0);
                break;
        }
    }
    //END-OF-SWITCH-AND-FOR-LOOP--------------------------------------------------------

    printStack(loadedMachine);
    return 0;
}