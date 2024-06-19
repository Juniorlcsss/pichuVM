#ifndef LEXER_H
#define LEXER_H

typedef enum{
    TYPE_NONE = -1,
    TYPE_NOP = 0,
    TYPE_PUSH,
    TYPE_POP,
    TYPE_POP2,
    TYPE_PUSH_NULL,
    TYPE_ADD,
    TYPE_SUB,
    TYPE_MUL,
    TYPE_DIV,
    TYPE_INC,
    TYPE_INC_ALL,
    TYPE_DEC,
    TYPE_DEC_ALL,
    TYPE_PRINT,
    TYPE_DUP,
    TYPE_SWAP,
    TYPE_NEG,
    TYPE_READ,
    TYPE_STOP,
    TYPE_CLEAR,
    TYPE_FLIP,
    TYPE_INDEX_SWAP,
    TYPE_INDEX_DUP,
    TYPE_CHANGE_TO,
    TYPE_MOD,
    TYPE_MOD_ALL,
    TYPE_COMPE,
    TYPE_COMPNE,
    TYPE_COMPG,
    TYPE_COMPGE,
    TYPE_COMPL,
    TYPE_COMPLE,
    TYPE_COMPE_NULL,
    TYPE_COMPNE_NULL,
    TYPE_CONST_M1,
    TYPE_CONST_0,
    TYPE_CONST_1,
    TYPE_CONST_2,
    TYPE_CONST_3,
    TYPE_CONST_4,
    TYPE_CONST_5,
    TYPE_GOTO,
    TYPE_GOTO_Z,
    TYPE_GOTO_NZ,
    TYPE_GOTO_NULL,
    TYPE_GOTO_C,
    TYPE_GOTO_NC,
    TYPE_CONDITION,
} TokenType;

typedef struct{
    TokenType type;
    char *text;
    int line;
    int character;
} Token;

int lexer();

#endif