#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct { char first[10]; char second[10]; uint8_t n; } Inst_t;
typedef struct { char name[10]; uint8_t code; } Funct_t;
typedef struct { char name[10]; uint8_t address; } Label_t;
typedef struct { Label_t labels[20]; uint8_t n; } ListLabel_t;
int isNum(char str[]);
void splitLine(char line[],Inst_t* inst);
void printInst(Inst_t* inst);
uint8_t getFunctByName(char name[]);
void setLabel(ListLabel_t* lbs,uint8_t add,char name[]);
uint8_t getLabel(ListLabel_t* lbs,char name[]);
void printLabels(ListLabel_t* lbs);

Funct_t functions[58] = {
    {"nop"   ,0x00},{"loadA" ,0x01},{"loadB" ,0x02},{"storeA",0x03},{"storeB",0x04},{"addAB" ,0x05},{"subAB" ,0x06},{"subBA" ,0x07},{"mulAB" ,0x08},{"divAB" ,0x09},{"divBA" ,0x0A},{"notA"  ,0x0B},{"notB"  ,0x0C},{"andAB" ,0x0D},{"orAB"  ,0x0E},{"xorAB" ,0x0F},{"shl"   ,0x10},{"shr"   ,0x11},{"rol"   ,0x12},{"ror"   ,0x13},{"pushA" ,0x14},{"pushB" ,0x15},{"popA"  ,0x16},{"popB"  ,0x17},{"call"  ,0x18},{"ret"   ,0x19},{"jmp"   ,0x20},{"out"   ,0xFE},{"hlt"   ,0xFF},
    // Capitle
    {"NOP"   ,0x00},{"LOADA" ,0x01},{"LOADB" ,0x02},{"STOREA",0x03},{"STOREB",0x04},{"ADDAB" ,0x05},{"SUBAB" ,0x06},{"SUBBA" ,0x07},{"MULAB" ,0x08},{"DIVAB" ,0x09},{"DIVBA" ,0x0A},{"NOTA"  ,0x0B},{"NOTB"  ,0x0C},{"ANDAB" ,0x0D},{"ORAB"  ,0x0E},{"XORAB" ,0x0F},{"SHL"   ,0x10},{"SHR"   ,0x11},{"ROL"   ,0x12},{"ROR"   ,0x13},{"PUSHA" ,0x14},{"PUSHB" ,0x15},{"POPA"  ,0x16},{"POPB"  ,0x17},{"CALL"  ,0x18},{"RET"   ,0x19},{"JMP"   ,0x20},{"OUT"   ,0xFE},{"HLT"   ,0xFF}
};

void Read(ListLabel_t* labels,char filename[]);




void Assembler(char filename[],char fileoutput[]){
    ListLabel_t labels;
    labels.n=0;
    Read(&labels,filename);
    FILE* f_input = fopen(filename,"r");
    FILE* f_output = fopen(fileoutput,"wb");
    if(!f_input||!f_output){printf("File '%s' not exist\n",filename);exit(1);}
    char line[20];
    int lines = 0;
    printf("-------------------------------\nCode.bin\n");
    while(fgets(line,20,f_input)){
        lines++;
        line[strlen(line)-1]='\0';
        if(!strcmp(line,"Global start")){
            printf("%02x %02x\n",getFunctByName("jmp"),getLabel(&labels,"start"));
            uint16_t code = getFunctByName("jmp")&0xFF;
            code |= getLabel(&labels,"start")<<8;
            fwrite(&code,sizeof(uint16_t),1,f_output);
            continue;
        }
        else if(line[0]=='\n'||line[0]=='\0'){continue;}
        else if(line[0]==':'){continue;}
        else if(isNum(line)){
            printf("%04x\n",atoi(line));
            uint16_t code = atoi(line);
            fwrite(&code,sizeof(uint16_t),1,f_output);
            continue;}
        else{
            Inst_t inst;
            splitLine(line,&inst);
            if(inst.n==2){
                printf("%02x %02x\n",getFunctByName(inst.first),getLabel(&labels,inst.second));
                uint16_t code = getFunctByName(inst.first)&0xFF;
                code |= getLabel(&labels,inst.second)<<8;
                fwrite(&code,sizeof(uint16_t),1,f_output);
            }else{
                printf("%02x\n",getFunctByName(inst.first));
                uint16_t code = getFunctByName(inst.first)&0xFF;
                fwrite(&code,sizeof(uint16_t),1,f_output);
            }
        }
        continue;
    }
    fclose(f_input);
    fclose(f_output);
    printf("-------------------------------\n%d lines\n",lines);
}

int main(int argc,char* argv[]){
    if(argc>1) Assembler(argv[1],argv[2]);
    else Assembler(argv[1],"./build/code.bin");
    return 0;
}
int isNum(char str[]){
    if(str[0]=='0'||str[0]=='1'||str[0]=='2'||str[0]=='3'||str[0]=='4'||str[0]=='5'||str[0]=='6'||str[0]=='7'||str[0]=='8'||str[0]=='9') return 1;
    else return 0;
}
void splitLine(char line[],Inst_t* inst){
    int len = strlen(line);
    int i = 0, next = 0, f = 0, s = 0;
    while(i<len && f<len && s<len){
        if(line[i]==' ') {next=1;i++;continue;}
        if(!next) {
            inst->first[f++] = line[i++];
            continue;
        }
        if(next) {
            inst->second[s++] = line[i++];
        }       
    }
    inst->first[f]='\0';
    inst->second[s]='\0';
    inst->n = next+1;
}
void printInst(Inst_t* inst){
    if(inst->n-1) printf("[0]=>%s\t[1]=>%s\n",inst->first,inst->second);
    else printf("[0]=>%s\n",inst->first);
}
uint8_t getFunctByName(char name[]){
    for(int i=0;i<58;i++){
        if(!strcmp(name,functions[i].name)){
            return functions[i].code;
        }
    }
    return 0x00;
}
void setLabel(ListLabel_t* lbs,uint8_t add,char name[]){
    lbs->labels[lbs->n].address = add;
    strcpy(lbs->labels[lbs->n].name,name);
    lbs->n++;
}
uint8_t getLabel(ListLabel_t* lbs,char name[]){
    for(int i=0;i<lbs->n;i++){
        if(!strcmp(lbs->labels[i].name,name)){
            return lbs->labels[i].address;
        }
    }
    return 0x00;
}
void printLabels(ListLabel_t* lbs){
    printf("-------------------------------\nLabels:\n");
    for(Label_t* p=lbs->labels;p<lbs->labels+lbs->n;p++){
        printf("\t%s => 0x%02x\n",p->name,p->address);
    }
}

void Read(ListLabel_t* labels,char filename[]){
    FILE* f_input = fopen(filename,"r");
    if(!f_input){printf("File '%s' not exist\n",filename);exit(1);}
    char line[20];
    int n = 0;
    while(fgets(line,20,f_input)){
        line[strlen(line)-1]='\0';
        if(!strcmp(line,"Global start\n")){n++;continue;}
        if(line[0]=='\n'||line[0]=='\0'){continue;}
        if(line[0]==':') {setLabel(labels,n,line+1);continue;}
        n++;continue;
    }
    fclose(f_input);
    printLabels(labels);
}
