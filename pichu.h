#ifndef PICHU_H
#define PICHU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_STACK_SIZE 1024


typedef enum{
    //https://en.wikipedia.org/wiki/List_of_Java_bytecode_instructions
    //https://en.wikipedia.org/wiki/X86_instruction_listings

    //Push/Pop
    INST_PUSH,          //push onto stack  
    INST_POP,           //pop off stack
    INST_POP2,          //pops the top two values on the stack
    INST_PUSH_NULL,     //Pushes a null reference onto the stack //!Unfinished

    //Arithmetic
    INST_ADD,           //add first 2 popped values from stack
    INST_SUB,           //subtract first 2 popped values from stack
    INST_MUL,           //multiply first 2 popped values from stack
    INST_DIV,           //divide first 2 popped values from stack
    INST_INC,           //Increments the top of stack by 1
    INST_INC_ALL,       //Increments all values of stack by 1
    INST_DEC,           //Decrements the top of stack by 1 
    INST_DEC_ALL,       //Decrements all of the values on the stack by 1

    //MISC
    INST_PRINT,         //print popped value
    INST_DUP,           //pops top, and then pushes back into stack twice
    INST_SWAP,          //swaps front 2 values around
    INST_NEG,           //Negates operand
    INST_READ,          //Prints value top of stack, doesn't pop it off the stack
    INST_STOP,          //Ignores any further instructions
    INST_CLEAR,         //Removes all prior instructions from stack
    INST_NOP,           //No operation
    INST_FLIP,          //Inverts the stack
    INST_INDEX_SWAP,    //Swaps operands around depending on the position of the swap (e.g. swap(0) would swap the top of the stack to the bottom of the stack)
    INST_INDEX_DUP,     //Swaps operands like swap, but the duplicate stays where it initially was

    //Modify
    INST_CHANGE_TO,     //Stores what you want to modify the value you want to change to
    INST_MOD,           //Modifies the nearest specified value in stack
    INST_MOD_ALL,       //Modifies all of the specified value in stack
    //(e.g. if you want to replace the nearest value 5 with the value 6, you would put M_INST_CHANGE_TO(6) and M_INST_MOD(5)

    //Comparison
    INST_COMPE,         //Compares, if they are equal, pop both and push in 1, else, push 0
    INST_COMPNE,        //Inverse of COMPE
    INST_COMPG,         //Compares if first value is greater than second value
    INST_COMPGE,        //compares if first value is greater than or equal to the second value
    INST_COMPL,         //compares if first value is less than second value
    INST_COMPLE,        //compares if first value is less than or equal to the second value
    INST_COMPE_NULL,    //Checks if top == null, if yes 1, if not 0 //!Unfinished
    INST_COMPNE_NULL,   //inverse of COMPNE_NULL //!Unfinished

    //Constant instructions
    INST_CONST_M1,      //load int value -1 onto stack
    INST_CONST_0,       //load int value 0 onto stack
    INST_CONST_1,       //load int value 1 onto stack
    INST_CONST_2,       //load int value 2 onto stack
    INST_CONST_3,       //load int value 3 onto stack
    INST_CONST_4,       //load int value 4 onto stack
    INST_CONST_5,       //load int value 5 onto stack

    //Jumps
    INST_GOTO,          //Goes to the nearest selected value
    INST_GOTO_Z,        //If top of stack == 0, go to operand 
    INST_GOTO_NZ,       //If top of stack != 0, go to operand
    INST_GOTO_C,        //If top of stack = given condition, go to operand
    INST_GOTO_NC,       //If top of stack != given condition, go to operand
    INST_CONDITION,     //Input the condition to be used for goto
} InstructionSet;


//Instruction
typedef struct{
    InstructionSet operator;    //e.g. push
    int operand;                //e.g. 2
} Instruction;


//Machine
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
#define M_INST_CLEAR() {.operator = INST_CLEAR}
#define M_INST_PUSH_NULL() {.operator = INST_PUSH_NULL}
#define M_INST_COMPE_NULL() {.operator = INST_COMPE_NULL}
#define M_INST_COMPNE_NULL() {.operator = INST_COMPNE_NULL}
#define M_INST_GOTO_Z(x) {.operator = INST_GOTO_Z, .operand = x}
#define M_INST_GOTO_NZ(x) {.operator = INST_GOTO_NZ, .operand = x}
#define M_CONST_M1() {.operator = INST_CONST_M1}
#define M_CONST_0() {.operator = INST_CONST_0}
#define M_CONST_1() {.operator = INST_CONST_1}
#define M_CONST_2() {.operator = INST_CONST_2}
#define M_CONST_3() {.operator = INST_CONST_3}
#define M_CONST_4() {.operator = INST_CONST_4}
#define M_CONST_5() {.operator = INST_CONST_5}
#define M_INST_MOD(x) {.operator = INST_MOD, .operand = x}
#define M_INST_MOD_ALL(x) {.operator = INST_MOD_ALL, .operand = x}
#define M_INST_CHANGE_TO(x) {.operator = INST_CHANGE_TO, .operand = x}
#define M_INST_FLIP() {.operator = INST_FLIP}
#define M_INST_CONDITION(x) {.operator = INST_CONDITION, .operand = x}
#define M_INST_GOTO_C(x) {.operator = INST_GOTO_C, .operand = x}
#define M_INST_GOTO_NC(x) {.operator = INST_GOTO_NC, .operand = x}
#define M_INST_INC() {.operator = INST_INC}
#define M_INST_DEC() {.operator = INST_DEC}
#define M_INST_INC_ALL() {.operator = INST_INC_ALL}
#define M_INST_DEC_ALL() {.operator = INST_DEC_ALL}
#define M_INST_INDEX_DUP(x) {.operator = INST_INDEX_DUP, .operand = x}
#define M_INST_INDEX_SWAP(x) {.operator = INST_INDEX_SWAP, .operand = x}
//----------------------------------------------------------------


//functions
void push(Machine *machine, int operand);
int pop(Machine *machine);
void IndexSwap(Machine *machine, int index);
void IndexDuplicate(Machine *machine, int index);
void printStack(Machine *machine);
void writeProgramToFile(Machine *machine, char *filename);
Machine *readFromFile(Machine *machine, char *filename);
void runPichu(Machine *machine);
int main();


#endif