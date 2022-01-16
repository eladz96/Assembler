/*including librarys*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*----------------------------------------*/
/*defining constants*/
#define LINE_LENGTH 82
#define LABEL_LENGTH 32
#define NUM_OF_COMMENDS 16
/*----------------------------------------*/
/*defining structs*/
typedef struct{
	char name[LABEL_LENGTH];
	int val;	
	char attribute;
	int entryFlag;
	struct symbol *next;
}symbol;

typedef struct{
	int address;
	int L;
	int machineCode;
	char attribute;
	struct codeLine *next;
}codeLine;

typedef struct{
	int address;
	int machineCode;
	char attribute;
	struct dataLine *next;	
}dataLine;

typedef struct{
	int source[4];
	int destination[4];	
}addressingModes;

typedef struct {
	char name[LABEL_LENGTH];
	int address;	
	struct ex_symbol *next;
}ex_symbol;
/*----------------------------------------*/
/*tabels file*/
void createCommendTable();/*creates a table that has the binary codes of all the commends*/
void createAddressingModesTable();/*creats a table that has all the data about what addressing mode is leagel for every commend*/
void createConstantTables();/*create all the tables*/

/*firstpass file*/
int firstPass(FILE*,int*,int*);/*a function that do the first pass on the source code, input: file pointer, IC adress, DC adress, return: number of errors found*/
int checkComment(char*);/*check if the line is a comment return 1 if so and 0 if not*/
int isLabel(char*,int*);/*return 0 if there is no label, return the index of ':' if there is a label*/
int whatInLine(char*,int,int);/*check what type of commend is in this line(data,string,entry,extern,empty line,commend) */
int checkEmpty(char*);/*check if the line is empty, if so return 5 if not return 6*/
int addToSymbolTable(char*,int,int*,int,char);/*adds a new symbol to the table, return 0 if there was an error*/
int labelIsSaveWord(char* ,int);/*check if the label name is a save word return 0 if so and 1 if not*/
int checkDuplicates(char*);/*checks if the label already exist in the symbol table return 0 if so and 1 if not*/
void addSymbol(char*,int,char);/*adds the new symbol the the table*/
int addDataToTableNum(char*,int*,int);/*check if the data commend is lleagl if so add the numbers to the data table, return 0 if no errors return 1 if there were errors*/
void addData(int*,int);/*add the number to the table*/
int numberOfDigits(int);/*return how many digit are in the number*/
int addDataToTableStr(char* ,int* ,int);/*check if the string commend is lleagl if so add the charecters to the data table, return 0 if no errors return 1 if there were errors*/
int addExternSymbol(char*,int);/*checks if the label is leagel and add it to the symbo table, reutn 0 if there was an error and 1 if not*/
int checkDuplicatesExtern(char*);/*checks if the label already exist in the symbol table return 1 if exist with an attribute other tnen extern, 2 if exist with the attribute extern 3 if doesnt exist*/
int addCodeToTable(char*,int*,int,int);/*check if the commend is lleagl if so add the commends to the code table, return 0 if no errors return 1 if there were errors*/
int searchCommend(char*);/*check what commend we rescived return its index in the commend table or 16 if its not a llegal commend*/
void addcode(int,int,int,char);/*add a new code to the table*/
int checkAddressingMode(char*,int);/*check what addressing mode the operator use and returns its number 0,1,2,3*/
int checkOperandLeagel(char*,int,int,int*,int);/*check if the operand is legal return 0 if so and 1 if not*/
void updateSymbolTable(int);/*update the values of the symbols in the table*/


/*secondpass file*/
int secondPass(FILE*);/*a function that do the second pass on the source code, input: file pointer return: number of errors found*/
int updateEntryLabel(char*, int, int);/*update the entry flag of the symbol return 0 if successful 1 if there were errors*/
int searchAndUpdateLabel(char*, int);/*search the label in the symbol table and update the entry flag, return 0 if successful 1 if not*/
int addDataWordCode(char*,codeLine**,int,int,int);/*finish coding the machine code of the commend. return 0 if successfull 1 if not*/
int getOperandVal(char*,int,int);/*find the symbol in the symbols table and returns its val, return -1 if the symbol does not exist int the table*/
void addEx_symbol(char*,int,int,int);/*add the operand to the table of operands that use external labels*/

/*outputfile file*/
void createOutputFile(char*,int,int);/*creates the output files*/
int isThereEntry();/*check if there is a symbol that was declared as an entry, return 0 if there is, 1 if not*/
void printfHex(int,FILE*);/*print the machine code in hexdecimal*/

/*freestructs file*/
void freeTablesForNextRun(char*);/*free the memory needed for the next run*/
void freeAll();/*free all the memory the program used*/

