 #include "stdio.h"
 #include "string.h"

int main(){
	unsigned char a ;
	char stmp[50];
	char *s1 = "temp1";
	char *s2 = "temp2";
	sprintf(stmp,"%s nihao %s",s1,s2);
	printf("%s len:%d\n",stmp,strlen(stmp));
	char *stmp2 = new char[strlen(stmp)+1];
	strcpy(stmp2,stmp);
	printf("2 %s len:%d\n",stmp2,strlen(stmp2));
	for(int i = 0; i< 300; i++)
	{
		a++;
		printf("%d	", a);
		if(i%20 == 0)	printf("\n");
	}
	printf("\n");
	return 0;

}
