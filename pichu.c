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
    {.type = INST_PUSH, .operand = 15},
    {.type = INST_PRINT},
};

#define MAX_STACK_SIZE 1024
int stack[MAX_STACK_SIZE];
int currentStackSize;

void push(int value){   //push value
    stack[currentStackSize] = value;
    currentStackSize++;
}
int pop(){             //pop the top
    return stack[--currentStackSize];
}

int main(){
    for(size_t i=0; i< PROGRAM_SIZE; i++) {
        switch(program[i].type){
            case INST_PUSH:
                push(program[i].operand);
                break;
            case INST_POP:
                break;
            case INST_ADD:
                break;
            case INST_SUB:
                break;
            case INST_MUL:
                break;
            case INST_DIV:
                break;
            case INST_PRINT:
                printf("%d\n", pop());
                break;
            default:
                assert(0);
                break;
        }
    }
    return 0;
}