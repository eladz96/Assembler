#include "declarations.h"

/*global variables*/
int* commendTable;
addressingModes* addressingModesTable;
symbol* symbolTableHead;
codeLine* codePictureHead;
dataLine* dataPictureHead;
ex_symbol* exSymbolsHead;
int numOfOperands[NUM_OF_COMMENDS]={2,2,2,2,2,1,1,1,1,1,1,1,1,1,0,0};


int main(int argc, char *argv[]){
	
	int IC,DC,ICF,DCF;
	FILE *sourcefp;
	int i;
	char* fileName;
	
	createConstantTables();
	for (i=1;i<argc;++i){
		IC=100;
		DC=0;
		if(!(fileName=(char*)malloc(sizeof(char)*(strlen(argv[i])+4)))){
			fprintf(stderr,"Error malloc faild!\n");
			exit(0);
		}
		strcpy(fileName,argv[i]);	
		strcat(fileName,".as");
		if(!(sourcefp=fopen(fileName,"r"))){/*opening the file and checking if the opening was successfull*/
			fprintf(stderr,"Cannot open file\n");	
			exit(0);			
		}/*end of if*/
		if(firstPass(sourcefp,&IC,&DC)){ /*if the function found errors return 1 as a sign of faliure*/	
			fclose(sourcefp);
			printf("errors were found so moving to the next file\n\n\n\n");					
			freeTablesForNextRun(fileName);/*freeing the memory we used in this run*/			
			continue;
		}
		ICF=IC;/*saving the final values of the counters*/
		DCF=DC;
		updateSymbolTable(ICF);/*upadeting the values of all the symbols in the table*/
		rewind(sourcefp);
		if(secondPass(sourcefp)){ /*if the function found errors return 1 as a sign of faliure*/
			fclose(sourcefp);			
			printf("errors were found so moving to the next file\n\n\n\n");					
			freeTablesForNextRun(fileName);/*freeing the memory we used in this run*/	
			continue;	
		}
		createOutputFile(fileName,ICF,DCF);/*creating the output files*/
		freeTablesForNextRun(fileName);/*freeing the memory we used in this run*/
		fclose(sourcefp);
	}/*end of input loop*/
	freeAll();/*freeing all the memory we used in this run of the assembler*/
	return 0;
}
