#include "declarations.h"

extern int* commendTable;
extern addressingModes* addressingModesTable;
extern int numOfOperands[NUM_OF_COMMENDS];
extern symbol* symbolTableHead;
extern codeLine* codePictureHead;
extern dataLine* dataPictureHead;
extern ex_symbol* exSymbolsHead;

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void createConstantTables(){/*create all the tables*/
	createCommendTable();/*create the commend table*/
	createAddressingModesTable();/*create the addressing modes table*/
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void createCommendTable(){/*creates a table that has the binary codes of all the commends*/
	int i=0;
	
	if (!(commendTable=(int*)malloc(sizeof(int)*NUM_OF_COMMENDS))){ /*getting the memory for the table and checking if the malloc was seccessfull*/
		fprintf(stderr,"Error malloc faild!\n");
		exit(0);
	}/*end of if*/
	for (i=0;i<NUM_OF_COMMENDS;++i)
		commendTable[i]=0;/*initializing the table*/
	commendTable[0]=0; 		 /*mov*/
	commendTable[1]=(1<<8);		 /*cmp*/
	commendTable[2]=(2<<8)+(10<<4);  /*add*/
	commendTable[3]=(2<<8)+(11<<4);	 /*sub*/
	commendTable[4]=(4<<8);		 /*lea*/
	commendTable[5]=(5<<8)+(10<<4);	 /*clr*/
	commendTable[6]=(5<<8)+(11<<4);	 /*not*/
	commendTable[7]=(5<<8)+(12<<4);	 /*inc*/
	commendTable[8]=(5<<8)+(13<<4);	 /*dec*/
	commendTable[9]=(9<<8)+(10<<4);	 /*jmp*/
	commendTable[10]=(9<<8)+(11<<4); /*bne*/
	commendTable[11]=(9<<8)+(12<<4); /*jsr*/
	commendTable[12]=(12<<8);	 /*red*/
	commendTable[13]=(13<<8);	 /*prn*/
	commendTable[14]=(14<<8);	 /*rts*/
	commendTable[15]=(15<<8);	 /*stop*/
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void createAddressingModesTable(){/*creats a table that has all the data about what addressing mode is leagel for every commend*/

	if(!(addressingModesTable=(addressingModes*)malloc(sizeof(addressingModes)*(NUM_OF_COMMENDS-2)))){ /*getting the memory for the table and checking if the malloc was seccessfull*/
		fprintf(stderr,"Error malloc faild!\n");
		exit(0);
	}/*end of if*/
	/*mov*/
	addressingModesTable[0].source[0]=1;
	addressingModesTable[0].source[1]=1;	
	addressingModesTable[0].source[2]=0;
	addressingModesTable[0].source[3]=1;	
	addressingModesTable[0].destination[0]=0;
	addressingModesTable[0].destination[1]=1;	
	addressingModesTable[0].destination[2]=0;
	addressingModesTable[0].destination[3]=1;	
	/*cmp*/
	addressingModesTable[1].source[0]=1;
	addressingModesTable[1].source[1]=1;	
	addressingModesTable[1].source[2]=0;
	addressingModesTable[1].source[3]=1;	
	addressingModesTable[1].destination[0]=1;
	addressingModesTable[1].destination[1]=1;	
	addressingModesTable[1].destination[2]=0;
	addressingModesTable[1].destination[3]=1;	
	/*add*/
	addressingModesTable[2].source[0]=1;
	addressingModesTable[2].source[1]=1;	
	addressingModesTable[2].source[2]=0;
	addressingModesTable[2].source[3]=1;	
	addressingModesTable[2].destination[0]=0;
	addressingModesTable[2].destination[1]=1;	
	addressingModesTable[2].destination[2]=0;
	addressingModesTable[2].destination[3]=1;	
	/*sub*/
	addressingModesTable[3].source[0]=1;
	addressingModesTable[3].source[1]=1;	
	addressingModesTable[3].source[2]=0;
	addressingModesTable[3].source[3]=1;	
	addressingModesTable[3].destination[0]=0;
	addressingModesTable[3].destination[1]=1;	
	addressingModesTable[3].destination[2]=0;
	addressingModesTable[3].destination[3]=1;	
	/*lea*/
	addressingModesTable[4].source[0]=0;
	addressingModesTable[4].source[1]=1;	
	addressingModesTable[4].source[2]=0;
	addressingModesTable[4].source[3]=0;	
	addressingModesTable[4].destination[0]=0;
	addressingModesTable[4].destination[1]=1;	
	addressingModesTable[4].destination[2]=0;
	addressingModesTable[4].destination[3]=1;	
	/*clr*/
	addressingModesTable[5].source[0]=0;
	addressingModesTable[5].source[1]=0;	
	addressingModesTable[5].source[2]=0;
	addressingModesTable[5].source[3]=0;	
	addressingModesTable[5].destination[0]=0;
	addressingModesTable[5].destination[1]=1;	
	addressingModesTable[5].destination[2]=0;
	addressingModesTable[5].destination[3]=1;	
	/*not*/
	addressingModesTable[6].source[0]=0;
	addressingModesTable[6].source[1]=0;	
	addressingModesTable[6].source[2]=0;
	addressingModesTable[6].source[3]=0;	
	addressingModesTable[6].destination[0]=0;
	addressingModesTable[6].destination[1]=1;	
	addressingModesTable[6].destination[2]=0;
	addressingModesTable[6].destination[3]=1;	
	/*inc*/
	addressingModesTable[7].source[0]=0;
	addressingModesTable[7].source[1]=0;	
	addressingModesTable[7].source[2]=0;
	addressingModesTable[7].source[3]=0;	
	addressingModesTable[7].destination[0]=0;
	addressingModesTable[7].destination[1]=1;	
	addressingModesTable[7].destination[2]=0;
	addressingModesTable[7].destination[3]=1;	
	/*dec*/
	addressingModesTable[8].source[0]=0;
	addressingModesTable[8].source[1]=0;	
	addressingModesTable[8].source[2]=0;
	addressingModesTable[8].source[3]=0;	
	addressingModesTable[8].destination[0]=0;
	addressingModesTable[8].destination[1]=1;	
	addressingModesTable[8].destination[2]=0;
	addressingModesTable[8].destination[3]=1;	
	/*jmp*/
	addressingModesTable[9].source[0]=0;
	addressingModesTable[9].source[1]=0;	
	addressingModesTable[9].source[2]=0;
	addressingModesTable[9].source[3]=0;	
	addressingModesTable[9].destination[0]=0;
	addressingModesTable[9].destination[1]=1;	
	addressingModesTable[9].destination[2]=1;
	addressingModesTable[9].destination[3]=0;	
	/*bne*/
	addressingModesTable[10].source[0]=0;
	addressingModesTable[10].source[1]=0;	
	addressingModesTable[10].source[2]=0;
	addressingModesTable[10].source[3]=0;	
	addressingModesTable[10].destination[0]=0;
	addressingModesTable[10].destination[1]=1;	
	addressingModesTable[10].destination[2]=1;
	addressingModesTable[10].destination[3]=0;	
	/*jsr*/
	addressingModesTable[11].source[0]=0;
	addressingModesTable[11].source[1]=0;	
	addressingModesTable[11].source[2]=0;
	addressingModesTable[11].source[3]=0;	
	addressingModesTable[11].destination[0]=0;
	addressingModesTable[11].destination[1]=1;	
	addressingModesTable[11].destination[2]=1;
	addressingModesTable[11].destination[3]=0;	
	/*red*/
	addressingModesTable[12].source[0]=0;
	addressingModesTable[12].source[1]=0;	
	addressingModesTable[12].source[2]=0;
	addressingModesTable[12].source[3]=0;	
	addressingModesTable[12].destination[0]=0;
	addressingModesTable[12].destination[1]=1;	
	addressingModesTable[12].destination[2]=0;
	addressingModesTable[12].destination[3]=1;	
	/*prn*/
	addressingModesTable[13].source[0]=0;
	addressingModesTable[13].source[1]=0;	
	addressingModesTable[13].source[2]=0;
	addressingModesTable[13].source[3]=0;	
	addressingModesTable[13].destination[0]=1;
	addressingModesTable[13].destination[1]=1;	
	addressingModesTable[13].destination[2]=0;
	addressingModesTable[13].destination[3]=1;	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



