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

//macros
#define M_INST_PUSH(x) {.type = INST_PUSH, .operand = x}//only one that needs value!
#define M_INST_POP() {.type = INST_POP}
#define M_INST_ADD() {.type = INST_ADD}
#define M_INST_SUB() {.type = INST_SUB}
#define M_INST_MUL() {.type = INST_MUL}
#define M_INST_DIV() {.type = INST_DIV}
#define M_INST_PRINT() {.type = INST_PRINT}

//running the instructions
#define PROGRAM_SIZE (sizeof(program)/sizeof(program[0]))
instruction program[] = {
    M_INST_PUSH(1),
    M_INST_PUSH(2),
    M_INST_ADD(),
    M_INST_PUSH(3),
    M_INST_PUSH(4),
    M_INST_SUB(),
    M_INST_PUSH(5),
    M_INST_PUSH(6),
    M_INST_MUL(),
    M_INST_PUSH(100),
    M_INST_PUSH(5),
    M_INST_DIV(),
    M_INST_PUSH(9),
    M_INST_PUSH(10),
    M_INST_ADD(),
    M_INST_PUSH(11),
    M_INST_PUSH(12),
    M_INST_ADD(),
    M_INST_PUSH(13),
    M_INST_PUSH(14),
    M_INST_ADD(),
    M_INST_PUSH(15),
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
            default://unexpected state
                assert(0);
                break;
        }
    }
    printStack();
    return 0;
}