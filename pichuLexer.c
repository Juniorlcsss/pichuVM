#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "pichuLexer.h"

char *openFile(char *filename, int *length){
    FILE *f = fopen(filename, "r");
    if(f == NULL){
        fprintf(stderr, "could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char* current = {0};

    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);

    current = malloc(sizeof(char) * (*length));
    fread(current, 1, *length, f);
    if(!current){
        fprintf(stderr,"could not read %s\n", filename);
    }

    fclose(f);
    return current;
}   

void printToken(Token token){
    switch(token.type){
    case TYPE_NONE:
        printf("TYPE NONE\n");
        break;
    case TYPE_NOP:
        printf("TYPE NOP\n");
        break;
    case TYPE_PUSH:
        printf("TYPE PUSH\n");
        break;
    case TYPE_POP:
        printf("TYPE POP\n");
        break;
    case TYPE_POP2:
        printf("TYPE POP2\n");
        break;
    case TYPE_PUSH_NULL:
        printf("TYPE PUSH_NULL\n");
        break;
    case TYPE_ADD:
        printf("TYPE ADD\n");
        break;
    case TYPE_SUB:
        printf("TYPE SUBTRACT\n");
        break;
    case TYPE_MUL:
        printf("TYPE MULTIPLY\n");
        break;
    case TYPE_DIV:
        printf("TYPE DIVIDE\n");
        break;
    case TYPE_INC:
        printf("TYPE INCREMENT\n");
        break;
    case TYPE_INC_ALL:
        printf("TYPE INCREMENT ALL\n");
        break;
    case TYPE_DEC:
        printf("TYPE DECREMENT\n");
        break;
    case TYPE_DEC_ALL:
        printf("TYPE DECREMENT ALL\n");
        break;
    case TYPE_PRINT:
        printf("TYPE PRINT\n");
        break;
    case TYPE_DUP:
        printf("TYPE DUPLICATE\n");
        break;
    case TYPE_SWAP:
        printf("TYPE SWAP\n");
        break;
    case TYPE_NEG:
        printf("TYPE NEGATE\n");
        break;
    case TYPE_READ:
        printf("TYPE READ\n");
        break;
    case TYPE_STOP:
        printf("TYPE STOP\n");
        break;
    case TYPE_CLEAR:
        printf("TYPE CLEAR\n");
        break;
    case TYPE_FLIP:
        printf("TYPE FLIP\n");
        break;
    case TYPE_INDEX_SWAP:
        printf("TYPE INDEX_SWAP\n");
        break;
    case TYPE_INDEX_DUP:
        printf("TYPE INDEX_DUP\n");
        break;
    case TYPE_CHANGE_TO:
        printf("TYPE CHANGE_TO\n");
        break;
    case TYPE_MOD:
        printf("TYPE MOD\n");
        break;
    case TYPE_MOD_ALL:
        printf("TYPE MOD_ALL\n");
        break;
    case TYPE_COMPE:
        printf("TYPE COMPARE EQUALS\n");
        break;
    case TYPE_COMPNE:
        printf("TYPE COMPARE NOT EQUALS\n");
        break;
    case TYPE_COMPG:
        printf("TYPE COMPARE GREATER THAN\n");
        break;
    case TYPE_COMPGE:
        printf("TYPE COMPARE GREATER THAN OR EQUAL TO\n");
        break;
    case TYPE_COMPL:
        printf("TYPE COMPARE LESS THAN");
        break;
    case TYPE_COMPLE:
        printf("TYPE COMPARE LESS THAN OR EQUAL TO\n");
        break;
    case TYPE_COMPE_NULL:
        printf("TYPE COMPARE EQUAL TO NULL");
        break;
    case TYPE_COMPNE_NULL:
        printf("TYPE COMPARE NOT EQUAL TO NULL");
        break;
    case TYPE_CONST_M1:
        printf("TYPE CONSTANT -1\n");
        break;
    case TYPE_CONST_0:
        printf("TYPE CONSTANT 0\n");
        break;
    case TYPE_CONST_1:
        printf("TYPE CONSTANT 1\n");
        break;
    case TYPE_CONST_2:
        printf("TYPE CONSTANT 2\n");
        break;
    case TYPE_CONST_3:
        printf("TYPE CONSTANT 3\n");
        break;
    case TYPE_CONST_4:
        printf("TYPE CONSTANT 4\n");
        break;
    case TYPE_CONST_5:
        printf("TYPE CONSTANT 5\n");
        break;
    case TYPE_GOTO:
        printf("TYPE GOTO\n");
        break;
    case TYPE_GOTO_Z:
        printf("TYPE GOTO IF EQUAL Z\n");
        break;
    case TYPE_GOTO_NZ:
        printf("TYPE GOTO IF NOT EQUAL Z\n");
        break;
    case TYPE_GOTO_NULL:
        printf("TYPE GOTO NULL\n");
        break;
    case TYPE_GOTO_C:
        printf("TYPE GOTO IF EQUAL CONDITION\n");
        break;
    case TYPE_GOTO_NC:
        printf("TYPE GOTO IF NOT EQUAL CONDITION\n");
        break;
    case TYPE_CONDITION:
        printf("TYPE CONDITION\n");
        break;
    }
    printf("text:%s, line:%d, char:%d\n", token.text, token.line, token.character);
}

Token initToken(TokenType type, char *text, int line, int character){
    Token token = {.type = type, .text = text, .line = line, .character = character};
    return token;
}

TokenType check(char *name){
    if(strcmp(name, "nop") == 0){
        return TYPE_NOP;
    }
    else if(strcmp(name, "push")== 0){
        return TYPE_PUSH;
    }
    else if(strcmp(name, "pop")== 0){
        return TYPE_POP;
    }
    else if(strcmp(name, "pop2")== 0){
        return TYPE_POP2;
    }
    else if(strcmp(name, "push_null")== 0){
        return TYPE_PUSH_NULL;
    }
    else if(strcmp(name, "add")== 0){
        return TYPE_ADD;
    }
    else if(strcmp(name, "sub")== 0){
        return TYPE_SUB;
    }
    else if(strcmp(name, "mul")== 0){
        return TYPE_MUL;
    }
    else if(strcmp(name, "div")== 0){
        return TYPE_DIV;
    }
    else if(strcmp(name, "inc")== 0){
        return TYPE_INC;
    }
    else if(strcmp(name, "inc_all")== 0){
        return TYPE_INC_ALL;
    }
    else if(strcmp(name, "dec")== 0){
        return TYPE_DEC;
    }
    else if(strcmp(name, "dec_all")== 0){
        return TYPE_DEC_ALL;
    }
    else if(strcmp(name, "print")== 0){
        return TYPE_PRINT;
    }
    else if(strcmp(name, "dup")== 0){
        return TYPE_DUP;
    }
    else if(strcmp(name, "swap")== 0){
        return TYPE_SWAP;
    }
    else if(strcmp(name, "neg")== 0){
        return TYPE_NEG;
    }
    else if(strcmp(name, "read")== 0){
        return TYPE_READ;
    }
    else if(strcmp(name, "stop")== 0){
        return TYPE_STOP;
    }
    else if(strcmp(name, "clear")== 0){
        return TYPE_CLEAR;
    }
    else if(strcmp(name, "flip")== 0){
        return TYPE_FLIP;
    }
    else if(strcmp(name, "index_swap")== 0){
        return TYPE_INDEX_SWAP;
    }
    else if(strcmp(name, "index_dup")== 0){
        return TYPE_INDEX_DUP;
    }
    else if(strcmp(name, "change_to")== 0){
        return TYPE_CHANGE_TO;
    }
    else if(strcmp(name, "mod")== 0){
        return TYPE_MOD;
    }
    else if(strcmp(name, "mod_all")== 0){
        return TYPE_MOD_ALL;
    }
    else if(strcmp(name, "compe")== 0){
        return TYPE_COMPE;
    }
    else if(strcmp(name, "compne")== 0){
        return TYPE_COMPNE;
    }
    else if(strcmp(name, "compg")== 0){
        return TYPE_COMPG;;
    }
    else if(strcmp(name, "compge")== 0){
        return TYPE_COMPGE;
    }
    else if(strcmp(name, "compl")== 0){
        return TYPE_COMPL;
    }
    else if(strcmp(name, "comple")== 0){
        return TYPE_COMPLE;
    }
    else if(strcmp(name, "compe_null")== 0){
        return TYPE_COMPE_NULL;
    }
    else if(strcmp(name, "compne_null")== 0){
        return TYPE_COMPNE_NULL;
    }
    else if(strcmp(name, "const_-1")== 0){
        return TYPE_CONST_M1;
    }
    else if(strcmp(name, "const_0")== 0){
        return TYPE_CONST_0;
    }
    else if(strcmp(name, "const_1")== 0){
        return TYPE_CONST_1;
    }
    else if(strcmp(name, "const_2")== 0){
        return TYPE_CONST_2;
    }
    else if(strcmp(name, "const_3")== 0){
        return TYPE_CONST_3;
    }
    else if(strcmp(name, "const_4")== 0){
        return TYPE_CONST_4;
    }
    else if(strcmp(name, "const_5")== 0){
        return TYPE_CONST_5;
    }
    else if(strcmp(name, "goto")== 0){
        return TYPE_GOTO;
    }
    else if(strcmp(name, "goto _z")== 0){
        return TYPE_GOTO_Z;
    }
    else if(strcmp(name, "goto_nz")== 0){
        return TYPE_GOTO_NZ;
    }
    else if (strcmp(name, "goto_null")== 0){
        return TYPE_GOTO_NULL;
    }
    else if(strcmp(name, "goto_c")== 0){
        return TYPE_GOTO_C;
    }
    else if(strcmp(name, "goto_nc")== 0){
        return TYPE_GOTO_NC;
    }
    else if(strcmp(name, "condition")== 0){
        return TYPE_CONDITION;
    }  
    return TYPE_NONE;  
}

Token keywordGenerate(char *current, int *currentIndex, int line, int character){
    char *keyWord = malloc(sizeof(char) * 16);
    int keyWordLength = 0;
    while(isalpha(current[*currentIndex])){
        keyWord[keyWordLength] = current[*currentIndex];
        *currentIndex += 1;
        keyWordLength++;
    }
    keyWord[keyWordLength] = '\0';
    TokenType type = check(keyWord);
    assert(type != TYPE_NONE && "identifier not implemented yet\n");
    Token token = initToken(type, keyWord, line, character);
    return token;
}

int lexer(){
    int length;
    char *current = openFile("pichu.asm", &length);
    int currentIndex = 0;

    int line = 1;
    int character = 1;

    while(currentIndex < length){
        if(current[currentIndex] == '\n'){
            line++;
            character = 0;
        }

        if(isalpha(current[currentIndex])){
            Token token = keywordGenerate(current, &currentIndex, line, character);
            currentIndex--;
            printToken(token);
        }
        else if(isdigit(current[currentIndex])){
            printf("number literal");
            currentIndex++;
        }
        character++;
    }
    return 0;
}