#include "pre_assembler.h"

/*Function checks if definition of mac is a command, if so returns error "mcrNameIncorrect",
 * if empty returns emptyArg, else returns success, function received char* */
error isCommand(char *command) 
{
    if (!command)
        return emptyArg;
    if (!strcmp(command, "mov"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "cmp"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "add"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "sub"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "not"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "clr"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "lea"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "inc"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "dec"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "jmp"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "bne"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "red"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "prn"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "jsr"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "rts"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "stop"))
        return mcrNameIncorrect;
    else if (!strcmp(command, ".data"))
        return mcrNameIncorrect;
    else if (!strcmp(command, ".string"))
        return mcrNameIncorrect;
    else if (!strcmp(command, ".entry"))
        return mcrNameIncorrect;
    else if (!strcmp(command, ".extern"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "@r0"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "@r1"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "@r2"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "@r3"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "@r4"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "@r5"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "@r6"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "@r7"))
        return mcrNameIncorrect;
    else
        return success;
}

/* function frees list and nodes, the function recieves ListMcr * and NodeMcr** of the head of list*/
static error clearList (ListMcr *lst, NodeMcr **node) 
{
    if (node && *node) 
    {
        if ((*node)->next)
            clearList(lst, &((*node)->next));
        free((*node)->data.name);
        free((*node)->data.code);
        free(*node);
        *node = NULL;
        lst->count--;
    } else if (lst->head)
        clearList(lst, &(lst->head));
    return success;
}

/*add one node to head of the list
 * received NodeMcr* of the node to add and ListMcr* */
void addToList(NodeMcr * newElement, ListMcr * list)
{
    if(list->head == NULL)/*if the list is empty*/
    {
        list->head = newElement;
        list->count++;
        return;
    }
    newElement->next = list->head;
    list->head = newElement;
    list->count++;
}

/* Function to create a new node
 * received char* of name and char* of code
 * the caller must free at the end*/
struct NodeMcr * createNode(char * name, char* code) 
{
    /* Allocate memory for the new node*/
    struct NodeMcr * newNode = (struct NodeMcr*) malloc(sizeof(struct NodeMcr));
    if (newNode == NULL) 
    {
        printf("Memory allocation failed");
        exit(1);
    }
    /* Allocate memory for the string data and copy the input string to it*/
    newNode->data.name = (char*) malloc((strlen(name) + 1) * sizeof(char));
    if (!newNode->data.name) 
    {
        printf("Memory allocation failed");
        exit(1);
    }
    newNode->data.code = (char*) malloc((strlen(code) + 1) * sizeof(char));
    if (!newNode->data.code) 
    {
        printf("Memory allocation failed");
        exit(1);
    }
    strcpy(newNode->data.name, name);
    strcpy(newNode->data.code,code);
    newNode->next = NULL;
    freeString(&code);
    return newNode;
}

/*Checks if it is a definition of a macro "mcr" and if so return success
 * the function receives pointer of string*/
error is_mcr_def( char **lineOut) 
{
    char *word;
    getToken(lineOut, &word, " ");
    if (!word)/*if the word is NULL*/
        return emptyArg;
    if (!strcmp(word, "mcro")) /*if there is a macro definition*/
    {
        free(word);
        return success;
    } else /*there is no macro definition*/
    {
        free(word);
        return noMcr;
    }
}

/*Checks if it is a definition of an end of macro "endmcr" and if so return success
 * the function receives pointer of string line */
error is_mcrEnd(char *line)
{
    char* linecpy;
    linecpy= removeWhiteSpace(line);
    if(!strcmp(linecpy,"endmcro"))/*if the line is the end of definition of the macro*/
    {
        return success;
    }
    else return noMcr;
}

/* function checks if it is a name of macro
 * the function receives string of line, list of macros
 * if found name of macro, return success*/
error is_name_of_mcr(char* line,ListMcr * mcrList)
{
    char* word;
    int i;
    NodeMcr * currentNode=mcrList->head;
    if (mcrList->count==0)/*if the list is empty*/
        return noMcr;
    else 
    {
        word=removeWhiteSpace(line);/*removes the whitespaces in the file*/

        for(i=0;i< mcrList->count;i++){/*till the end of the macro list*/
            if(!strcmp(word,currentNode->data.name))/*if the name is a macro name that's in the macro list*/
            {
                return success;
            }
            currentNode=currentNode->next;
        }
    }
    return noMcr;/*the name is not a macro name*/
}

/**
 * This function gets a file that has code in the assembly language.
 * The file needs to be with the extension .as.
 * It creates a file with the extension .am,
 * and checks if the macro definitions in the file are legal,
 * if it's legal it continues and spreads the macros inside of the file and returns success at the end.
 * If it's illegal then the program stops and deletes the file with the extension .am and returns err.
*/
error preAssembler(char* fileName)
{
    FILE *fpAm=NULL, *fileSrc=NULL;
    char *line,*name=NULL,*linecpy;
    int i;
    char *codemcr;
    error err=success;

    /*Building a linked list of macros*/
    ListMcr * mcrList = calloc(1,sizeof (ListMcr));
    NodeMcr * newNode ;
    checkAlloc(mcrList);

    openFile(&fileSrc,fileName, ".as");/*opens the file with the extension .as*/
    if(fileSrc==NULL) /*if there's file with the extension .as or that it does not exist*/
    {
        fprintf(stderr, "unable to open file %s, continuing to next file...", fileName);
        return fileOpeningErr;
    }
    createFile(&fpAm,fileName,".am");/*create file with the extension .am*/
    while (!feof(fileSrc))/*while the file pointer does not points to the end of the file*/
    {
        if( getOneLine(&line,fileSrc)== success)/*get one line from the file*/
        {
            linecpy=(char *) malloc(sizeof (char)*LINE_MAX_LENGTH);
            checkAlloc(linecpy);/*checks if the memory allocation can be made*/
            strcpy(linecpy,line);/*copy the line from the file to linecpy*/

            /*checks name of macro */
            if(!is_name_of_mcr(linecpy,mcrList))/*if there is macro name in the line*/
            {
                NodeMcr * currentNode=mcrList->head;
                linecpy= removeWhiteSpace(linecpy);
                for(i=0;i< mcrList->count;i++){
                    if(!strcmp(linecpy,currentNode->data.name))/*checks for the macro name in the macro list*/ 
                    {
                        break;/*stops when it finds the macro in the macro list*/
                    }
                    currentNode = currentNode->next;
                }
                fputs(currentNode->data.code,fpAm);/*puts the definition inside file with the extension .am*/
                fflush(fpAm);/*flushes the file with the extension .am*/
                freeString(&line);
                freeString(&linecpy);
                linecpy=NULL;
                continue;
            }
            /*checks definition of macro "mcro"*/
            else if(!is_mcr_def(&linecpy))/*if there is a macro definition in  the line*/
            {
                if(strIsAlphaDigit(linecpy)!=success)/*if the first character of the new macro name is not a character in the alphabet*/
                {/*definition of macro with characters */
                    err = mcrNameIncorrect;/*macro name isn't legal*/
                    removeWhiteSpace(line);
                    fprintf(stderr,"Error: The definition of the macro \"%s\" is incorrect\n",line);
                    continue;
                }
                getToken(&linecpy, &name, " ");
                if(name)/*if there is more than word in the macro definition*/
                {/* definition of macro with space*/
                    err = mcrNameIncorrect;/*macro name isn't legal*/
                    removeWhiteSpace(line);
                    fprintf(stderr,"Error: The definition of the macro \"%s\" is incorrect\n",line);
                }
                name= realloc(name,sizeof(char )* strlen(linecpy)+1);
                strcpy(name, linecpy);
                if(isCommand(name)== success) /*if the macro name is not an opcode or an instruction*/
                {/* definition of macro is not a command */
                    freeString(&line);
                    freeString(&linecpy);
                    getOneLine(&line, fileSrc);
                    codemcr = calloc(1, sizeof(char));


                    /*copying code to the node of the list until "endmcro"*/
                    while ((is_mcrEnd(line)) != success) 
                    {
                        codemcr = concatenateStrings(codemcr, line);/*get the line in the macro definition*/
                        getOneLine(&line, fileSrc);
                    }
                    newNode = createNode(name, codemcr);/*put the macro definition inside the node of the macro name*/
                    addToList(newNode, mcrList);/*add the new macro to the list*/
                    codemcr = NULL;
                    freeString(&line);
                    freeString(&name);
                }
                else 
                {
                    err = mcrNameIncorrect;/*macro name is illegal*/
                    fprintf(stderr, "Error: The definition of a macro %s cannot be a name of command\n",name);
                    /*free everyhting that has been allocated*/
                    freeString(&line);
                    freeString(&linecpy);
                    getOneLine(&line, fileSrc);
                    while ((is_mcrEnd(line)) != success)
                    {
                        freeString(&line);
                        getOneLine(&line, fileSrc);
                    }
                    freeString(&line);
                    freeString(&name);
                }
            }
            else /*if there is no makro definition in the line-*/
            {
                fputs(line, fpAm);
                fputs("\n",fpAm);
                fflush(fpAm);
                freeString(&line);
                freeString(&linecpy);
            }
        }
        else break;
    }
    fputs(line, fpAm);
    fputs("\n",fpAm);
    fclose(fileSrc);
    fclose(fpAm);
    if(err!=success )/*if there are errors inside the file*/
    {
        removeFile(fileName,".am");/*remove the file with the extension .am*/
        fprintf(stderr,"cannot create file %s.am\n",fileName);
    }
    clearList(mcrList,NULL);
    freeString(&line);
    freeString(&linecpy);
    freeString(&name);
    free(mcrList);

    return err;
}

/*The function concatenates strings of macro code
 * recevied char * of str1 and str2,
 * str1 is a string of code we have and str2 is a string of code to add*/
char* concatenateStrings(char* str1, char* str2) 
{
    int len1 = (int) strlen(str1);
    int len2 = (int) strlen(str2);
    char *result = malloc(len1 + len2 + 4);

    checkAlloc(result);
    str2 = removeWhiteSpace(str2);/*remove the white spaces from the line*/
    /*concetenate the strings*/
    strcpy(result, str1);
    strcat(result, "\t");
    strcat(result, str2);
    strcat(result, "\n");
    /************************/
    freeString(&str1);
    freeString(&str2);
    return result;
}
