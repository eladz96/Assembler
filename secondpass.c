#include "declarations.h" 

extern int* commendTable;
extern symbol* symbolTableHead;
extern codeLine* codePictureHead;
extern dataLine* dataPictureHead;
extern addressingModes* addressingModesTable;
extern ex_symbol* exSymbolsHead;
extern int numOfOperands[NUM_OF_COMMENDS];

int secondPass(FILE* fp){/*a function that do the second pass on the source code, input: file pointer, return: number of errors found*/
	char newLine[LINE_LENGTH]="";/*the newly read line as a string*/
	int lineCounter=0;	/*counts the number of lines*/
	int errorCounter=0;	/*counts the number of errors*/
	int labelFlag=0;	/*a flag that indicates if there was a labl in this line*/
	int index=0;		/*gets the index in which the commend line or data line is starting(the index is 0 if there is no label and its the index of ':' if there is a label)*/
	int commendType=0;	/*the value of this int represents what commend is in this line. 1=.data 2=.string 3=.entry 4=.extern 5=enpty line 6=commend*/
	codeLine* codeP=codePictureHead;/*pointer for the code table*/

	while(!feof(fp)){
		labelFlag=0;		
		fgets(newLine,LINE_LENGTH-1,fp);/*getting the new line*/
		++lineCounter;
		if(checkComment(newLine))/*check if there is a comment*/
			continue;
		index=isLabel(newLine,&labelFlag);/*checking if there is a label if so return the index of ':' if not return 0*/		
		switch(commendType=whatInLine(newLine+index,lineCounter,labelFlag)){/*figuring what type of commend there s in the line*/
			case 1:/*.data*/
				continue;
			break;

			case 2:/*.string*/
				continue;
			break;
			
			case 3:/*.entry*/
				index=strchr(newLine,'.')-newLine+6;
				if(updateEntryLabel(newLine,index,lineCounter)){/*update the entry flag of the symbol*/
					++errorCounter;/*if the symbol is absent count the error*/
					continue;
				}/*end of if*/
			break;

			case 4:/*.extern*/
				continue;
			break;

			case 5:/*empty line*/
				continue;
			break;

			case 6:/*commend*/
				if(addDataWordCode(newLine,&codeP,lineCounter,index,labelFlag)){/*finish coding this commend*/
					++errorCounter;/*if there were an error count it*/
					continue;
				}/*end of if*/
			break;
		}/*end of switch*/			
	}/*end of while*/	
		if (errorCounter)
			return errorCounter;
		else return 0;
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int updateEntryLabel(char *newLine, int index, int lineCounter){/*update the entry flag of the symbol return 0 if successful 1 if there were errors*/
	int i;
	int labelLen;
	int labelStart;
	char label[LABEL_LENGTH]="";
	
	if(newLine[index]!=' ' && newLine[index]!='\t'){/*checking if there is a space between the .entry and the label*/
		printf("error no space after \".entry\" commend in line %d\n",lineCounter);		
		return 1;
	}/*end of if*/
	for(i=index;newLine[i]==' ' || newLine[i]=='\t';++i);/*finding where the label starts*/
	if (!((newLine[i]>='a' && newLine[i]<='z') || (newLine[i]>='A' && newLine[i]<='Z'))){/*checking if the first char is a letter*/
		printf("error illegal label name in line %d\n",lineCounter);		
		return 1;	
	}/*end of if*/
	labelStart=i;
	for(;newLine[i]!=' ' && newLine[i]!='\t' && newLine[i]!='\0' && newLine[i]!='\n';++i);/*finding the end of the label*/
	labelLen=i-labelStart;
	for(i=labelStart;i-labelStart<labelLen;++i){/*checking fo illegal characters in the label*/
		if(!((newLine[i]>='a' && newLine[i]<='z') || (newLine[i]>='A' && newLine[i]<='Z') || (newLine[i]>='0' && newLine[i]<='9'))){
			printf("error illegal character in label name in line %d\n",lineCounter);		
			return 1;
		}/*end of if*/
	}/*end of for*/
	for(;newLine[i]==' ' || newLine[i]=='\t' || newLine[i]=='\n';++i);/*skipping the white spaces at the end of the string*/
	if(newLine[i]!='\0'){/*if there is anything after the label report error*/
		printf("error there are charecters after the label in line %d\n",lineCounter);		
		return 1;
	}/*end of if*/	
	if(labelLen>=LABEL_LENGTH){/*if label is to long report the error*/
		printf("error illegal label length in line %d\n",lineCounter);		
		return 1;
	}/*end of if*/
	strncpy(label,newLine+labelStart,labelLen);	
	label[labelLen]=0;
	if(labelIsSaveWord(label,labelLen)){/*check if label is a saved word*/
		printf("error illegal label name in line %d\n",lineCounter);		
		return 1;
	}/*end of if*/
	if(searchAndUpdateLabel(label, lineCounter))
		return 1;
	return 0;
	
}/*end of function*/	
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int searchAndUpdateLabel(char* label, int lineCounter){/*search the label in the symbol table and update the entry flag, return 0 if successful 1 if not*/
	symbol* temp=symbolTableHead;
	
	while(temp){/*searching for the label in the symbol table*/
		if(!(strcmp(label,temp->name))){/*if found turn entry flag up*/
			temp->entryFlag=1;
			return 0;
		}/*end of if*/
		temp=(symbol*)temp->next;/*advancing*/
	}/*end of while*/
	printf("error the label \"%s\" in line %d, is not in the symbol table \n", label ,lineCounter);/*if we didnt found the label report an error*/
	return 1;
}/*end of function*/		
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int addDataWordCode(char* newLine,codeLine** codeP,int lineCounter,int index,int labelFlag){ /*finish coding the machine code of the commend. return 0 if successfull 1 if not*/
	int commendStart;
	int commendLen;
	char* commend;		
	int commendIndex;
	int addressingmode1, addressingmode2;
	int endOfOperand1,endOfOperand2;
	int operandVal;
	
	if(labelFlag)
		++index;
	for(;newLine[index]==' ' || newLine[index]=='\t';index++);/*skipping the white spaces*/
	commendStart=index;
	for(;newLine[index]!=' ' && newLine[index]!='\t' && (newLine[index]>='a' && newLine[index]<='z');index++);/*finding the end of the commend*/
	commendLen=index-commendStart;
	if(!(commend=(char*)malloc(sizeof(char)*(commendLen+1)))){/*creating an array for the commend and checking if malloc was successfull*/
		fprintf(stderr,"Error malloc faild!\n");
		exit(0);
	}/*end of if*/
	commend[0]=0;
	strncpy(commend,newLine+commendStart,commendLen);
	commend[commendLen]=0;
	commendIndex=searchCommend(commend);/*getting the commend index*/
	switch(numOfOperands[commendIndex]){
		case 0:	/*if 0 operands then the commend is fully coded so move to next one*/
			*codeP=(codeLine*)(*codeP)->next;
			free(commend);			
			return 0;
		break;
		
		case 1:/*if 1 operand, check if it was fully coded if not finish coding it*/
			*codeP=(codeLine*)(*codeP)->next;
			for(;newLine[index]==' ' || newLine[index]=='\t';++index);/*skip white spaces*/
			addressingmode1=checkAddressingMode(newLine,index);/*get addressing mode*/
			switch(addressingmode1){
				case 0:/*if addressing mode is 0 or 3 then its fully coded so move to next*/
				break;
				case 3:		
				break;
				case 1:
					for(endOfOperand1=index;newLine[endOfOperand1]!=' ' && newLine[endOfOperand1]!='\t' && newLine[endOfOperand1]!='\n';++endOfOperand1);/*find the end of the operand*/
					operandVal=getOperandVal(newLine,index,endOfOperand1);
					if(operandVal==(-1)){/*if the operand is not in the table*/
						printf("error operand in line %d does not exist in the symbols table\n",lineCounter);
						free(commend);		
						return 1;	
					}/*end of if*/
					if(!operandVal){/*if operand is external*/
						(*codeP)->attribute='E';
						addEx_symbol(newLine,index,endOfOperand1,(*codeP)->address);/*add it to the external symbol table*/
					}/*end of if*/
					else{
						(*codeP)->attribute='R';
						(*codeP)->machineCode=operandVal;
					}/*end of else*/
				break;
				case 2:
					++index;/*skip the '%'*/
					for(endOfOperand1=index;newLine[endOfOperand1]!=' ' && newLine[endOfOperand1]!='\t' && newLine[endOfOperand1]!='\n';++endOfOperand1);/*find the end of the operand*/
					operandVal=getOperandVal(newLine,index,endOfOperand1);
					if(operandVal==(-1)){/*if the operand is not in the table*/
						printf("error operand in line %d does not exist in the symbols table\n",lineCounter);
						free(commend);		
						return 1;	
					}/*end of if*/
					if(!operandVal){/*if symbol is external its illegal to use this addressing mode*/
						printf("error illegal use of '%c' in the commend in line %d \n",'%',lineCounter);		
						free(commend);
						return 1;	
					}/*end of if*/
					(*codeP)->attribute='A';
					(*codeP)->machineCode=operandVal-((*codeP)->address);/*claculate the distance and add it as the machine code*/			
				break;
			}/*end of switch*/
			(*codeP)=(codeLine*)(*codeP)->next;
			free(commend);
			return 0;
		break;
		
		case 2:/*if 2 operands, check if they are fully coded if not finish coding them*/
			*codeP=(codeLine*)(*codeP)->next;
			for(;newLine[index]==' ' || newLine[index]=='\t';++index);/*skip white spaces*/
			addressingmode1=checkAddressingMode(newLine,index);
			switch(addressingmode1){
				case 0:/*if addressing mode is 0 or 3 then its fully coded so move to next*/
				break;
				case 3:		
				break;
				case 1:
					for(endOfOperand1=index;newLine[endOfOperand1]!=' ' && newLine[endOfOperand1]!='\t' && newLine[endOfOperand1]!=',';++endOfOperand1);/*find the end of the operand*/
					operandVal=getOperandVal(newLine,index,endOfOperand1);
					if(operandVal==(-1)){/*if the operand is not in the table*/
						printf("error operand in line %d does not exist in the symbols table\n",lineCounter);		
						free(commend);		
						return 1;	
					}/*end of if*/
					if(!operandVal){/*if operand is external*/
						(*codeP)->attribute='E';
						addEx_symbol(newLine,index,endOfOperand1,(*codeP)->address);/*add it to the external symbol table*/	
					}/*end of if*/
					else{
						(*codeP)->attribute='R';
						(*codeP)->machineCode=operandVal;
					}/*end of else*/
				break;
				case 2:
					++index;/*skip the '%'*/
					for(endOfOperand1=index;newLine[endOfOperand1]!=' ' && newLine[endOfOperand1]!='\t' && newLine[endOfOperand1]!='\0';++endOfOperand1);/*find the end of the operand*/
					operandVal=getOperandVal(newLine,index,endOfOperand1);
					if(operandVal==(-1)){/*if the operand is not in the table*/
						printf("error operand in line %d does not exist in the symbols table\n",lineCounter);		
						free(commend);		
						return 1;	
					}/*end of if*/
					if(!operandVal){/*if symbol is external its illegal to use this addressing mode*/
						printf("error illegal use of '%c' in the commend in line %d \n",'%',lineCounter);		
						free(commend);		
						return 1;	
					}/*end of if*/
					(*codeP)->attribute='A';
					(*codeP)->machineCode=operandVal-((*codeP)->address);			
				break;
			}/*end of switch*/
			(*codeP)=(codeLine*)(*codeP)->next;/*advancing*/
			for(;newLine[index]!=',';++index);/*searching for the ','*/
			++index;
			for(;newLine[index]==' ' || newLine[index]=='\t';++index);/*searching for the beginning of the second operand*/
			addressingmode2=checkAddressingMode(newLine,index);
			switch(addressingmode2){
				case 0:/*if addressing mode is 0 or 3 then its fully coded so move to next*/
				break;
				case 3:		
				break;
				case 1:
					for(endOfOperand2=index;newLine[endOfOperand2]!=' ' && newLine[endOfOperand2]!='\t' && newLine[endOfOperand2]!='\0' && newLine[endOfOperand2]!='\n';++endOfOperand2);/*find the end of the operand*/
					operandVal=getOperandVal(newLine,index,endOfOperand2);
					if(operandVal==(-1)){/*if the operand is not in the table*/
						printf("error operand in line %d does not exist in the symbols table\n",lineCounter);		
						free(commend);		
						return 1;	
					}/*end of if*/
					if(!operandVal){/*if operand is external*/
						(*codeP)->attribute='E';
						addEx_symbol(newLine,index,endOfOperand2,(*codeP)->address);/*add it to the external symbol table*/	
					}/*end of if*/
					else{
						(*codeP)->attribute='R';
						(*codeP)->machineCode=operandVal;
					}/*end of else*/
				break;
				case 2:
					++index;/*skip the '%'*/
					for(endOfOperand2=index;newLine[endOfOperand2]!=' ' && newLine[endOfOperand2]!='\t' && newLine[endOfOperand2]!='\0';++endOfOperand2);/*find the end of the operand*/
					operandVal=getOperandVal(newLine,index,endOfOperand2);
					if(operandVal==(-1)){/*if the operand is not in the table*/
						printf("error operand in line %d does not exist in the symbols table\n",lineCounter);		
						free(commend);		
						return 1;	
					}/*end of if*/
					if(!operandVal){/*if symbol is external its illegal to use this addressing mode*/
						printf("error illegal use of '%c' in the commend in line %d \n",'%',lineCounter);		
						free(commend);		
						return 1;	
					}/*end of if*/
					(*codeP)->attribute='A';
					(*codeP)->machineCode=operandVal-((*codeP)->address);
				break;
			}/*end of switch*/
			(*codeP)=(codeLine*)(*codeP)->next;/*advancing*/
			free(commend);
			return 0;
		}/*end of switch*/
	return 1;	
}/*end of function*/ 

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int getOperandVal(char* newLine,int index,int endOfOperand){/*find the symbol in the symbols table and returns its val, return -1 if the symbol does not exist int the table*/

	symbol* symbolP=symbolTableHead;
	char operand[LABEL_LENGTH]="";

	strncpy(operand,(newLine+index),endOfOperand-index);
	operand[endOfOperand-index]=0;
	while(symbolP){/*searching for the operand in the symbol table*/
		if(!(strcmp(symbolP->name,operand)))
			return	symbolP->val;
		symbolP=(symbol*)symbolP->next;		
	}
	return (-1);
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void addEx_symbol(char *newLine,int index,int endOfOperand,int address){/*add the operand to the table of operands that use external labels*/	
	ex_symbol *exP=exSymbolsHead,*temp;
	
	if(!(exSymbolsHead)){
		if(!(exSymbolsHead=(ex_symbol*)malloc(sizeof(ex_symbol)))){
			printf("error malloc faild!");
			exit(0);
		}/*end of if*/
		strncpy(exSymbolsHead->name,(newLine+index),(endOfOperand-index));
		exSymbolsHead->name[endOfOperand-index]=0;
		exSymbolsHead->address=address;
		exSymbolsHead->next=NULL;
		return;
	}/*end of if*/
	else{
		while(exP->next)
			exP=(ex_symbol*)exP->next;
		if(!(temp=(ex_symbol*)malloc(sizeof(ex_symbol)))){
			printf("error malloc faild!");
			exit(0);
		}
		strncpy(temp->name,(newLine+index),(endOfOperand-index));
		temp->name[endOfOperand-index]=0;
		temp->address=address;
		temp->next=NULL;
		exP->next=(struct ex_symbol*)temp;
		return;		
	}/*end of else*/
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/







