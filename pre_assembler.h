#include "main.h"
/*Macro struct that gets the name of the macro and the definition of the macro*/
typedef struct Mcr 
{
    char *name;
    char *code;
} Mcr;

/*Node of macro*/
typedef struct NodeMcr 
{
    struct Mcr data;
    struct NodeMcr *next;
} NodeMcr;

/*linked list of macros*/
typedef struct ListMcr 
{
    int count;
    NodeMcr *head;
} ListMcr;

char *concatenateStrings(char* str1, char* str2);
void addToList(NodeMcr * newElement, ListMcr* list);
struct NodeMcr * createNode(char * name, char* code);
error is_mcr_def( char **lineOut);
error is_mcrEnd(char *line);
error is_name_of_mcr(char* line,ListMcr * mcrList);