assembler: assembler.o firstpass.o secondpass.o outputfile.o freestructs.o tabels.o
	gcc -g -ansi -Wall -pedantic assembler.o firstpass.o secondpass.o outputfile.o freestructs.o tabels.o -o assembler -lm

assembler.o: assembler.c declarations.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

firstpass.o: firstpass.c declarations.h
	gcc -c -ansi -Wall -pedantic firstpass.c -o firstpass.o

secondpass.o: secondpass.c declarations.h
	gcc -c -ansi -Wall -pedantic secondpass.c -o secondpass.o

outputfile.o: outputfile.c declarations.h
	gcc -c -ansi -Wall -pedantic outputfile.c -o outputfile.o 

freestructs.o: freestructs.c declarations.h
	gcc -c -ansi -Wall -pedantic freestructs.c -o freestructs.o

tabels.o: tabels.c declarations.h
	gcc -c -ansi -Wall -pedantic tabels.c -o tabels.o
