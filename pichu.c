#include "pichu.h"

//running the program
Instruction program[] = {
    M_INST_PUSH(1),
    M_INST_PUSH(4),
    M_INST_PUSH(6),
    M_INST_PUSH(8),
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
    printf("Top Of Stack:\n");
    for(int i = machine->currentStackSize - 1; i>=0; i--){
        printf("%d\n", machine->stack[i]);
    }
    printf("Bottom Of Stack:\n");
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

void runPichu(Machine *machine){
    int first, second; //first and second operands, used for division and subtraction
    int gotoStore;     //Used for GOTO_Z and GOTO_NZ
    int stackStore[MAX_STACK_SIZE]; //used to store previous values within the stack to restore during GOTO operation failures

    //run program & instructions--------------------------------------------------------
    for(size_t i=0; i < machine->programSize; i++) {
        switch((machine->instructions)[i].operator){
            //PUSH
            case INST_PUSH:
                push(machine, machine->instructions[i].operand);
                break;

            //POP
            case INST_POP:
                pop(machine);
                break;

            //ADD
            case INST_ADD:
                push(machine, pop(machine) + pop(machine));
                break;

            //SUBTRACT
            case INST_SUB:
                first = pop(machine);
                second = pop(machine);
                push(machine, first - second);
                break;

            //MULTIPLY
            case INST_MUL:
                push(machine ,pop(machine) * pop(machine));
                break;

            //DIVIDE
            case INST_DIV:
                first = pop(machine);
                second = pop(machine);
                if(second == 0){
                    fprintf(stderr, "cant divide by 0\n");
                    exit(EXIT_FAILURE);
                }
                push(machine ,first / second);
                break;

            //PRINT
            case INST_PRINT:
                printf("%d\n", pop(machine));
                break;

            //DUPLICATE
            case INST_DUP:
                first = pop(machine);
                push(machine ,first);
                push(machine ,first);
                break;

            //SWAP
            case INST_SWAP:
                first = pop(machine);
                second = pop(machine);
                push(machine, first);
                push(machine, second);
                break;

            //COMPARE EQUAL
            case INST_COMPE:
                first = pop(machine);
                second = pop(machine);
                if(first == second){
                    push(machine, 1);
                }
                else{
                    push(machine, 0);
                }
                break;

            //COMPARE NOT EQUAL
            case INST_COMPNE:
                first = pop(machine);
                second = pop(machine);
                if(first == second){
                    push(machine, 0);
                }
                else{
                    push(machine, 1);
                }
                break;  

            //COMPARE GREATER THAN
            case INST_COMPG:
                first = pop(machine);
                second = pop(machine);
                if(first > second){
                    push(machine, 1);
                }
                else{
                    push(machine, 0);
                }
                break;  

            //COMPARE GREATER THAN OR EQUAL TO
            case INST_COMPGE:
                first = pop(machine);
                second = pop(machine);
                if(first >= second){
                    push(machine, 1);
                }
                else{
                    push(machine, 0);
                }
                break;

            //COMPARE LESS THAN
            case INST_COMPL: 
                first = pop(machine);
                second = pop(machine);
                if(first < second){
                    push(machine, 1);
                }
                else{
                    push(machine, 0);
                }
                break;

            //COMPARE LESS THAN OR EQUAL TO
            case INST_COMPLE:  
                first = pop(machine);
                second = pop(machine);
                if(first <= second){
                    push(machine, 1);
                }
                else{
                    push(machine, 0);
                }
                break; 
            
            //NO OPERATION
            case INST_NOP:
                //null
                break;

            //POP2
            case INST_POP2:
                pop(machine);
                pop(machine);
                break;

            //NEGATE
            case INST_NEG:
                first = pop(machine);
                push(machine, -first);
                break;

            //READ
            case INST_READ:
                first = pop(machine);
                printf("top of stack is %d\n", first);
                push(machine, first);
                break;

            //STOP
            case INST_STOP:
                i = machine->programSize;
                break;

            //CLEAR
            case INST_CLEAR:
                while(machine->currentStackSize > 0){
                    pop(machine);
                }
                break;
            
            //CONSTANT -1
            case INST_CONST_M1:
                push(machine, -1);
                break;

            //CONSTANT 0
            case INST_CONST_0:
                push(machine, 0);
                break;

            //CONSTANT 1
            case INST_CONST_1:
                push(machine, 1);
                break;

            //CONSTANT 2
            case INST_CONST_2:
                push(machine, 2);
                break;

            //CONSTANT 3
            case INST_CONST_3:
                push(machine, 3);
                break;

            //CONSTANT 4
            case INST_CONST_4:
                push(machine, 4);
                break;

            //CONSTANT 5 
            case INST_CONST_5:
                push(machine, 5);
                break;


/*//!-----------------------------------------------------------------
            //PUSH NULL REFERENCE ONTO STACK
            case INST_PUSH_NULL:
                push(machine, NULL);
                break;

            //COMPARE NULL
            case INST_COMPE_NULL:
                first = pop(machine);
                if(first == NULL){
                    push(machine, 1);
                }
                else{
                    push(machine, 0);
                }
                break;

            //COMPARE NOT NULL
            case INST_COMPNE_NULL:
                first = pop(machine);
                if(first == NULL){
                    push(machine, 0);
                }
                else{
                    push(machine, 1);
                }
                break;
*///!-----------------------------------------------------------------

            //GOTO
            //NOTE: I have commented out the printf checks for this instruction
            case INST_GOTO:
                first = machine->instructions[i].operand;
                second = pop(machine);
                int popIterator =0;
                stackStore[0] = second; //stores initial popped value
                //printf("stackStore[0] = %d\n", stackStore[0]);
                for(int gotoIndex = 0; gotoIndex < machine->currentStackSize; popIterator++){
                    //printf("first = %d, second = %d\n", first, second);
                    if(first != second){
                        //printf("First != second loop\n");
                        second = pop(machine);
                        stackStore[popIterator+1] = second;
                        //printf("%d\n", stackStore[popIterator]);
                    }
                    else if(first == second){
                        //printf("First == second loop\n");
                        push(machine, second);
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
                        push(machine, stackStore[forLoopIterator]);
                    }
                }
                //printf("exited for loop\n");
                break;

            //GOTO IF ZERO
            case INST_GOTO_Z:
                gotoStore = pop(machine);
                printf("Popped first value and stored it as %d\n", gotoStore);
                if(gotoStore == 0){
                    //COPY GOTO
                    first = machine->instructions[i].operand;
                    second = pop(machine);
                    int popIterator =0;
                    stackStore[0] = second; //stores initial popped value
                    //printf("stackStore[0] = %d\n", stackStore[0]);
                    for(int gotoIndex = 0; gotoIndex < machine->currentStackSize; popIterator++){
                        //printf("first = %d, second = %d\n", first, second);
                        if(first != second){
                            //printf("First != second loop\n");
                            second = pop(machine);
                            stackStore[popIterator+1] = second;
                            //printf("%d\n", stackStore[popIterator]);
                        }
                        else if(first == second){
                            //printf("First == second loop\n");
                            push(machine, second);
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
                            push(machine, stackStore[forLoopIterator]);
                        }
                        push(machine, gotoStore);
                    }
                    //printf("exited for loop\n");
                }
                else{
                    //PUSH POPPED VALUE BACK IN
                    push(machine, gotoStore);
                }
                break;

            //GOTO IF NOT ZERO
            case INST_GOTO_NZ:
                gotoStore = pop(machine);
                printf("Popped first value and stored it as %d\n", gotoStore);
                if(gotoStore != 0){
                    //COPY GOTO
                    first = machine->instructions[i].operand;
                    second = pop(machine);
                    int popIterator = 0;
                    stackStore[0] = second; //stores initial popped value
                    //printf("stackStore[0] = %d\n", stackStore[0]);
                    for(int gotoIndex = 0; gotoIndex < machine->currentStackSize; popIterator++){
                        //printf("first = %d, second = %d\n", first, second);
                        if(first != second){
                            //printf("First != second loop\n");
                            second = pop(machine);
                            stackStore[popIterator+1] = second;
                            //printf("%d\n", stackStore[popIterator]);
                        }
                        else if(first == second){
                            //printf("First == second loop\n");
                            push(machine, second);
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
                            push(machine, stackStore[forLoopIterator]);
                        }
                        push(machine, gotoStore);
                    }
                    //printf("exited for loop\n");
                }
                else{
                    //PUSH POPPED VALUE BACK IN
                    push(machine, gotoStore);
                }
                break;

            //REPLACE WITH
            case INST_CHANGE_TO:
                int changeTo = machine->instructions[i].operand;
                printf("Change to is %d\n", changeTo);
                break;

            //MODIFY
            case INST_MOD:
                int mod = machine->instructions[i].operand;
                printf("Modifying %d to %d\n", mod, changeTo);

                int modifyStore[MAX_STACK_SIZE];
                modifyStore[0] = pop(machine);
                first = modifyStore[0];
                printf("First outside pop = %d\n", first);

                printf("ModifyStore %d\n", modifyStore[0]);

                int stackIterator = 1;
                for(int modLoop = machine->currentStackSize; modLoop != 0; modLoop--, stackIterator++){
                    first = pop(machine);
                    printf("stackIterator = %d\n", stackIterator);
                    modifyStore[stackIterator] = first;
                    printf("%d\n", first);
                    printf("ModifyStore %d\n", modifyStore[stackIterator]);

                    if(first == mod){
                        printf("in first == mod, %d\n", first);
                        push(machine, changeTo);
                        //push the stored stack back
                        for(int i = stackIterator; i != 0; i--){
                            printf("pushing %d at %d\n", modifyStore[i-1], i-1);
                            push(machine, modifyStore[i-1]);
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
                        push(machine, modifyStore[forLoopIterator-1]);
                    }                
                }
                printf("Exited loop\n");
                break; 

            //MODIFY ALL
            case INST_MOD_ALL:
                int modAll = machine->instructions[i].operand;
                printf("Modifying all %d to %d\n", modAll, changeTo);
                int modifyAllStore[MAX_STACK_SIZE];

                first = pop(machine);
                modifyAllStore[0] = first;
                printf("First outside pop = %d/%d\n", first, modifyAllStore[0]);

                stackIterator = 1;
                for(int modLoop = machine->currentStackSize; modLoop != 0; modLoop--, stackIterator++){
                    first = pop(machine);
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
                    push(machine, modifyAllStore[i-1]);
                }
                printf("Exited loop\n");
                break; 

            //FLIPS STACK
            case INST_FLIP:
                int flipStore[MAX_STACK_SIZE];
                stackIterator = 0;

                //puts stack into flipStore
                for(int i = machine->currentStackSize; i != 0; i--, stackIterator++){
                    flipStore[stackIterator] = pop(machine);
                    printf("%d\n", flipStore[stackIterator]);
                }

                //puts flipStore back into stack
                for(int i = 1; i != stackIterator+1; i++){
                    printf("pushing %d at %d\n", flipStore[i-1], i-1);
                    push(machine, flipStore[i-1]);
                }
                break;

            //GOTO CONDITION
            case INST_CONDITION:
                int condition = machine->instructions[i].operand;
                printf("GOTO condition is set to %d\n", condition);
                break;

            //GOTO IF CONDITION = TRUE
            case INST_GOTO_C:
                gotoStore = pop(machine);
                printf("Popped first value and stored it as %d\n", gotoStore);
                if(gotoStore == condition){
                    //COPY GOTO
                    first = machine->instructions[i].operand;
                    second = pop(machine);
                    int popIterator =0;
                    stackStore[0] = second; //stores initial popped value
                    //printf("stackStore[0] = %d\n", stackStore[0]);
                    for(int gotoIndex = 0; gotoIndex < machine->currentStackSize; popIterator++){
                        //printf("first = %d, second = %d\n", first, second);
                        if(first != second){
                            //printf("First != second loop\n");
                            second = pop(machine);
                            stackStore[popIterator+1] = second;
                            //printf("%d\n", stackStore[popIterator]);
                        }
                        else if(first == second){
                            //printf("First == second loop\n");
                            push(machine, second);
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
                            push(machine, stackStore[forLoopIterator]);
                        }
                        push(machine, gotoStore);
                    }
                    //printf("exited for loop\n");
                }
                else{
                    //PUSH POPPED VALUE BACK IN
                    push(machine, gotoStore);
                }
                break;

            //GOTO IF CONDITION = FALSE
            case INST_GOTO_NC:
                gotoStore = pop(machine);
                printf("Popped first value and stored it as %d\n", gotoStore);
                if(gotoStore != condition){
                    //COPY GOTO
                    first = machine->instructions[i].operand;
                    second = pop(machine);
                    int popIterator = 0;
                    stackStore[0] = second; //stores initial popped value
                    //printf("stackStore[0] = %d\n", stackStore[0]);
                    for(int gotoIndex = 0; gotoIndex < machine->currentStackSize; popIterator++){
                        //printf("first = %d, second = %d\n", first, second);
                        if(first != second){
                            //printf("First != second loop\n");
                            second = pop(machine);
                            stackStore[popIterator+1] = second;
                            //printf("%d\n", stackStore[popIterator]);
                        }
                        else if(first == second){
                            //printf("First == second loop\n");
                            push(machine, second);
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
                            push(machine, stackStore[forLoopIterator]);
                        }
                        push(machine, gotoStore);
                    }
                    //printf("exited for loop\n");
                }
                else{
                    //PUSH POPPED VALUE BACK IN
                    push(machine, gotoStore);
                }                
                break;

            //INCREMENT TOP
            case INST_INC:
                first = pop(machine);
                first++;
                push(machine, first);
                break;

            //DECREMENT TOP
            case INST_DEC:
                first = pop(machine);
                first--;
                push(machine, first);
                break;

            //INCREMENT ALL
            case INST_INC_ALL:
                int incAllStore[MAX_STACK_SIZE];
                stackIterator = 0;
                for(int i = machine->currentStackSize; i != 0; i--, stackIterator++){
                    incAllStore[stackIterator] = pop(machine) + 1;
                    printf("at %d, = %d\n",stackIterator, incAllStore[stackIterator]);
                }
                for(int i = stackIterator; i != 0; i--){
                    //printf("i = %d\n",i);
                    push(machine, incAllStore[i-1]);
                }
                break;

            //DECREMENT ALL
            case INST_DEC_ALL:
                int decAllStore[MAX_STACK_SIZE];
                stackIterator = 0;
                for(int i = machine->currentStackSize; i != 0; i--, stackIterator++){
                    decAllStore[stackIterator] = pop(machine) - 1;
                    printf("at %d, = %d\n",stackIterator, decAllStore[stackIterator]);
                }
                for(int i = stackIterator; i != 0; i--){
                    //printf("i = %d\n",i);
                    push(machine, decAllStore[i-1]);
                }
                break;

            //INDEX SWAP
            case INST_INDEX_SWAP:
                IndexSwap(machine, machine->instructions[i].operand);
                break;

            //INDEX DUPLICATE
            case INST_INDEX_DUP:
                IndexDuplicate(machine, machine->instructions[i].operand);
                break;

            //DEFAULT
            default: //unexpected state
                assert(0);
                break;
        }
    }
    //END-OF-SWITCH-AND-FOR-LOOP--------------------------------------------------------
}

int main(){
    /*
    int first, second; //first and second operands, used for division and subtraction
    int gotoStore;     //Used for GOTO_Z and GOTO_NZ
    int stackStore[MAX_STACK_SIZE]; //used to store previous values within the stack to restore during GOTO operation failures
    */

    Machine *loadedMachine = malloc(sizeof(Machine) * MAX_STACK_SIZE);
    loadedMachine->instructions = program;
    writeProgramToFile(loadedMachine, "program.bin");
    loadedMachine = readFromFile(loadedMachine, "program.bin");

    runPichu(loadedMachine);

    printStack(loadedMachine);
    return 0;
}