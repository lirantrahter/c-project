#include "assembler.h"

/*This function converts a decimal number to binary and stores the binary representation in a character array.
 * It takes the character array, the decimal number, and the size of the array as arguments.
 * It converts the decimal number to binary using the modulo operator and stores the binary digits as either a dot or a slash in the character array.
 * It pads the remaining elements of the character array with dots and then reverses the order of the elements in the array.*/
void binaryOf(char binaryForm[], unsigned num, int size) 
{
    int i = 0;
    while (num > 0 && i < size) 
    {
        if (num % 2 == 0)
            binaryForm[i] = '0';
        else
            binaryForm[i] = '1';
        num /= 2;
        i++;
    }
    while (i < size)
        binaryForm[i++] = '0';
    reverseSTR(binaryForm, size);
}

/*This function reverses a string by swapping the first and last character,
 * then moving inwards until the entire string is reversed.
 * It takes the string and its size as arguments and modifies the string in place.*/
void reverseSTR(char str[],int size) 
{
    int a = 0, b = 0;
    char temp;
    while (b < size - 1 && str[b + 1] != '\0')
        b++;
    while (b >= a) {
        temp = str[a];
        str[a] = str[b];
        str[b] = temp;
        a++;
        b--;
    }
}

/**
 * This function gets a string  of binary number with 12 bits 
 * and changes the number to base 64 in the given char base64.
*/
void binaryTo64(char binary[13], char *base64)
{   
    int i = 0, j = 0, temp = 0;
    char char_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    for(j = 0, i = 5; i >= 0 && j < 6; i--, j++)
    {
        temp += ((binary[j] - '0') * pow(2, i));
    }
    base64[0] = char_set[temp];
    
    temp = 0;
    for(j = 6, i = 5; i >= 0 && j < 13; i--, j++)
    {
        temp += ((binary[j] - '0') * pow(2, i));
    }
    base64[1] = char_set[temp];

    base64[2] = '\0';
}

/* function takes a pointer to a string (char*) which is dynamically allocated, and five unallocated pointers to string (char*)
 * function divides the given string to Label, command and two arguments (depending on each existence) */
error clearWhiteSpace(char **line) 
{
    int i = 0;
    if (!line || !(*line))
        return emptyArg;

    while ((*line)[i] != '\0') 
    {
        if (isspace((*line)[i])) 
        {
            (*line)[i] = '\0';
            strcat(*line, *line + i + 1);
        } else
            i++;
    }
    *line = (char *) realloc(*line, sizeof(char) * (strlen(*line) + 1));
    checkAlloc(*line);
    if (!strcmp(*line, ""))
        freeMulti(line, NULL);
    return success;
}

/*Function verifies and identifies the command*/
error idCommand(char *command, opcode *op) 
{
    if (!command)
        return emptyArg;
    *op = none;
    if (!strcmp(command, "mov"))
        *op = mov;
    if (!strcmp(command, "cmp"))
        *op = cmp;
    if (!strcmp(command, "add"))
        *op = add;
    if (!strcmp(command, "sub"))
        *op = sub;
    if (!strcmp(command, "not"))
        *op = not;
    if (!strcmp(command, "clr"))
        *op = clr;
    if (!strcmp(command, "lea"))
        *op = lea;
    if (!strcmp(command, "inc"))
        *op = inc;
    if (!strcmp(command, "dec"))
        *op = dec;
    if (!strcmp(command, "jmp"))
        *op = jmp;
    if (!strcmp(command, "bne"))
        *op = bne;
    if (!strcmp(command, "red"))
        *op = red;
    if (!strcmp(command, "prn"))
        *op = prn;
    if (!strcmp(command, "jsr"))
        *op = jsr;
    if (!strcmp(command, "rts"))
        *op = rts;
    if (!strcmp(command, "stop"))
        *op = stop;
    if (!strcmp(command, ".data"))
        *op = data;
    if (!strcmp(command, ".string"))
        *op = string;
    if (!strcmp(command, ".entry"))
        *op = entry;
    if (!strcmp(command, ".extern"))
        *op = external;
    if (*op == none)
        return undefinedCommand;
    return success;
}

/* Function to create a new node*/
struct Node* createNodeFirstRun(char * name, opcode opcode, int place, char* instructionCode) 
{
    /* Allocate memory for the new node*/
    struct Node *newNode = (struct Node *) calloc(1, sizeof(struct Node));
    checkAlloc(newNode);
    strcpy(newNode->data.InstructionCode, instructionCode);
    strcpy(newNode->data.name, name);
    newNode->data.type = opcode;
    newNode->data.place = place;
    newNode->next = NULL;
    return newNode;
}

/*Function verifies and identifies the label*/
static error idLabel(char* arg) 
{   
    int i = 1;   
    if (strlen(arg) >= LABEL_MAX_SIZE) 
    {
        fprintf(stderr, "\tLabel too long\n");
        return unknownArg;
    }
    removeWhiteSpace(arg);
    if (strpbrk(arg, " \t") != NULL) 
    {
        fprintf(stderr, "\tLabel cannot contain white spaces\n");
        return unknownArg;
    }
    if (!isalpha(arg[0])) 
    {
        fprintf(stderr, "\tLabel must start with letter\n");
        return unknownArg;
    }
    if(strchr(arg,',')) 
    {
        fprintf(stderr, "\tLabel or arg cannot contain \',\',\n");
        return unknownArg;
    }
    while(i < strlen(arg))
    {
        if(!isalpha(arg[i]) && !isdigit(arg[i]))
        {
            fprintf(stderr, "\tLabel cannot contain non-digits or non-letters\n");
            return unknownArg;
        }
        i++;
    }
    return success;
}

/*Function adds a given node to a given list*/
static error addToList(list *lst, Node **nodeToAdd) 
{
    Node *ptr = lst->head;
    (*nodeToAdd)->next = NULL;
    (*nodeToAdd)->next = NULL;
    if (ptr) 
    {
        while (ptr->next)
            ptr = ptr->next;
        ptr->next = *nodeToAdd;
    } else lst->head = *nodeToAdd;
    lst->count++;
    *nodeToAdd = NULL;
    return success;
}

/*search node by name
 * checks only if the names are the same
 * received pointer to list and name (char*)*/
error searchNode(list* list, char* name,Node **nodeOut) 
{
    Node *currentNode = list->head;
    int i;
    if (list->count == 0)
        return success;
    else {
        for (i = 0; i < list->count; i++) 
        {
            if (!strcmp(name, currentNode->data.name)) 
            {
                *nodeOut = currentNode;
                return labelExists;
            }
            currentNode = currentNode->next;
        }
    }
    return success;
}

/*function clears a linked list by recursively freeing each node of the list.
 * It takes the pointer to the list and the current node as arguments.
 * If the current node is not null, it recursively calls itself with the next node in the list until it reaches the end.
 * It then frees the current node and sets the node pointer to null.
 * Finally, it updates the list count and returns a success or error code.
 * If the node pointer is null, it starts the recursion from the head of the list.*/
static error clearList(list *lst, Node **node) 
{
    if (node && *node) 
    {
        if ((*node)->next)
            clearList(lst, &((*node)->next));
        free(*node);
        *node = NULL;
        lst->count--;
    } else if (lst->head)
        clearList(lst, &(lst->head));
    return success;
}

/*This function parses a line of data and adds each data item to a linked list.
 * It takes the line, a pointer to the linked list, and a counter as arguments.
 * It uses getToken to extract each data item from the line, converts it to binary using binaryOf, and adds it to a new node in the linked list.
 * It returns a success or error code based on the outcome of the operation. It also handles errors for consecutive commas, unrecognized arguments, or missing commas.*/
error codeData(char *line, list *dataList, int *counter) 
{
    char *word = NULL, *endPTR = NULL;
    int num = 0, i;
    Node *newNode = NULL;
    getToken(&line, &word, ",");
    if (!word)
        getToken(&line, &word, NULL);
    while (word) 
    {
        if (!word || !strcmp(word, "")) 
        {
            fprintf(stderr, "\tConsecutive commas in .data command\n");
            return consecutiveCommas;
        }
        num = strtol(word, &endPTR, 10);
        if (strcmp(endPTR, "")) {
            for (i = 0; i < strlen(endPTR); ++i) 
            {
                if (!isspace(endPTR[i])) 
                {
                    fprintf(stderr, "\tAn unrecognized argument in .data, or missing comma(s)\n");
                    freeMulti(&word, &line, NULL);
                    return unknownArg;
                }
            }
        }
        newNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(newNode);
        binaryOf(newNode->data.InstructionCode, num, sizeof(newNode->data.InstructionCode) - 1);
        newNode->data.place = (*counter)++;
        addToList(dataList, &newNode);
        free(word);
        getToken(&line, &word, ",");
        if (!word)
            getToken(&line, &word, NULL);
    }
    freeMulti(&word, &line, NULL);
    return success;
}

/*Function takes a string argument and converts it to a sequence of binary values, which are then stored in a linked list.
 * The function checks if the string is in a valid format for a .string command, and if so, it iterates over the characters in the string and adds each one to the linked list in binary form.
 * If there are any errors encountered during the process, the function frees any memory that was allocated and returns an error code.*/
error codeString(char *line, list *dataList, int *counter) 
{
    Node *newNode, *OGNode = dataList->head;
    int i = 0;
    char ch;
    if (line == NULL)
        return emptyArg;
    ch = line[i];
    while (OGNode)
        OGNode = OGNode->next;
    while (ch != '\0' && ch != '\"')
    {
        printf("%c\n", ch);
        if(isalpha(ch))
        {
            printf("%c\n", ch);
            free(line);
            fprintf(stderr, "\tA .string command is missing an argument\n");
            return tooManyArg;
        }
        ch = line[i++];
    }
    if (ch == '\0')
    {
        free(line);
        fprintf(stderr, "\tA .string command is missing an argument\n");
        return missingParentheses;
    }
    ch = line[++i];
    while (ch != '\0' && ch != '\"') 
    {
        newNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(newNode);
        binaryOf(newNode->data.InstructionCode, line[i], DATA_SIZE);
        newNode->data.place = (*counter)++;
        addToList(dataList, &newNode);
        ch = line[++i];
    }
    newNode = (Node *) calloc(1, sizeof(Node));
    checkAlloc(newNode);
    binaryOf(newNode->data.InstructionCode, '\0', DATA_SIZE);
    newNode->data.place = (*counter)++;
    addToList(dataList, &newNode);
    if (ch != '\"') 
    {
        (*counter) -= dataList->count;
        clearList(dataList, &OGNode);
        (*counter) += dataList->count;
        fprintf(stderr, "\tA .string command is missing a closing parentheses\n");
        free(line);
        return missingClosingParentheses;
    }
    while ((ch = line[++i]) != '\0') 
    {
        if (!isalpha(ch) && !isspace(ch)) 
        {
            (*counter) -= dataList->count;
            clearList(dataList, &OGNode);
            (*counter) += dataList->count;
            fprintf(stderr, "\tA .string command has too many args\n");
            free(line);
            return tooManyArg;
        }
    }
    free(line);
    return success;
}

/*The function parses the line and checks for errors in the syntax and semantics of the instruction based on the opcode command.
 * It then generates the binary code for the instruction and adds it to the list of instructions.
 * If a label is present, the function also generates a label node and adds it to the list of instructions.*/
error codeCommand (char *line, list *instructionList, opcode command, int *count) 
{
    addressMethod am1 = 0,/*src*/ am2 = 0/*dest*/;
    char *label = NULL, *arg1 = NULL, *arg2 = NULL, *endPTR;
    int argVal;
    Node *newNode = (Node *) calloc(1, sizeof(Node));
    checkAlloc(newNode);
    switch (command) 
    {
        case jmp:
        case bne:
        case jsr:
            am1 = 0;
            if (!label) 
            {
                getToken(&line, &arg2, NULL);
                if (arg2 && strpbrk(arg2, ",\t ")) 
                {
                    fprintf(stderr, "\tCommand has too many arguments\n");
                    return tooManyArg;
                }
                idArg(&arg2, &am2);
                arg1 = NULL;
            } else 
            {
                clearWhiteSpace(&line);
                if (line) 
                {
                    fprintf(stderr, "\tCommand has too many arguments\n");
                    return tooManyArg;
                }        
            }
            if (arg2 == NULL) 
            {
                fprintf(stderr, "\tCommand is missing arguments\n");
                return missingArg;
            }
            break;
        case mov:
        case add:
        case sub:
        case lea:
        case cmp:
            getToken(&line, &arg1, ",");
            getToken(&line, &arg2, NULL);
            if (!arg2 || !arg1 || !strcmp(arg1, "")) 
            {
                fprintf(stderr, "\tCommand is missing arguments\n");
                return missingArg;
            }
            if (strpbrk(arg2, ", \t") != NULL) 
            {
                fprintf(stderr, "\tConsecutive commas or Too many args\n");
                return consecutiveCommas;
            }
            idArg(&arg1, &am1);
            idArg(&arg2, &am2);
            if ((command == lea && am1 != direct) || am2 == immediate) 
            {
                if(command == cmp)
                    break;
                fprintf(stderr, "\tCommand has a wrong type of argument1\n");
                return wrongArg;
            }
            break;
        case not:
        case clr:
        case inc:
        case dec:
        case red:
        case prn:
            label = arg1 = NULL;
            am1 = 0;/**/
            getToken(&line, &arg2, NULL);
            if (!arg2) 
            {
                fprintf(stderr, "\tCommand is missing arguments\n");
                return missingArg;
            }
            if (strpbrk(arg2, ", \t")) 
            {
                fprintf(stderr, "\tCommand has too many arguments\n");
                return tooManyArg;
            }
            if (idArg(&arg2, &am2) == success  &&  !(command == prn || command == not) && am2 == immediate) /*changed*/
            {
                fprintf(stderr, "\tCommand has a wrong type of argument\n");
                return wrongArg;
            }
            break;
        case stop:
        case rts:
            am1 = am2 = 0;
            label = arg1 = arg2 = NULL;
            clearWhiteSpace(&line);
            if (line) 
            {
                fprintf(stderr, "\tCommand has too many arguments\n");
                return tooManyArg;
            }
            break;
        default:
            break;
    }
    binaryOf(newNode->data.InstructionCode + OPCODE_START, command, OPCODE_SIZE);
    binaryOf(newNode->data.InstructionCode + SRC_AM_START, am1, AM_SIZE);
    binaryOf(newNode->data.InstructionCode + DEST_AM_START, am2, AM_SIZE);
    binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, ARE_SIZE);
    newNode->data.place = (*count)++;
    addToList(instructionList, &newNode);
    if (label) 
    {
        newNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(newNode);
        newNode->data.place = (*count)++;
        clearWhiteSpace(&label);
        strcpy(newNode->data.name, label);
        addToList(instructionList, &newNode);
    }

    if (arg1) 
    {
        newNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(newNode);
        newNode->data.place = (*count)++;
        switch (am1) 
        {
            case immediate:
                argVal = strtol(arg1, &endPTR, 10);
                if (strcmp(endPTR, "")) 
                {
                    fprintf(stderr, "\tCommand has an unknown argument\n");
                    return unknownArg;
                }
                binaryOf(newNode->data.InstructionCode, argVal, IMMEDIATE_SIZE);
                binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, ARE_SIZE);
                addToList(instructionList, &newNode);
                break;
            case directRegister:
                binaryOf(newNode->data.InstructionCode + REGI_1_START, arg1[2] - '0', REGI_SIZE);/*change arg1[1] Because @*/
                binaryOf(newNode->data.InstructionCode + REGI_2_START, 0, REGI_SIZE);
                binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, ARE_SIZE);
                if (am2 != directRegister)
                    addToList(instructionList, &newNode);
                break;
            default:
                strcpy(newNode->data.name, arg1);
                addToList(instructionList, &newNode);
                break;
        }
    }
    if (arg2) 
    {
        switch (am2) 
        {
            case immediate:
                newNode = (Node *) calloc(1, sizeof(Node));
                checkAlloc(newNode);
                newNode->data.place = (*count)++;
                argVal = strtol(arg2, &endPTR, 10);
                if (strcmp(endPTR, "")) 
                {
                    fprintf(stderr, "\tCommand has an unknown argument\n");
                    return unknownArg;
                }
                binaryOf(newNode->data.InstructionCode, argVal, IMMEDIATE_SIZE);
                binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, 2);
                break;
            case directRegister:
                if (!newNode) 
                {
                    newNode = (Node *) calloc(1, sizeof(Node));
                    checkAlloc(newNode);
                    newNode->data.place = (*count)++;
                    binaryOf(newNode->data.InstructionCode + REGI_1_START, 0, REGI_SIZE);
                }
                binaryOf(newNode->data.InstructionCode + REGI_2_START, arg2[2] - '0', REGI_SIZE);/*change arg1[1] Because @*/
                binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, ARE_SIZE);
                break;
            default:
                newNode = (Node *) calloc(1, sizeof(Node));
                checkAlloc(newNode);
                newNode->data.place = (*count)++;
                strcpy(newNode->data.name, arg2);
                break;
        }
        addToList(instructionList, &newNode);
    }
    freeMulti(&line, &label, &arg1, &arg2, NULL);
    return success;
}

/*function prints a linked list by recursively printing each node of the list.
 * It takes the pointer to the list and the current node as arguments.
 * If the current node is not null, it recursively calls itself with the next node in the list until it reaches the end.
 * It then prints the current node and sets the node pointer to null.
 * Finally, it updates the list count and returns a success or error code.
 * If the node pointer is null, it starts the recursion from the head of the list.
 * Prints the code of the node or name is code is empty*/
error printList (list *lst, Node **node) 
{
    if (node && *node) 
    {
        printf("%04d\t", (*node)->data.place);
        if (strcmp((*node)->data.InstructionCode, ""))
            printf("%s\n", (*node)->data.InstructionCode);
        else
            printf("%s\n", (*node)->data.name);
        if ((*node)->next)
            printList(lst, &((*node)->next));
    } else if (lst->head)
        printList(lst, &(lst->head));
    return success;
}


error codeLabel (opcode type, char* line , list* labelList) 
{
    error err;
    Node *node = NULL, *newNode = NULL;
    char *label = NULL;
    getToken(&line, &label, ",");
    if (!line) 
    {
        fprintf(stderr, "\tError: Missing label \n");
        free(label);
        return missingArg;
    }
    if(idLabel(line)!=success) 
    {
        freeMulti(&label, &line, NULL);
        return tooManyArg;
    }
    if (strlen(line) > LABEL_MAX_SIZE) 
    {
        fprintf(stderr, "\tError: The definition of the label %s is too long\n", line);
        freeMulti(&label, &line, NULL);
        return labelTooLong;
    }
    if (label == NULL) 
    {
        clearWhiteSpace(&line);
        if ((err = strIsAlphaDigit(line)) != success && isalpha(line[0])) 
        {
            fprintf(stderr, "\tError: The definition of label %s is incorrect\n", line);
            freeMulti(&label, &line, NULL);
            return wrongDefLabel;
        }
        getToken(&line, &label, NULL);
        clearWhiteSpace(&label);
        if (line == NULL) 
        {
            if ((err = searchNode(labelList, label, &node)) == success) 
            {
                newNode = createNodeFirstRun(label, type, -1, "");
                addToList(labelList, &newNode);
                freeMulti(&label, &line, &newNode, NULL);
                return success;
            } 
            else if (err == labelExists) 
            {
                if (node->data.type != type)
                 {
                    /*fixes label "place" problem:*/
                    if (node->data.type != entry && node->data.type != external)
					{
						if(type == external)
						{
							fprintf(stderr, "\t            Error: The definition of label %s already exists\n", node->data.name);
                    		freeMulti(&label, &line, &newNode, NULL);
                    		return labelExists;
						} else node->data.type = type;
                    }
                    else 
                    {
                        newNode = createNodeFirstRun(label, type, -1, "");
                        addToList(labelList, &newNode);
                    }
                    freeMulti(&label, &line, &newNode, NULL);
                    return success;
                } 
                else 
                {
                    fprintf(stderr, "\t            Error: The definition of label %s already exists\n", node->data.name);
                    freeMulti(&label, &line, &newNode, NULL);
                    return labelExists;
                }
            } else if (line == NULL) 
            {
                fprintf(stderr, "\tError: The definition of label %s is already exists\n", node->data.name);
                freeMulti(&label, &line, &newNode, NULL);
                return labelExists;
            }
        }
    } else if ((err = strIsAlphaDigit(line)) != success) 
    {
        fprintf(stderr, "\tError: Definition of two labels at once\n");
        freeMulti(&label, &line, &newNode, NULL);
        return wrongDefLabel;
    } else 
    {
        fprintf(stderr, "\tError: The definition of the label %s is incorrect\n", line);
        freeMulti(&label, &line, &newNode, NULL);
        return wrongDefLabel;
    }
    freeMulti(&label, &line, &newNode, &node, NULL);
    return success;
}

/*This function sets the address of a label in a linked list of labels.
 * It first checks if the label exists in the list and if not, it adds a new node for the label.
 * If the label already exists, it checks if the place attribute of the node is already set and if so, it returns an error.
 * Finally, it sets the place attribute of the relevant node to the given address.
 * The function returns an error code indicating success or failure.*/
error setLabelAddress ( list *lst, char* name,int address, opcode op) 
{
    Node *relevantNode, *temp;
    if (idLabel(name) == success) 
    {
        if (searchNode(lst, name, &relevantNode) != labelExists) 
        {
            relevantNode = (Node *) calloc(1, sizeof(Node));
            checkAlloc(relevantNode);
            strcpy(relevantNode->data.name, name);
            relevantNode->data.type = op;
            temp = relevantNode;
            addToList(lst, &relevantNode);
            relevantNode = temp;
        } else if (relevantNode->data.place != -1)
            return labelExists;
        else
        {
            relevantNode->data.place = address;
        }
        relevantNode->data.place = address;
        return success;
    }
    return unknownArg;
}

/*Function verifies and identifies the arguments*/
error idArg(char **arg, addressMethod *amArg) 
{
    clearWhiteSpace(arg);
    if (arg == NULL || (*arg) == NULL) 
    {
        *amArg = 0;
        return emptyArg;
    }
    if (((*arg)[0] == '-' && isdigit((*arg)[1])) || isdigit((*arg)[0]))/**/
    {
        *amArg = immediate;
        return success;
    } else if (strlen(*arg) == REGI_NAME_SIZE && (*arg)[0] == '@' && (*arg)[1] == 'r' && '0' <= (*arg)[2] && (*arg)[2] <= '7') 
    {
        *amArg = directRegister;
        return success;
    } else if (idLabel(*arg) == success) 
    {
        *amArg = direct;
        return success;
    }
    return unknownArg;
}

/*This is a function that takes a file path as input and performs an initial run through the file to extract information such as labels, commands, and data.
 * It then modifies the label and data information and passes it along to a second function for further processing.
 * The function returns an error flag indicating success or failure.*/
error firstRun (char *path) 
{
    Node *node, *temp;
    FILE *stream;
    error errForSecond = success, errFlag = success;
    opcode commandOP;
    list dataList = {0};
    list labelList = {0};
    list instructionList = {0};
    int IC = 100, DC = 0;
    char *label, *word, *line;
    openFile(&stream, path, ".am");
    printf("Your code:\n");
    while (!feof(stream)) 
    {
        getOneLine(&line, stream);
        removeComments(&line);
        if(strlen(line)>LINE_MAX_LENGTH) 
        {
            fprintf(stderr, "Line length cannot exceed 80 chars (comments exclusive).");
            free(line);
            continue;
        }
        printf("%s\n", line);
        getToken(&line, &label, ":"); /*Get label if exists*/
        getToken(&line, &word, " \t\n"); /*Get first word of line*/
        if (!word)
            getToken(&line, &word, NULL);
        if (!line && !label && !word) 
        {
            freeMulti(&word, &line, &label, NULL);
            continue;
        }
        errFlag = idCommand(word, &commandOP); /*Identify the command*/
        if (label && (commandOP != entry && commandOP != external)) 
        {
            errFlag = setLabelAddress(&labelList, label, (commandOP == data || commandOP == string ? DC : IC),commandOP);
            if (errFlag != success)
                errForSecond = errFlag;
        } 
        /***************for label definition after external definition*************************************/
        if(label != NULL)
            searchNode(&labelList, label, &temp);
        if((label && temp) && (temp->data.type == external))
        {
            fprintf(stderr, "\tError: The definition of label %s already exists\n", label);
            errForSecond = wrongDefLabel;
        }
        /*************************************************************************************************/
        switch (commandOP) 
        {
            case none:
                fprintf(stderr, "\tUndefined command encountered\n");
                free(line);
                break;
            case data:
                errFlag = codeData(line, &dataList, &DC);
                break;
            case string:
                errFlag = codeString(line, &dataList, &DC);
                break;
            case external:
            case entry:
                if (label) 
                {
                    errFlag = meaninglessLabel;
                    freeMulti(&label, NULL);
                }
                errFlag = codeLabel(commandOP, line, &labelList);
                break;
            default:
                errFlag = codeCommand(line, &instructionList, commandOP, &IC);
                break;
        }
        freeMulti(&label, &word, NULL);
        if (errFlag != success)
            errForSecond = errFlag;
    }
    node = dataList.head;
    while (node) 
    {
        node->data.place += IC;
        node = node->next; 
    }
    node = labelList.head;
    while (node) 
    {
        if ((node->data.type == data || node->data.type == string || node->data.type == entry ||
            node->data.type == external) && (node->data.place < 100))
            node->data.place += IC;
        node = node->next;
    }
    if(errForSecond==success)
        fprintf(stderr,"\nfirst run executed successfully.\n");
    else
        fprintf(stderr,"Error(s) were found in your code, cannot assemble.\n");
    closeFile(stream);


    secondRun(&dataList, &labelList, &instructionList, path, errForSecond,IC-100,DC);
    clearList(&instructionList, NULL);
    clearList(&labelList, NULL);
    clearList(&dataList, NULL);
    return errFlag;
}

/*The function performs a second run, it received lists of labels,
 * instructions code and data, pointer of string of file name, and error flag
 * the function complete all information and if there is not an error creates file of obj
 * if there is a entry label creates file of entries
 * and if there is a external label creates file of external
 * returns success
 * */
error secondRun(list* dataList, list* labelList, list* instructionList,char* fileName, error errFlag,int IC,int DC) 
{
    char *base64 = (char *)(malloc((sizeof(char *) * 2)));
    Node *currentNode = NULL, *nodeOut = NULL;
    int i, cntEnt = 0, cntExt = 0;
    FILE *fpObj = NULL, *fpEnt = NULL, *fpExt = NULL;
    currentNode = instructionList->head;
    for (i = 0; i < instructionList->count; i++) 
    {
        /*If it's a label and not an encoded line*/
        if (isalpha(currentNode->data.name[0])) 
        {
            if (searchNode(labelList, currentNode->data.name, &nodeOut) == labelExists) 
            {
                /*write binary code of the label*/
                if (nodeOut->data.type == external) 
                {
                    cntExt++;
                    binaryOf(currentNode->data.InstructionCode, 0, ADDRESS_SIZE);
                    strcpy(currentNode->data.InstructionCode + ARE_START, "01");
                } else if (nodeOut->data.type == entry) 
                {
                    cntEnt++;
                    binaryOf(currentNode->data.InstructionCode, nodeOut->data.place, ADDRESS_SIZE);
                    strcpy(currentNode->data.InstructionCode + ARE_START, "10");
                } else 
                {
                    binaryOf(currentNode->data.InstructionCode, nodeOut->data.place, ADDRESS_SIZE);
                    strcpy(currentNode->data.InstructionCode + ARE_START, "10");
                }
            } else 
            {
                errFlag = missingLabel;
                fprintf(stderr, "\tmissing label %s \n", currentNode->data.name);
            }
        }
        currentNode = currentNode->next;
    }
    /*There were no errors*/
    if (errFlag == success) 
    {
        createFile(&fpObj, fileName, ".ob");
        /*write the instruction codes into new file ".ob"*/
        currentNode = instructionList->head;
        fprintf(fpObj,"%d   %d\n",IC,DC);
        for (i = 0; i < instructionList->count; i++) 
        {
            binaryTo64(currentNode->data.InstructionCode, base64);
            fprintf(fpObj, "%s", base64);
            fputs("\n", fpObj);
            currentNode = currentNode->next;
        }

        currentNode = dataList->head;
        for (i = 0; i < dataList->count; i++) 
        {
            binaryTo64(currentNode->data.InstructionCode, base64);
            fprintf(fpObj, "%s", base64);
            fputs("\n", fpObj);
            currentNode = currentNode->next;
        }
        fclose(fpObj);

        /*there is entry*/
        if (cntEnt) 
        {
            createFile(&fpEnt, fileName, ".ent");
            currentNode = labelList->head;
            for (i = 0; i < labelList->count; i++) 
            {
                if (currentNode->data.type == entry) 
                {
                    fputs(currentNode->data.name, fpEnt);
                    fputs("\t", fpEnt);
                    binaryOf(currentNode->data.InstructionCode,currentNode->data.place,DATA_SIZE);
                    fprintf(fpEnt,"%d",currentNode->data.place);
                    fputs("\n", fpEnt);
                }
                currentNode = currentNode->next;
            }
            fclose(fpEnt);
        }
        /*there is extern*/
        if (cntExt) 
        {
            createFile(&fpExt, fileName, ".ext");
            currentNode = instructionList->head;
            for (i = 0; i < instructionList->count; i++) 
            {
                if (isalpha(currentNode->data.name[0])) 
                {
                    searchNode(labelList, currentNode->data.name, &nodeOut);
                    if (nodeOut->data.type == external) 
                    {
                        fputs(currentNode->data.name, fpExt);
                        fputs("\t", fpExt);
                        fprintf(fpExt,"%d",currentNode->data.place);
                        fputs("\n", fpExt);
                    }
                }
                currentNode = currentNode->next;
            }
            fclose(fpExt);
        }
    }
    else 
    {
        fprintf(stderr, "Errors founds, cannot create file %s.ob\n", fileName);
        return success;
    }
    free(base64);
    fprintf(stderr,"second run executed successfully.\n" );
    fprintf(stderr, "file %s.ob created successfully.\n", fileName);
    return success;
}
