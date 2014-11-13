#include "stdio.h"


int main(){


char CONSTSTR[]="심박 조율기 탐지 신규 환자";
//printf("%s \n",CONSTSTR); 
char *str = &CONSTSTR[0];

		int serrior = 0;
		unsigned long  welldisplay[1000];
		unsigned   char display;
		int        xb=0;
		while (*str!=0x00){    //原始字符串结束符判定！
			char stmp[4]={0};
			display=*str;
			if   (display>=0x80 && display<0xe0){                       //双字节编码字符，可为汉字或双字节越南字符！
				stmp[0]=str[0];stmp[1]=str[1];
				welldisplay[xb]= display ;
				str++;
				display=*str;
				welldisplay[xb]=(welldisplay[xb])*256+display;
				str++;
			}
			else{
				if(display>0xe0 ){ 					//三字节编码越南字符！
					stmp[0]=str[0];stmp[1]=str[1];stmp[2]=str[2];
					welldisplay[xb]= display*256*256;
					str++;
					display=*str;
					welldisplay[xb]=(welldisplay[xb])+display*256;
					str++;
					display=*str;
					welldisplay[xb]=(welldisplay[xb])+display;
					str++;
				}
				else{                          //标准ASCII字符！
					stmp[0]=str[0];
					welldisplay[xb]=*str;
					str++;
				}
			}
			printf("%s	%x	\n",stmp,welldisplay[xb]);
			xb++;
		}

		welldisplay[xb]=0;		//字符串编码数组，最后一个值赋零！

/*
		while (welldisplay[serrior]!=0){//当为零时，该字符串处理完毕！
			char stmp[] = welldisplay[serrior];
			printf("%s	%x	\n",stmp,welldisplay[serrior]);
			serrior++;
		}
*/
return 0;
}
