#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

#define PROGRAM_SIZE (sizeof(program)/sizeof(program[0]))
instruction program[] = {
    {.type = INST_PUSH, .operand = 1},
    {.type = INST_PUSH, .operand = 2},
    {.type = INST_PUSH, .operand = 3},
    {.type = INST_PUSH, .operand = 4},
    //{.type = INST_DIV},
    //{.type = INST_PRINT},
};

#define MAX_STACK_SIZE 1024
int stack[MAX_STACK_SIZE];
int currentStackSize;

void push(int value){   //push value
    if(currentStackSize >= MAX_STACK_SIZE){
        fprintf(stderr, "stack overflow\n");
        exit(EXIT_FAILURE);
    }
    stack[currentStackSize] = value;
    currentStackSize++;
}
int pop(){             //pop the top
    if(currentStackSize <= 0) {
        fprintf(stderr, "stack underflow\n");
        exit(EXIT_FAILURE);
    }
    return stack[--currentStackSize];
}

void printStack(){
    for(int i= currentStackSize - 1; i>=0; i--){
        printf("%d\n", stack[i]);
    }
    printf("\n");
}

int main(){
    for(size_t i=0; i< PROGRAM_SIZE; i++) {
        switch(program[i].type){
            case INST_PUSH:
                push(program[i].operand);
                break;
            case INST_POP:
                pop();
                break;
            case INST_ADD:
                push(pop() + pop());
                break;
            case INST_SUB:
                int first = pop();
                int second = pop();
                push(second - first);
                break;
            case INST_MUL:
                push(pop() * pop());
                break;
            case INST_DIV:
                first = pop();
                second = pop();
                push(second / first);
                break;
            case INST_PRINT:
                printf("%d\n", pop());
                break;
            default:
                assert(0);
                break;
        }
    }
    printStack();
    return 0;
}