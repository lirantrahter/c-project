#include "main.h"

/**
 * frees the given string in the function
*/
void freeString(char **ptr) 
{
    if (*ptr)/*if the string isn't NULL*/ 
    {
        free(*ptr);
        *ptr = NULL;
    }
}

/*Function receives char *
 * the functions checks if the string is alpha or number if so return success
 * if not returns an error code represented as an error enum*/
error strIsAlphaDigit(char* str)
{
    int i;

    for (i=0; str[i]!= '\0' ; i++) /*till the end of the str*/
    {
        if(!isalnum(str[i]))/*if the character in the i'th index inside the string is a number or it's a letter from the alphabet*/
        {
            return wrongDefLabel;
        }
    }
    return success;
}

/*remove white spaces from the beginning of the string
 * received pointer to string
 * return pointer to string*/
char *removeWhiteSpace(char *str) 
{
    int len = (int) strlen(str), i;
    char *result = str;
    for (i = 0; i < len; i++) 
    {
        if (isspace(str[i]))/*if there is a space in the string*/
            result++;
        else/*if there is no white space*/
            break;
    }
    memmove(str, result, strlen(result) + 1);/*removes the white spaces*/
    return str;/*return the new line without the white spaces*/
}

/*This function takes in a void pointer as an argument and checks if the pointer is null.
 * If the pointer is null, it prints an error message and terminates the program.*/
void checkAlloc(void *test) 
{
    if (!test)/*if test is NULL*/
    {
        perror("Memory allocation error");
        exit(1);
    }
}

/*Frees several pointers, sets them to Null, called with address of the pointer:
 * LAST ONE MUST BE NULL: freeMulti(&a,&b,&c,NULL);*/
void freeMulti(void *ptr, ...) 
{
    void **nextPtr = ptr;/*pointer for the arguments of the function*/
    va_list args;
    va_start(args, ptr);
    while (nextPtr != NULL) /*while the list of the arguments are not NULL*/
    {
        free(*nextPtr);/*free the argument*/
        *nextPtr = NULL;
        nextPtr = va_arg(args, void **);
    }
    va_end(args);
}

/* Function receives a pointer to a FILE pointer and a file path (string)
 * The function opens the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error openFile(FILE **filePointer, char *filePath, char *suffix) 
{
    char *path = (char *) malloc(strlen(filePath) + strlen(suffix) + 1);
    checkAlloc(path);/*checks if the memory allocated successfully*/
    strcpy(path, filePath);/*gets the name of the file*/
    strcat(path, suffix);/*concetanes the name of the file with the extension of the file*/
    *filePointer = fopen(path, "r");/*opens the file*/
    free(path);
    if (!*filePointer) /*if the file can not be oppened*/
    {
        perror("File opening error");
        return fileOpeningErr;
    }
    return success;
}

/* Function receives a pointer to a FILE pointer and a file path (string)
 * The function creates the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error createFile(FILE **filePointer, char *filePath, char *suffix) 
{
    char *path = (char *) malloc(strlen(filePath) + strlen(suffix) + 1);
    checkAlloc(path);
    strcpy(path, filePath);/*gets the name of the file*/
    strcat(path, suffix);/*concetanes the name of the file with the extension of the file*/
    *filePointer = fopen(path, "w"); /*creates the new file with the new extension*/
    free(path);
    if (!*filePointer) /*if the file can not be created*/
    {
        perror("File creating error");
        return fileOpeningErr;
    }
    return success;
}

/* Function receives a file path (string)
 * The function deletes the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error removeFile(char *filePath, char*suffix) 
{
    char *path = (char *) malloc(strlen(filePath) + strlen(suffix) + 1);
    int status;
    checkAlloc(path);
    strcpy(path, filePath);/*gets the name of the file*/
    strcat(path, suffix);/*concetanes the name of the file with the extension of the file*/
    status = remove(path);/*remove the file*/
    free(path);
    if (status != 0)/*if the file cannot be removed*/
        return removingErr;
    return success;
}

/* Function receives a FILE pointer
 * The function closes the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error closeFile(FILE *filePointer) 
{
    return ((fclose(filePointer) == EOF) ? fileClosingErr : success);/*close the file*/
}

/* '*str' and '*token' MUST be freed by the caller!
 * Function receives pointer to:
 * A source string (pointer to char *)
 * An empty Token string (pointer to char *) must be unallocated
 * A list of delimeters (string)
 * Function returns the first token found, and removes if from the source string
 * if none of the delimeter was found, source is unchanged,token will be NULL */
error getToken(char **str, char **token, char *delim) 
{
    int i = 0;
    char *chAfterTok;
    if (!str || !(*str) || !token) /*if one of the arguments is empty*/
    {
        if (token)
            *token = NULL;
        if (str)
            str = NULL;
        return emptyArg;
    }
    /*Skip white spaces from the beginning of the string*/
    while (isspace((*str)[i]))
        i++;
    /*if delimeter is null, copy entire string to token*/
    if (!delim) 
    {
        if (strcmp(*str, ""))/*if the string is empty*/ 
        {
            *token = (char *) malloc(strlen(*str + i) + 1);
            checkAlloc(token);
            strcpy(*token, *str + i);
            free(*str);
            *str = NULL;
        } else
            *token = NULL;
        return success;
    }
    /*If reached the end of the string*/
    if ((*str)[i] == '\0') 
    {
        free(*str);
        *str = NULL;
        *token = NULL;
        return emptyArg;
    }
    /*If delimeter is found. Else, delimeter wasn't found*/
    if ((chAfterTok = strpbrk(*str + i, delim))) 
    {
        *(chAfterTok++) = '\0'; /*put '\0' then increase by one*/
        /*Prepare a place for the token, size of that ever is left in '*str' +1 */
        *token = (char *) calloc(chAfterTok - (*str + i), sizeof(char));
        checkAlloc(*token);
        /*Copy token*/
        memcpy(*token, *str + i, chAfterTok - (*str + i));
        /*Remove token from str by overwriting it*/
        memmove(*str, chAfterTok, strlen(chAfterTok) + 1);
        *str = (char *) realloc(*str, (strlen(*str) + 1));/*free unneeded space from "tail"*/
        checkAlloc(*str);
    } else  /*Leave str as is*/
        *token = NULL;
    return success;
} /*Caller MUST free '*token' and '*str' */

/*This function removes comments from a string by finding the first semicolon: ';'
 * and truncating the string at that point.
 * Then reallocates memory for the modified string and returns a success or error code.*/
error removeComments(char **str) 
{
    char *ch;
    if (str == NULL || *str == NULL)/*if the string is NULL*/
        return emptyArg;
    ch = strpbrk(*str, ";");/*checks if there is comment symbol ";" in the line*/
    if (!ch)/*if there is no comment symbol*/
        return success;
    /*else*/
    *ch = '\0';
    *str = (char *) realloc(*str, 1 + strlen(*str));
    checkAlloc(*str);
    return success;
}

/**
 * enlarges the size of the given string 
*/
char *my_strdup(const char *s) 
{
    size_t len = strlen(s) + 1; /* +1 for null terminator*/
    char *newstr = malloc(len);
    if (newstr == NULL) return NULL; /* handle allocation failure*/
    strcpy(newstr, s);
    return newstr;
}

/* get one line in a time, the function receives pointer to a string and pointer to a file,
 * takes one line from the file and copy it to the receiving string*/
error getOneLine(char **line_out, FILE *fp) 
{
    int buffer_size=LINE_MAX_LENGTH;
    int bytes_readen = 0;
    char *buffer = (char *) malloc(LINE_MAX_LENGTH * sizeof(char));
    checkAlloc(buffer);
    while (1) 
    {
        char current = (char) fgetc(fp);
        if (current == EOF || current == '\n') {/*if current is the EOF value or if current is the new line character*/
            buffer[bytes_readen] = '\0';
            *line_out = my_strdup(buffer);
            free(buffer);
            return (current == EOF) ? endOfFile : success;
        }
        else if (bytes_readen >= LINE_MAX_LENGTH - 1) 
        {
            buffer= (char*) realloc(buffer,(buffer_size*sizeof(char) )*2);
            if (buffer == NULL) {
                return memoryAllocErr;
            }
        }
        else
        {
            if(current == '\r')
                buffer[bytes_readen++] = '\0';
            buffer[bytes_readen++] = current;
        }
    }
}