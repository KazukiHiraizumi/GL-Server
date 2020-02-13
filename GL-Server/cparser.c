#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <ctype.h>
#include "cparser.h"

#pragma warning 

static char *cparsed[16];
static char buffer[20000];
static char *s1;
static int cn=0,pn=0;
static char *eos;
static int go=0;

void cparser_set(char *f){
  strcpy(buffer,f);
	s1=buffer;
  eos=buffer+strlen(buffer);
	cn=pn=0;
}
char **cparser_next(void){
  for(;;){
    int a=*s1;
    if(go){
      cparsed[0]="G";
      cn=1;
      go=0;
      break;
    }
    else if(s1>=eos) return NULL;
    else if(iscntrl(a)){
      if(cn>0){
        *s1=0;
        go=1;
        s1++;
        break;
      }
      else s1++;
    }
    else if(a==';'){
			if(cn>0){
				*s1=0;
        s1++;
				break;
			}
		}
		else if(a==' '){
			if(pn==cn) continue;
			else{
				*s1=0;
				s1++;
				pn=cn;
			}
		}
		else{
			if(pn==cn) cparsed[cn++]=s1;
			s1++;
		}
	}
  fprintf(stderr,"parsed ");
  for(int i=0;i<cn;i++){
    fprintf(stderr,"%s ",cparsed[i]);
  }
  fprintf(stderr,"end\n");
	for(int i=cn;i<16;i++){
		cparsed[i]=NULL;
	}
  cn=pn=0;
	return cparsed;
}
/*
int main(int argc,char **argv){
	for(int i=0;;i++){
		char **argv=cparser(stdin);
		if(argv==NULL) break;
		printf("%d) %s %s %s\n",i,argv[0],argv[1],argv[2]);
	}
	return 0;
}
*/
