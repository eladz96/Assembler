#include "declarations.h"

extern int* commendTable;
extern addressingModes* addressingModesTable;
extern int numOfOperands[NUM_OF_COMMENDS];
extern symbol* symbolTableHead;
extern codeLine* codePictureHead;
extern dataLine* dataPictureHead;
extern ex_symbol* exSymbolsHead;


/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void freeTablesForNextRun(char *fileName){/*free the memory needed for the next run*/
	
	symbol *tempS=symbolTableHead;
	codeLine *tempC=codePictureHead;
	dataLine *tempD=dataPictureHead;
	ex_symbol *tempX=exSymbolsHead;
	
	free(fileName);
	while(symbolTableHead){ /*freeing the symbol table*/
		symbolTableHead=(symbol*)symbolTableHead->next;
		free(tempS);
		tempS=symbolTableHead;
	}/*end of while*/
	while(codePictureHead){ /*freeing the code table*/
		codePictureHead=(codeLine*)codePictureHead->next;
		free(tempC);
		tempC=codePictureHead;
	}/*end of while*/
	while(dataPictureHead){ /*freeing the data table*/
		dataPictureHead=(dataLine*)dataPictureHead->next;
		free(tempD);
		tempD=dataPictureHead;
	}/*end of while*/
	while(exSymbolsHead){ /*freeing the external symbols table*/
		exSymbolsHead=(ex_symbol*)exSymbolsHead->next;
		free(tempX);
		tempX=exSymbolsHead;
	}/*end of while*/

}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void freeAll(){/*free all the memory the program used*/
	
	free(commendTable);/*freeing the constant tables*/
	free(addressingModesTable);	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/




