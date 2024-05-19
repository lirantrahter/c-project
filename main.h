#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

/*Various defines*/
#define LINE_MAX_LENGTH 81
#define WORD 13
#define LABEL_MAX_SIZE 31

/*Enum for various error handling*/
typedef enum Error
{
    success,
    fileOpeningErr,
    fileClosingErr,
    emptyArg,
    memoryAllocErr,
    endOfFile,
    noMcr,
    undefinedCommand,
    undefinedAddressing,
    labelExists,
    labelTooLong,
    wrongDefLabel,
    meaninglessLabel,
    unknownArg,
    missingClosingParentheses,
    missingParentheses,
    missingComma,
    missingArg,
    tooManyArg,
    wrongArg,
    missingLabel,
    consecutiveCommas,
    mcrNameIncorrect,
    removingErr
}error;

/*Enum for opcodes */
typedef enum Opcode 
{
    mov = 0,
    cmp = 1,
    add = 2,
    sub = 3,
    not = 4,
    clr = 5,
    lea = 6,
    inc = 7,
    dec = 8,
    jmp = 9,
    bne = 10,
    red = 11,
    prn = 12,
    jsr = 13,
    rts = 14,
    stop = 15,
    data,
    string,
    entry,
    external,
    none
} opcode;

/*Function header in main.c or funcLib.c only*/
error preAssembler(char* fileName);
error firstRun(char *);
error openFile(FILE **filePointer, char *filePath, char *suffix);
error createFile(FILE **filePointer, char *filePath, char *suffix);
error removeFile(char *filePath, char*suffix);
error closeFile(FILE *filePointer);
error getToken(char **str, char **token, char *delim);
error getOneLine(char **line_out, FILE *fp);
error removeComments(char **str);
char *removeWhiteSpace(char *str);
void freeString(char **ptr);
void freeMulti(void *ptr, ...);
void checkAlloc(void *);
error strIsAlphaDigit(char* str);