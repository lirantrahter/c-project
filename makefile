assembler: main.o pre_assembler.o functions.o assembler.o
	       gcc -ansi -Wall -pedantic -g main.o pre_assembler.o functions.o assembler.o -o assembler -lm

main.o: main.c main.h
			 gcc -c -ansi -Wall -pedantic main.c -o main.o

pre_assembler.o: pre_assembler.c pre_assembler.h
				 gcc -c -ansi -Wall -pedantic pre_assembler.c -o pre_assembler.o

functions.o: functions.c 
		     gcc -c -ansi -Wall -pedantic functions.c -o functions.o

assembler.o: assembler.c assembler.h
			 gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o