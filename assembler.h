#include "main.h"
#define WORD 13/*need to know*/

#define REGI_1_START 0/**/
#define REGI_2_START 5/**/
#define SRC_AM_START 0/**/
#define OPCODE_START 3/**/
#define DEST_AM_START 7/**/
#define ARE_START 10/**/

#define REGI_NAME_SIZE 3/*need to check*/
#define ARE_SIZE 2/**/
#define OPCODE_SIZE 4/**/
#define AM_SIZE 3/**/
#define REGI_SIZE 5/**/
#define ADDRESS_SIZE 10/**/
#define IMMEDIATE_SIZE 10/**/
#define DATA_SIZE 12/**/

typedef enum AreType 
{/** It should be like this ARE  working */
    ARE_Immediate = 0,
    ARE_external = 1,
    ARE_relocate = 2
} AreType;

typedef enum AddressMethod 
{/*****/
    immediate = 1,
    direct = 3,
    directRegister = 5
} addressMethod;

typedef struct Code 
{
    char name[LABEL_MAX_SIZE];
    opcode type;
    int place;
    char InstructionCode[WORD];
} Code;

typedef struct Node 
{
    Code data;
    struct Node *next;
} Node;

typedef struct List 
{
    int count;
    Node *head;
} list;

void binaryTo64(char binary[13], char *base64);
error strIsAlphaDigit(char *str);
void reverseSTR(char str[], int);
error clearWhiteSpace(char **line);
error idArg(char **arg, addressMethod *AMarg);
error searchNode(list *list, char *name, Node **nodeOut);
struct Node *createNodeFirstRun(char *name, opcode opcode, int place, char *instructionCode);
error secondRun(list *dataList, list *labelList, list *instructionList, char *fileName, error errFlag,int IC,int DC);