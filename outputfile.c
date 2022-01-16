#include "declarations.h"

extern int* commendTable;
extern symbol* symbolTableHead;
extern codeLine* codePictureHead;
extern dataLine* dataPictureHead;
extern addressingModes* addressingModesTable;
extern ex_symbol* exSymbolsHead;
extern int numOfOperands[NUM_OF_COMMENDS];


/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void createOutputFile(char *fileName,int ICF,int DCF){/*creates the output files*/
	FILE *fp;
	codeLine *cTemp=codePictureHead;
	dataLine *dTemp=dataPictureHead;
	symbol *sTemp=symbolTableHead;
	ex_symbol *xTemp=exSymbolsHead;
	
	fileName[strlen(fileName)-3]=0;
	if(!(fp=fopen(strcat(fileName,".ob"),"w"))){/*cheking if file opening was successfull*/
			fprintf(stderr,"Cannot open file\n");	
			exit(0);			
		}/*end of if*/	
	fprintf(fp,"%d %d\n",ICF-100,DCF);/*printing the number of commends words and the number of data words int the output*/
	while(cTemp){/*printing the code words*/
		fprintf(fp,"%04d ",cTemp->address);/*printing the address of the code word*/
		printfHex(cTemp->machineCode,fp);/*printing the code word as hexdecimal number*/
		fprintf(fp,"%c\n",cTemp->attribute);/*printing the attribute of the commend*/
		cTemp=(codeLine*)cTemp->next;/*advancing*/
	}/*end of while*/
	while(dTemp){
		fprintf(fp,"%04d ",dTemp->address+ICF);/*printing the address of the data word*/
		printfHex(dTemp->machineCode,fp);/*printing the code of the datat wornd as a hexdecimal number*/
		fprintf(fp,"%c\n",dTemp->attribute);/*printing the attribute of the data*/
		dTemp=(dataLine*)dTemp->next;/*advancing*/
	}/*end of while*/
	fclose(fp);/*closing the file*/
	if(!(isThereEntry())){/*checking if there were any entrys in the code*/
		fileName[strlen(fileName)-3]=0;
		if(!(fp=fopen(strcat(fileName,".ent"),"w")))/*creating a file and cheking if the opening was succesfull*/
			{
				fprintf(stderr,"Cannot open file\n");	
				exit(0);			
			}/*end of if*/	
		while(sTemp){/*printing the data to entry file*/	
			if(sTemp->entryFlag){/*if the entry flag is up print to file*/
				fprintf(fp,"%s %04d\n",sTemp->name,sTemp->val);
			}/*end of if*/
			sTemp=(symbol*)sTemp->next;/*advancing*/			
		}/*end of while*/
	}/*end of if*/
	fclose(fp);/*colse file*/
	if(xTemp){/*cheking if there were externs in the code*/
		fileName[strlen(fileName)-4]=0;
		if(!(fp=fopen(strcat(fileName,".ext"),"w")))/*creating a file and cheking if the opening was succesfull*/
			{
				fprintf(stderr,"Cannot open file\n");	
				exit(0);			
			}/*end of if*/	
		while(xTemp){/*printing the data to extern file*/
			fprintf(fp,"%s %04d\n",xTemp->name,xTemp->address);
			xTemp=(ex_symbol*)xTemp->next;/*advancing*/
		}/*end of while*/
	}/*end of if*/
	fclose(fp);/*closing the file*/
	
}
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int isThereEntry(){/*check if there is a symbol that was declared as an entry, return 0 if there is, 1 if not*/
	symbol *temp=symbolTableHead;
	
	while(temp){
		if(temp->entryFlag)
			return 0;/*if we found a symbol that its entry flag is up return 0 as sign of success*/
		temp=(symbol*)temp->next;/*advancing*/
	}/*end of while*/
	return 1;/*if we didnt find a symbol with entry flag up return 1 as failure*/
}
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void printfHex(int num, FILE* fp){/*print the machine code in hexdecimal*/
	char hexNum[3]={'0','0','0'};
	int temp=0;
	int i=2;
	
	if(num<0){
			num+=4096;
	}
	while(num){
		temp=num%16;
		if(temp<10){/*if the remnant is smaller the 10 its a "normal" digit*/ 
			hexNum[i]=temp+'0'; 
			--i; 
		}/*end of if*/
		else{ /*the remnant is a letter*/
			hexNum[i]=temp-10+'A'; 
			--i; 
		}/*end of else*/
		num=num/16; 
	}/*end of while*/
	fprintf(fp,"%c%c%c ",hexNum[0],hexNum[1],hexNum[2]);/*printing the hex number*/

}
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



