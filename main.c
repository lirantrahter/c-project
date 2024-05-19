#include "main.h"

int main(int argc, char *argv[])
{   
    int i;
    for (i = 1; i < argc; i++) 
    {
        if (preAssembler(argv[i]) == success)/*if pre-assembler step is completed with success*/
            firstRun(argv[i]);/*proceed to the first run*/
    }
    return 0;
}