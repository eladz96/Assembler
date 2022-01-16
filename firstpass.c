#include "declarations.h"

extern int* commendTable;
extern symbol* symbolTableHead;
extern codeLine* codePictureHead;
extern dataLine* dataPictureHead;
extern addressingModes* addressingModesTable;
extern int numOfOperands[NUM_OF_COMMENDS];

int firstPass(FILE* fp,int* ICp,int* DCp){/*a function that do the first pass on the source code, input: file pointer, IC adress, DC adress, return: number of errors found*/
	char newLine[LINE_LENGTH]="";	/*array for the new line*/
	int lineCounter=0;		/*counts the number of lines*/
	int errorCounter=0;		/*counts the number of errors*/
	int labelFlag=0;		/*a flag that indicates if there was a labl in this line*/
	int index=0;			/*gets the index in which the commend line or data line is starting(the index is 0 if there is no label and its the index of ':' if there is a label)*/
	int commendType=0;		/*the value of this int represents what commend is in this line. 1=.data 2=.string 3=.entry 4=.extern 5=enpty line 6=commend*/
	
	while(!feof(fp)){
		labelFlag=0;		
		fgets(newLine,LINE_LENGTH-1,fp);/*reading a new line*/
		++lineCounter;
		if(checkComment(newLine))/*checking of its a comment line if so move on*/
			continue;
		index=isLabel(newLine,&labelFlag);/*getting the index of the ':' if there is a label and 0 if there isn't*/		
		switch(commendType=whatInLine(newLine+index,lineCounter,labelFlag)){/*checking what commend appear in the line*/
			case 1:/*.data*/
				if(labelFlag){/*if there s a label add it to the symbol table with attribute 'd'*/
					if(!(addToSymbolTable(newLine,index,DCp,lineCounter,'d'))){
						errorCounter++;
						continue;
					}/*end of if*/
				}/*end of if*/	
				if(addDataToTableNum(newLine,DCp,lineCounter))/*add the data to the data table*/
					errorCounter++;								
			break;

			case 2:/*.string*/
				if(labelFlag){/*if there s a label add it to the symbol table with attribute 'd'*/
					if(!(addToSymbolTable(newLine,index,DCp,lineCounter,'d'))){
						errorCounter++;
						continue;
					}/*end of if*/
				}/*end of if*/
				if(addDataToTableStr(newLine,DCp,lineCounter))/*add the data to the data table*/
					errorCounter++;
					
			break;
			
			case 3:/*.entry*/
			break;

			case 4:/*.extern*/
				if(!(addExternSymbol(newLine,lineCounter)))/*add symbol to the table with attricute 'e'*/
					errorCounter++;				
			break;

			case 5:/*empty line*/
			break;

			case 6:/*commend*/
				if(labelFlag){/*if there s a label add it to the symbol table with attribute 'c'*/
					if(!(addToSymbolTable(newLine,index,ICp,lineCounter,'c'))){
						errorCounter++;
						continue;
					}/*end of if*/
				}/*end of if*/	
				if(addCodeToTable(newLine,ICp,lineCounter,index))/*code the commend into the code table*/
					errorCounter++;
			break;
			case 7:/*error*/
				errorCounter++;
			break;
		}/*end of switch*/			
	}/*end of while*/	
		if (errorCounter)/*if errors where found return faliure*/
			return errorCounter;
		else return 0;
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int checkComment(char* newLine){/*check if the line is a comment return 1 if so and 0 if not*/
	int i;
	for (i=0;newLine[i]==' ' || newLine[i]=='\t'; ++i);/*skip white spaces*/
	if (newLine[i]==';')/*if the first char is ';' its a comment*/
		return 1;
	return 0;
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int isLabel(char *newLine,int* labelFlag){/*return 0 if there is no label, return the index of ':' if there is a label*/
	int i;

	for (i=0;i<LINE_LENGTH-1;++i){ /*a loop that check if there is a ':' char in the line, if so there might be a label in it*/
		if (newLine[i]==':'){
			*labelFlag=1;/*change the label flag*/			
			return i;/*return the index of ':'*/
		}else if (newLine[i]=='\0')/*if we reached the end if the string there is no label so return 0*/
				return 0;
	}/*end of for*/	
	return 0;
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int whatInLine(char *newLine,int lineCounter,int labelFlag){ /*1=.data 2=.string 3=.entry 4=.extern 5=enpty line 6=commend 7=error*/
	char *dot;
	
	if(labelFlag)/*if there is a label newLine points to ':' so move on 1 spot*/
		newLine++;
	for(dot=newLine;*dot==' ' || *dot=='\t';++dot);
	if(*dot=='.'){
		if(!(strncmp(dot,".data",5)))
			return 1;		
		else if(!(strncmp(dot,".string",7)))	
			return 2;
		else if(!(strncmp(dot,".entry",6)))
			return 3;
		else if(!(strncmp(dot,".extern",7)))
			return 4;
		else{/*if there is a '.' but its not the above commends its an error*/
			 printf("error illegal commend in line %d\n",lineCounter);
			return 7;	
		}/*end of else*/	
	}else return (checkEmpty(newLine));
	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int checkEmpty(char *newLine){ /*check if the line is empty, if so return 5 if not return 6*/
	int i;

	for (i=0;i<strlen(newLine);++i){
		if(newLine[i]!=' ' && newLine[i]!='\t' && newLine[i]!='\n')
			return 6;
	}/*enf of for*/
	return 5;

}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int addToSymbolTable(char *newLine,int index,int *counter,int lineCounter,char att){/*adds a new symbol to the table, return 0 if there was an error*/
	int i;
	int labelLen;
	int labelStart;
	char label[LABEL_LENGTH]="";

	for(i=0;newLine[i]==' ' || newLine[i]=='\t';++i);/*finding where the label starts*/
	if (!((newLine[i]>='a' && newLine[i]<='z') || (newLine[i]>='A' && newLine[i]<='Z'))){/*checking if the first char is a letter*/
		printf("error illegal label name in line %d\n",lineCounter);		
		return 0;	
	}/*end of if*/
	labelLen=index-i;
	labelStart=i;
	for(;i<index;++i){/*checking if there is an illegal char in the name*/
		if(newLine[i]==' ' || newLine[i]=='\t' || (!((newLine[i]>='a' && newLine[i]<='z') || (newLine[i]>='A' && newLine[i]<='Z') || (newLine[i]>='0' && newLine[i]<='9')))){
			printf("error illegal character in label name in line %d\n",lineCounter);		
			return 0;
		}/*end of if*/
	}/*end of for*/
	if(newLine[index+1]!=' ' && newLine[index+1]!='\t'){/*check fo apace after ':'*/
		printf("error no space after the label name in line %d\n",lineCounter);		
		return 0;
	}/*end of if*/
	if(labelLen>=LABEL_LENGTH){/*check if length is legal*/
		printf("error illegal label length in line %d\n",lineCounter);		
		return 0;
	}/*end of if*/
	strncpy(label,newLine+labelStart,labelLen);
	label[labelLen]=0;	
	if(labelIsSaveWord(label,labelLen)){/*check if label is save word*/
		printf("error illegal label name in line %d\n",lineCounter);		
		return 0;
	}/*end of if*/
	if(!(checkDuplicates(label))){/*check for duplicates*/
		printf("error label in line %d already exist\n",lineCounter);		
		return 0;
	}/*end of if*/	
	addSymbol(label,*counter,att);/*if we are here so the label is legal so add it to the table*/
	return 1;
	
	
	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int labelIsSaveWord(char* label,int labelLen){/*check if the label name is a save word return 0 if so and 1 if not*/
	
	if(labelLen==2 && label[0]=='r' && (label[1]>='0' && label[1]<='7'))
		return 1;
	if(!(strcmp(label,"mov")))
		return 1;
	if(!(strcmp(label,"cmp")))
		return 1;
	if(!(strcmp(label,"add")))
		return 1;
	if(!(strcmp(label,"sub")))
		return 1;
	if(!(strcmp(label,"lea")))
		return 1;
	if(!(strcmp(label,"clr")))
		return 1;
	if(!(strcmp(label,"not")))
		return 1;
	if(!(strcmp(label,"inc")))
		return 1;
	if(!(strcmp(label,"dec")))
		return 1;
	if(!(strcmp(label,"jmp")))
		return 1;
	if(!(strcmp(label,"bne")))
		return 1;
	if(!(strcmp(label,"jsr")))
		return 1;
	if(!(strcmp(label,"red")))
		return 1;
	if(!(strcmp(label,"prn")))
		return 1;
	if(!(strcmp(label,"rts")))
		return 1;
	if(!(strcmp(label,"stop")))
		return 1;
	if(!(strcmp(label,"data")))
		return 1;
	if(!(strcmp(label,"string")))
		return 1;
	if(!(strcmp(label,"entry")))
		return 1;
	if(!(strcmp(label,"extern")))
		return 1;
	return 0;
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int checkDuplicates(char *label){/*checks if the label already exist in the symbol table return 0 if so and 1 if not*/
	symbol *tableScanner; 
		
	tableScanner=symbolTableHead;
	while(tableScanner){
		if (!(strcmp(tableScanner->name,label)))/*if the name is equal return 0*/
			return 0;
		tableScanner=(symbol*)tableScanner->next;/*advancing*/
	}/*end of while*/
	return 1;	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void addSymbol(char *label,int val,char att){/*adds the new symbol the the table*/
	symbol *symbolP, *temp;
	symbolP=symbolTableHead;
	
	if (!(symbolTableHead)){
		if(!(symbolTableHead=(symbol*)malloc(sizeof(symbol)))){
			printf("error malloc faild!");
			exit(0);
		}/*end of if*/
		strcpy(symbolTableHead->name,label);/*adding the data to the struct*/
		symbolTableHead->val=val;
		symbolTableHead->attribute=att;
		symbolTableHead->entryFlag=0;
		symbolTableHead->next=NULL;
	}/*end of if*/
	else{
		while (symbolP->next){/*find the last link in the list*/
			symbolP=(symbol*)symbolP->next;/*advancing*/
		}/*end of while*/
		if(!(temp=(symbol*)malloc(sizeof(symbol)))){
			printf("error malloc faild!");
			exit(0);
		}/*end of if*/
		strcpy(temp->name,label);/*adding the data to the struct*/
		temp->val=val;
		temp->attribute=att;
		temp->entryFlag=0;
		temp->next=NULL;
		symbolP->next=(struct symbol*)temp;/*adding the new struc to the list*/
	}/*end of else*/
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int addDataToTableNum(char* newLine,int* DCp,int lineCounter){/*check if the data commend is lleagl if so add the numbers to the data table, return 0 if no errors return 1 if there were errors*/
	int comaFlag=1;
	char *i;
	int num;
	
	i=strchr(newLine,'.');/*finding '.'*/
	i+=5;/*skipping the word 'data'*/
	for(;(*i)!='\n';i++){
		if (*i==' ' || *i=='\t')/*skip while spaces*/
			continue;
		if(*i==',' && comaFlag){/*if no numbers between comas*/
			printf("error two ',' without a number between them in line %d\n",lineCounter);
			return 1;
		}/*end of if*/
		if(*i==',' && comaFlag==0){/*if we found a coma raise flag*/
			comaFlag=1;
			continue;
		}/*end of if*/
		if(((*i>='0' && *i<='9') || *i=='+' || *i=='-') && !(comaFlag)){/*if no coma between 2 numbers*/
			printf("error two numbers without a ',' between them in line %d\n",lineCounter);
			return 1;
		}/*end of if*/
		if((*i=='+' || *i=='-') && comaFlag && (*(i+1)>='0' && *(i+1)<='9')){/*if number is legal add to the data table*/
			num=atoi(i);			
			addData(DCp, num);
			comaFlag=0;/*take down the flag*/
			i+=(numberOfDigits(num));/*skip the number of digits num has*/
			continue;
		}/*end of if*/
		if((*i=='+' || *i=='-') && (!(*(i+1)>='0' && *(i+1)<='9'))){/*if there is a '+' or '-' but no number after*/
			printf("error illegal expression in line %d\n",lineCounter);
			return 1;			
		}/*end of if*/
		if(comaFlag && (*i>='0' && *i<='9')){/*if number is legal add it to the table*/
			num=atoi(i);			
			addData(DCp, num);
			comaFlag=0;/*take down the flag*/
			i+=numberOfDigits(num)-1;
			continue;
		}/*end of if*/
	}/*end of for*/
	if(comaFlag){/*if there is a ',' but no number after it*/
		printf("error no number after the last ',' in line %d\n",lineCounter);
		return 1;			
	}/*end of if*/
	return 0;
	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void addData(int* DCp, int num){/*add the number to the table*/
	dataLine *dataP, *temp;	
	dataP=dataPictureHead;
		
	if(!(dataPictureHead)){/*if its the first number*/
		if(!(dataPictureHead=(dataLine*)malloc(sizeof(dataLine)))){	
			printf("error malloc faild!");
			exit(0);	
		}/*end of if*/
		dataPictureHead->address=*DCp;/*adding the data to the struct*/
		(*DCp)++;
		dataPictureHead->machineCode=num;
		dataPictureHead->attribute='A';
		dataPictureHead->next=NULL;
	}/*end of if*/
	else{
		while(dataP->next){/*finding the last link in the list*/
			dataP=(dataLine*)dataP->next;/*advancing*/			
		}/*end of while*/
		if(!(temp=(dataLine*)malloc(sizeof(dataLine)))){
			printf("error malloc faild!");
			exit(0);	
		}/*end of if*/
		temp->address=*DCp;/*adding data to the struct*/
		(*DCp)++;
		temp->machineCode=num;
		temp->attribute='A';
		temp->next=NULL;
		dataP->next=(struct dataLine*)temp;/*adding struct to the list*/
	}/*end of else*/
		
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int numberOfDigits(int num){/*return how many digit are in the number*/
	int counter=0;	
	if(num<0)
		num*=(-1);
	while(num){
		num/=10;
		++counter;
	}/*end of while*/
	return counter;
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int addDataToTableStr(char* newLine,int* DCp,int lineCounter){/*check if the string commend is lleagl if so add the charecters to the data table, return 0 if no errors return 1 if there were errors*/
	int quotationMarkFlag=0;/*1 if there is an open '"', 0 if the '"' is close*/
	char* i;
	
	i=strchr(newLine,'.');/*finding '.'*/
	i+=7;/*skipping 'string'*/
	for(;(*i)!='\n';i++){
				
		if(!quotationMarkFlag && *i!=' ' && *i!='\t' && *i!='"'){
			printf("error there is a char before the \" in line %d\n",lineCounter);
			return 1;						
		}/*end of if*/
		if(!quotationMarkFlag && (*i==' ' || *i=='\t'))/*skip white spaces*/
			continue; 
		if(!(quotationMarkFlag) && *i=='"'){
			if(*(i+1)=='"'){/*check for empty string*/
				printf("error empty string in line %d\n",lineCounter);
				return 1;						
			}/*end of if*/			
			quotationMarkFlag=1;/*rise the flag*/
			continue;	
		}/*end of if*/
		if(quotationMarkFlag && *i!='"')
			addData(DCp,*i);/*addig the data*/
		if(quotationMarkFlag && *i=='"'){
			++i;
			quotationMarkFlag=0;/*taking down the flag because the string is done*/
			addData(DCp,0);/*adding the char '\0' to the end of the string*/
			for(;(*i)!='\n';++i){/*searching for charecters after the '"'*/
				if(!(*i==' ' || *i=='\t' || *i=='\n' || *i=='\0')){
					printf("error there is a char after \" in line %d\n",lineCounter);
					return 1;						
				}/*end of if*/
			}/*end of for*/
			return 0;
		}/*end of if*/
	}/*end of fof*/
	if(quotationMarkFlag && *i=='\n'){/*if the '"' didnt closed*/
		printf("error no '\"' in the end of the string in line %d\n",lineCounter);
		return 1;						
	}/*end of if*/
	return 1;
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int addExternSymbol(char* newLine,int lineCounter){/*checks if the label is leagel and add it to the symbo table, reutn 0 if there was an error and 1 if not*/
	char* i;
	char* labelEnd;
	char* labelStart;
	char label[LABEL_LENGTH]="";
	int labelLen;
	i=strchr(newLine,'.');/*finding '.'*/
	i+=7;/*skiping 'extern'*/
	if(*i!=' ' && *i!='\t'){
		printf("error no space to tab between .extern and label name in line %d\n",lineCounter);		
		return 0;
	}/*end of if*/
	for(;*i==' ' || *i=='\t';++i);/*skipping while spaces*/
	if (!((*i>='a' && *i<='z') || (*i>='A' && *i<='Z'))){/*checking if the first char is a letter*/
		printf("error illegal label name in line %d\n",lineCounter);		
		return 0;	
	}/*end of if*/
	labelStart=i;
	for(;*i!=' ' && *i!='\t' && *i!='\0' && *i!='\n' ;++i);/*finding where the label ends*/
	labelEnd=i;
	labelLen=labelEnd-labelStart;
	for(;*i!='\0';++i){
		if(*i!=' ' && *i!='\t' && *i!='\n'){
			printf("error more char after label name in line %d\n",lineCounter);		
			return 0;
		}/*end of if*/		
	}/*end of for*/	
	i=labelStart;
	for(;i<labelEnd;++i){/*checking if there is an illegal char in the name*/
		if(*i==' ' || *i=='\t' || (!((*i>='a' && *i<='z') || (*i>='A' && *i<='Z') || (*i>='0' && *i<='9')))){
			printf("error illegal character '%c' in label name in line %d\n",*i,lineCounter);		
			return 0;
		}/*end of if*/
	}/*end of for*/
		if(labelLen>=LABEL_LENGTH){/*checking if the length is legal*/
		printf("error illegal label length in line %d\n",lineCounter);		
		return 0;
	}/*end of if*/
	strncpy(label,labelStart,labelLen);
	label[labelLen]=0;	
	if(labelIsSaveWord(label,labelLen)){/*check if the label is not a saved word*/
		printf("error illegal label name in line %d\n",lineCounter);		
		return 0;
	}/*end of if*/
	switch(checkDuplicatesExtern(label)){/*check for duplicates, 1=exict with an attribute that is not extern,2=exict with attribute extern,3=doesn't exist*/
		case 1:
			printf("error label in line %d already exist with an attribute other then extern!\n",lineCounter);		
			return 0;
		break;

		case 2:
			return 1;
		break;

		case 3:
			addSymbol(label,0,'e');
			return 1;	
		break;
	}/*end of switch*/
	return 0;	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int checkDuplicatesExtern(char* label){/*checks if the label already exist in the symbol table return 1 if exist with an attribute other tnen extern, 2 if exist with the attribute extern 3 if doesnt exist*/
	symbol* tableScanner; 
		
	tableScanner=symbolTableHead;
	while(tableScanner){
		if (!(strcmp(tableScanner->name,label))){
			if(tableScanner->attribute=='e')
				return 2;/*if attribute is extern*/
			else return 1;/*if attribute is not extern*/
		}/*end of if*/
		tableScanner=(symbol*)tableScanner->next;/*advancing*/
	}/*end of while*/
	return 3; /*if doesn't exist*/	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int addCodeToTable(char* newLine,int* ICp,int lineCounter,int index){/*check if the commend is lleagl if so add the commends to the code table, return 0 if no errors return 1 if there were errors*/
	int commendStart;
	int commendLen;
	char* commend="";		
	int commendIndex;
	int L=1;
	int addressingmode1, addressingmode2;
	int endOfOperand1,endOfOperand2;
	int comaFlag=0;

	if(index){
		if(!(newLine[index+1]==' ' || newLine[index+1]=='\t')){
			printf("error no space between ':' and the commend in line %d\n",lineCounter);		
			return 1;	
		}/*end of if*/
		else{
			index++;
		}/*end of else*/
	}/*end of if*/
	for(;newLine[index]==' ' || newLine[index]=='\t';index++);
	commendStart=index;/*finding the start of the commend*/
	for(;newLine[index]!=' ' && newLine[index]!='\t' && (newLine[index]>='a' && newLine[index]<='z');index++);	
	commendLen=index-commendStart;/*finding the length of the commend*/
	if(!(commend=(char*)malloc(sizeof(char)*(commendLen+1)))){
		fprintf(stderr,"Error malloc faild!\n");
		exit(0);
	}/*end of if*/
	commend[0]=0;
	strncpy(commend,newLine+commendStart,commendLen);
	commend[commendLen]=0;
	commendIndex=searchCommend(commend);/*getting the index of the commend*/
	if(commendIndex==16){/*if the commend doesnt exist*/
		printf("error illegal commend in line %d\n",lineCounter);		
		return 1;	
	}/*end of if*/
	L+=numOfOperands[commendIndex];/*calculating how many code word needed to code this commend*/
	switch(numOfOperands[commendIndex]){
	case 0:	/*0 operands*/
		for(;newLine[index]!='\n';index++){
			if(newLine[index]!=' ' && newLine[index]!='\t'){
				printf("error there is an operand after a commend that doesn't need any in line %d\n",lineCounter);		
				return 1;			
			}/*end of if*/
		}/*end of for*/
		addcode(*ICp,L,commendTable[commendIndex],'A');
		*ICp+=L;
		return 0;
	break;
		
	case 1: /*1 operand*/
		if(newLine[index]!=' ' && newLine[index]!='\t'){
			printf("error no space betwenn commend and operand in line %d \n",lineCounter);		
			return 1;
		}/*end of if*/	
		for(;newLine[index]==' ' || newLine[index]=='\t' || newLine[index]=='\n';++index);/*skipping white spaces*/
		if(newLine[index]=='\0'){/*if no operand*/
			printf("error no operand in line %d \n",lineCounter);		
			return 1;
		}/*end of if*/	
		addressingmode1=checkAddressingMode(newLine,index);/*checking what addressing mode was used*/
		if(checkOperandLeagel(newLine,index,addressingmode1,&endOfOperand1,lineCounter))/*check if the operand is legal*/
			return 1;
		for(;newLine[endOfOperand1]==' ' || newLine[endOfOperand1]=='\t' || newLine[endOfOperand1]=='\n';++endOfOperand1);/*searching for the operand's end*/
		if(newLine[endOfOperand1]!='\0'){
			printf("error there are charecters after the operands in line %d\n",lineCounter);		
			return 1;	
		}/*end of if*/
		addcode(*ICp,L,commendTable[commendIndex]+addressingmode1,'A');/*adding the first word(the word of the commend)*/
		*ICp+=1;
		switch(addressingmode1){
			case 0:/*immediate-add code now*/
				++index;
				addcode(*ICp,0,atoi(newLine+index),'A');			
			break;
			case 3:/*register direct- add code now*/
				++index;
				addcode(*ICp,0,1<<atoi(newLine+index),'A');			
			break;
			case 1:/*direct and relative- add code later in second pass*/
				addcode(*ICp,0,0,'?');
			break;
			case 2:
				addcode(*ICp,0,0,'?');
			break;
		}/*end of switch*/
		*ICp+=1;
		return 0;
	break;
		
	case 2: /*2 operands*/
		if(newLine[index]!=' ' && newLine[index]!='\t'){
			printf("error no space betwenn commend and operand in line %d\n",lineCounter);		
			return 1;	
		}/*end of if*/
		for(;newLine[index]==' ' || newLine[index]=='\t' || newLine[index]=='\n';++index);/*skipping white spaces*/
		if(newLine[index]=='\0'){
			printf("error no operand in line %d \n",lineCounter);		
			return 1;	
		}/*end of if*/
		addressingmode1=checkAddressingMode(newLine,index);/*getting the addressing mode of the first operand*/
		if(checkOperandLeagel(newLine,index,addressingmode1,&endOfOperand1,lineCounter))/*check if the operand is legal*/
			return 1;
		for(;newLine[endOfOperand1]==' ' || newLine[endOfOperand1]=='\t' || newLine[endOfOperand1]==',' ;++endOfOperand1){/*skipping white spaces and searching for the ',' between th eoperands*/
			if(!comaFlag && newLine[endOfOperand1]==',')
				comaFlag=1;
			else if (comaFlag && newLine[endOfOperand1]==','){
				printf("error 2 ',' in line %d\n",lineCounter);		
				return 1;	
			}/*end of else if*/
		}/*end of for*/
		if(newLine[endOfOperand1]=='\n'){/*no second operand*/
			printf("error no operand in line %d \n",lineCounter);		
			return 1;	
		}/*end of if*/
		if(!comaFlag){/*no ',' between operands*/
			printf("error no ',' between operands in line %d\n",lineCounter);		
			return 1;	
		}/*end of if*/
		addressingmode2=checkAddressingMode(newLine,endOfOperand1);/*getting the addressing mode of the second operand*/
		if(checkOperandLeagel(newLine,endOfOperand1,addressingmode2,&endOfOperand2,lineCounter))/*check if the operand is legal*/
			return 1;
		for(;newLine[endOfOperand2]==' ' || newLine[endOfOperand2]=='\t' || newLine[endOfOperand2]=='\n';++endOfOperand2);/*check for cahrs after the second operand*/
		if(newLine[endOfOperand2]!='\0'){
			printf("error there are charecters after the operands in line %d\n",lineCounter);		
			return 1;	
		}/*end of if*/
		addcode(*ICp,L,commendTable[commendIndex]+(addressingmode1<<2)+addressingmode2,'A');/*adding the first word(the commend word)*/
		*ICp+=1;
		switch(addressingmode1){
			case 0:/*immediate-add code now*/
				++index;
				addcode(*ICp,0,atoi(newLine+index),'A');			
			break;
			case 3:/*register direct- add code now*/
				++index;
				addcode(*ICp,0,1<<atoi(newLine+index),'A');			
			break;
			case 1:/*direct and relative- add code later in second pass*/
				addcode(*ICp,0,0,'?');
			break;
			case 2:
				addcode(*ICp,0,0,'?');
			break;
		}/*end of switch*/
		*ICp+=1;
		switch(addressingmode2){
			case 0:/*immediate-add code now*/
				++endOfOperand1;
				addcode(*ICp,0,atoi(newLine+endOfOperand1),'A');			
			break;
			case 3:/*register direct- add code now*/
				++endOfOperand1;
				addcode(*ICp,0,1<<atoi(newLine+endOfOperand1),'A');			
			break;
			case 1:/*direct and relative- add code later in second pass*/
				addcode(*ICp,0,0,'?');
			break;
			case 2:
				addcode(*ICp,0,0,'?');
			break;
		}/*end of switch*/	
		*ICp+=1;
		return 0;
	break;		
	}/*end of switch*/
	return 1;	
}/*end of function*/


/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int searchCommend(char* commend){/*check what commend we rescived return its index in the commend table or 16 if its not a llegal commend*/
		
	if(!(strcmp(commend,"mov")))
		return 0;
	if(!(strcmp(commend,"cmp")))
		return 1;
	if(!(strcmp(commend,"add")))
		return 2;
	if(!(strcmp(commend,"sub")))
		return 3;
	if(!(strcmp(commend,"lea")))
		return 4;
	if(!(strcmp(commend,"clr")))
		return 5;
	if(!(strcmp(commend,"not")))
		return 6;
	if(!(strcmp(commend,"inc")))
		return 7;
	if(!(strcmp(commend,"dec")))
		return 8;
	if(!(strcmp(commend,"jmp")))
		return 9;
	if(!(strcmp(commend,"bne")))
		return 10;
	if(!(strcmp(commend,"jsr")))
		return 11;
	if(!(strcmp(commend,"red")))
		return 12;
	if(!(strcmp(commend,"prn")))
		return 13;
	if(!(strcmp(commend,"rts")))
		return 14;
	if(!(strcmp(commend,"stop")))
		return 15;
	return 16;/*if non of the above so error*/
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void addcode(int address,int L,int machineCode,char attribute){/*add a new code to the table*/
	codeLine *codeP, *temp;
	codeP=codePictureHead;

	if(!(codePictureHead)){/*if it's the first code word*/
		if(!(codePictureHead=(codeLine*)malloc(sizeof(codeLine)))){
			printf("error malloc faild!");
			exit(0);		
		}/*end of if*/
		codePictureHead->address=address;/*adding data to the struct*/
		codePictureHead->L=L;
		codePictureHead->machineCode=machineCode;
		codePictureHead->attribute=attribute;
		codePictureHead->next=NULL;
	}/*end of if*/
	else{
		while(codeP->next){/*go to the last link int the list*/
			codeP=(codeLine*)codeP->next;/*advancing*/
		}/*end of while*/
		if(!(temp=(codeLine*)malloc(sizeof(codeLine)))){
			printf("error malloc faild!");
			exit(0);	
		}/*end of if*/
		temp->address=address;/*add data to the struct*/
		temp->L=L;
		temp->machineCode=machineCode;
		temp->attribute=attribute;
		temp->next=NULL;
		codeP->next=(struct codeLine*)temp;/*add new struct to the list*/
	}/*end of else*/
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int checkAddressingMode(char* newLine,int index){/*check what addressing mode the operator use and returns its number 0,1,2,3*/
	
	if(newLine[index]=='#')
		return 0;
	else if(newLine[index]=='%')
		return 2;
	else if ((strlen(newLine)-index)>=2 && newLine[index]=='r' && newLine[index+1]>='0' && newLine[index+1]<='7' && (newLine[index+2]==' ' || newLine[index+2]=='\t' || newLine[index+2]==',' || newLine[index+2]=='\n'))
		return 3;
	else 
		return 1;
	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int checkOperandLeagel(char *newLine,int index,int addressingmode,int *endOfOperand, int lineCounter){/*check if the operand is legal return 0 if so and 1 if not*/		
	int labelLen=index*(-1);
	int labelStart=index;
	char label[LABEL_LENGTH]="";


	switch(addressingmode){
		case 0: /*immediate '#'*/
			++index;/*skip '#'*/
			if(newLine[index]!='+' && newLine[index]!='-' && (!(newLine[index]>='0' && newLine[index]<='9'))){/*if first char is not '+' '-' or a number*/
				printf("error illegal char after '#' in line %d\n",lineCounter);		
				return 1;	
			}/*end of if*/
			if(newLine[index]=='+' || newLine[index]=='-'){/*number with '+' or '-'*/
				if(newLine[index+1]>='0' && newLine[index+1]<='9')	
					++index;
				else{
					printf("error illegal operand in line %d\n",lineCounter);		
					return 1;	
				} /*end of else*/
			}/*end of if*/
			for(;newLine[index]!=',' && newLine[index]!=' ' && newLine[index]!='\t' && newLine[index]!='\n';++index){/*searching of illegal chars in the number*/
				if(!(newLine[index]>='0' && newLine[index]<='9')){		
					printf("error illegal operand in line %d\n",lineCounter);		
					return 1;	
				}/*end of if*/
			}/*end of for*/
			*endOfOperand=index;/*updating end of operand*/
			return 0;
		break;
		case 1: /*direct 'label'*/
			if (!((newLine[index]>='a' && newLine[index]<='z') || (newLine[index]>='A' && newLine[index]<='Z'))){/*checking if the first char is a letter*/
				printf("error illegal label name in line %d\n",lineCounter);		
				return 1;	
			}/*end of if*/		
			for(;newLine[index]!=' ' && newLine[index]!='\t' && newLine[index]!=',' && newLine[index]!='\n';++index){/*checking if there is an illegal char in the name*/
				if(!((newLine[index]>='a' && newLine[index]<='z') || (newLine[index]>='A' && newLine[index]<='Z') || (newLine[index]>='0' && newLine[index]<='9'))){
					printf("error illegal character in label name in line %d\n",lineCounter);		
					return 1;
				}/*end of if*/
			}/*end of for*/
			labelLen+=index;
			if(labelLen>=LABEL_LENGTH){/*check if length legal*/
				printf("error illegal label length in line %d\n",lineCounter);		
				return 1;
			}/*end of if*/
			strncpy(label,newLine+labelStart,labelLen);
			label[labelLen]=0;
			if(labelIsSaveWord(label,labelLen)){/*check if saved word*/
				printf("error illegal label name in line %d\n",lineCounter);		
				return 1;
			}/*end of if*/
			*endOfOperand=index;/*update end of operand*/
			return 0;
		break;
		case 2:/*relative %*/
			++index;/*skip '%'*/
			if (!((newLine[index]>='a' && newLine[index]<='z') || (newLine[index]>='A' && newLine[index]<='Z'))){/*checking if the first char is a letter*/
				printf("error illegal label name in line %d\n",lineCounter);		
				return 1;	
			}/*end of if*/		
			for(;newLine[index]!=' ' && newLine[index]!='\t' && newLine[index]!=',' && newLine[index]!='\n';++index){/*checking if there is an illegal char in the name*/
				if(!((newLine[index]>='a' && newLine[index]<='z') || (newLine[index]>='A' && newLine[index]<='Z') || (newLine[index]>='0' && newLine[index]<='9'))){
					printf("error illegal character in label name in line %d\n",lineCounter);		
					return 1;
				}/*end of if*/
			}/*end of for*/
			labelLen+=index;
			if(labelLen>=LABEL_LENGTH){/*check if length legal*/
				printf("error illegal label length in line %d\n",lineCounter);		
				return 1;
			}/*end of if*/
			strncpy(label,newLine+labelStart,labelLen);
			label[labelLen]=0;	
			if(labelIsSaveWord(label,labelLen)){/*check if saved word*/
				printf("error illegal label name in line %d\n",lineCounter);		
				return 1;
			}/*end of if*/
			*endOfOperand=index;/*update the end of operand*/
			return 0;
		break;
		case 3:/*register 'r_'*/
			if(newLine[index]=='r' && newLine[index+1]>='0' && newLine[index+1]<='7' && (newLine[index+2]==' ' || newLine[index+2]==',' || newLine[index+2]=='\t' || newLine[index+2]=='\n')){
				*endOfOperand=index+2;
				return 0;
			}/*end of if*/
			else {
				printf("error illegal character in label name in line %d\n",lineCounter);		
				return 1;
			}/*end of else*/
		break;
	}/*end of switch*/
	return 1;	
	
	
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void updateSymbolTable(int ICF){/*update the values of the symbols in the table*/
	symbol* tempP=symbolTableHead;
	while(tempP){
		if(tempP->attribute=='d')
			tempP->val+=ICF;/*if the attribute is 'd' add ICF to value*/
		tempP=(symbol*)tempP->next;/*advancing*/
	}/*end of while*/
}/*end of function*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/














