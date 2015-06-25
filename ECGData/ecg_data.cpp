

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/msg.h>
#include "unistd.h"

#define ALL_ECG "/root/workdir/data.txt"
#define ECG_TXT "/root/workdir/ecg_data.txt"
#define RESP_TXT "/root/workdir/resp_data.txt"
#define ECG_ARRAY "/root/workdir/array_ecg_data.txt"

#define ECG_LENGTH 2000*1024
#define LEN_SHOW 20

#define ECG_DATA1(x,y) ((UINT)((UINT)x|(((UINT)y & 0xF0)<<4)))
#define ECG_DATA2(x,y) ((UINT)((UINT)x|(((UINT)y & 0x0F)<<8)))
#define UINT unsigned int

// #define TEST

typedef unsigned char			BYTE;	//字节型

BYTE data_buf_s[ECG_LENGTH];
BYTE data_buf_array[ECG_LENGTH];
BYTE data_buf[ECG_LENGTH];
int  ecg_data[ECG_LENGTH];
int  resp_data[ECG_LENGTH];

FILE *all_ecg_fd;
FILE *ecg_data_fd;
FILE *resp_data_fd;
FILE *ecg_data_array_fd;

int all_data_index = 0;
int ecg_data_index = 0;
int resp_data_index = 0;

int m_iEcgTdNum = 0;
bool m_bResp = false;


bool isHexchar(BYTE cc)
{
	if((cc>='0' && cc<='9') || (cc>='a' && cc<='f'))
	{
		return true;
	}
	else
	{
		return false;
	}
}

int chartoint(BYTE cc)
{
	if(cc>='0' && cc<='9')
	{
		return (cc-'0');
	}
	else if(cc>='a' && cc<='f')
	{
		return (cc-'a'+10);
	}
	else if(cc>='A' && cc<='F')
	{
		return (cc-'A'+10);
	}
	else
	{
		printf("char error!!!\n");
		return -1;
	}
}

void convert(BYTE *str, BYTE *Hex)
{
	int index = 0;
	for(int i= 0;str[i]!= 0;i++)
	{
		if(str[i] == '\n' || str[i] == ' ')
		{
			continue;
		}
		else
		{
			if( isHexchar(str[i]))
			{
				
				if(isHexchar(str[i+1]))
				{
					Hex[index++] = chartoint(str[i])*16 +chartoint(str[i+1]);
					i++;
		 		}
				else
				{
#ifdef TEST
					printf("not 2 char for 16Hex %c%c !\n",str[i],str[i+1]);
#endif
					continue;
				}
			}
			else
			{
#ifdef TEST
				printf("not 1 char for 16hex\n");
#endif
				continue;
			}
		}		
	}
	all_data_index = index;
}

int  readAllData()
{
// 	char *s_write_time = NULL;
// 	size_t tmplen = 0;
// 	getline(&s_write_time,&tmplen,all_ecg_fd);
// 	printf("time %s\n",s_write_time);
// 	free(s_write_time);
	
	int readnum = fread(data_buf_s,sizeof(BYTE),ECG_LENGTH,all_ecg_fd);
	if(readnum == 0)
	{
	      printf("read file %s fail, error no:%d\n",ALL_ECG,readnum);
	      return 0;
	}
// 	all_data_index = readnum;
	printf("read file OK, size :%d\n",readnum);
	int array_index = 0;
	for(int i=0;i<readnum;i++)
	{
#ifdef TEST	
		//printf("%02x ",data_buf[i]);
		printf("%c",data_buf_s[i]);
#endif
		if(data_buf_s[i] == ' ')
		{
			if(isHexchar(data_buf_s[i+1]))
			{
				data_buf_array[array_index++] = ',';
				data_buf_array[array_index++] = '0';
				data_buf_array[array_index++] = 'x';
			}
			else if(data_buf_s[i+1] == '\n')
			{
			  	data_buf_array[array_index++] = ',';
				data_buf_array[array_index++] = '\n';
				data_buf_array[array_index++] = '0';
				data_buf_array[array_index++] = 'x';
				i++;
			}
			else
			{
				data_buf_array[array_index++] = ',';
			}
		}
		else{
			data_buf_array[array_index++] = data_buf_s[i];
		}
		
	}
	
	int writenum = fwrite(data_buf_array,array_index,1,ecg_data_array_fd);
	if(writenum <=0)
	{
		printf("write ecg array data error\n");
	}
	printf("\n");

	printf("convert str to hex array\n");	
	convert(data_buf_s,data_buf);
#ifdef TEST	
	for(int i=0;i<all_data_index;i++)
	{
		printf("%02x ",data_buf[i]);
	}
#endif
	printf("\n all ecg data len:%d\n",all_data_index);
	return 1;
}


void analyse(BYTE *buf,int buf_long,int style){
	if(m_iEcgTdNum == 0)
	{
		return;
	}
	//get ecg1 data
	int val[40];	
	int i;
	int ecg_td = 1;
	for(int k=0;k<4;k++){
		switch(k){
		case 0:
			for(i=0;i<ecg_td;i++){
				val[i]=ECG_DATA1(buf[3+i*6],buf[5+i*6]);
			}
			break;
		case 1:
			for(i=0;i<ecg_td;i++){
				val[i]=ECG_DATA2(buf[4+i*6],buf[5+i*6]);
			}
			break;
		case 2:
			for(i=0;i<ecg_td;i++){
				val[i]=ECG_DATA1(buf[6+i*6],buf[8+i*6]);
			}
			break;
		case 3:
			for(i=0;i<ecg_td;i++){
				val[i]=ECG_DATA2(buf[7+i*6],buf[8+i*6]);
			}
			break;
		default:
			break;
		}
// 		val[0]=ECG_DATA1(buf[3],buf[5]);
// 		val[1]=ECG_DATA1(buf[4],buf[5]);
// 		val[2]=ECG_DATA1(buf[6],buf[8]);
// 		val[3]=ECG_DATA1(buf[7],buf[8]);
		ecg_data[ecg_data_index++] = val[0];
	}

	//得到RESP数据get resp data
	if(m_bResp)
	{
		resp_data[resp_data_index++]=(BYTE)(buf[27-(4-m_iEcgTdNum)*6]);
	}
}

void parseAllData()
{
	printf("parse data begin\n");
	int p=0;
	int s=all_data_index;
	m_iEcgTdNum = 0;
	while(p+24<s)
	{
		if(data_buf[p]==0xaa)
		{
// 			if((data_buf[p+1] & 0x10) )
// 			{
				int blocklen =  data_buf[p+2];
				//数据包
				if(p+blocklen-1<s){
					if((data_buf[p+blocklen+3] & 0xf)==0xc)
					{
						switch(blocklen)
						{
						case 0x19:case 0x18:
							  m_iEcgTdNum = 4;
							  break;
						case 0x13:case 0x12:
							  m_iEcgTdNum = 3;
							  break;
						case 0x0C:case 0x0B:
							  m_iEcgTdNum = 2;
							  break;
						case 0x07:case 0x06:
							  m_iEcgTdNum = 1;
							  break;
						default:
#ifdef TEST
							  printf("ECG block len %02x cotinue!\n",blocklen);
#endif
							  break;
						}
						if(blocklen == 0x19 || blocklen == 0x13 || blocklen == 0x0C || blocklen == 0x07)
						{
							m_bResp = true;
						}
#ifdef TEST
						printf(" wave data ecg_td:%d len:%02x ",m_iEcgTdNum,blocklen);
						for(int i=0;i<blocklen;i++)
						{
							printf("%02x ",data_buf[p+i]);
						}
						printf("\n");
#endif
						//Analyse Data
						analyse(data_buf+p,blocklen,0);
						p+=blocklen;
					}
					else
					{
						p++;
					}
				}else{
					break;
				}
// 			}
// 			else
// 			{
// 				p++;
// 			}
		}
		else
		{
			p++;
		}
	} //end of while
	printf("parse data pass\n");
}



void writeECgRespData()
{
	printf("ecg data num: %d\n",ecg_data_index);
	char stmp[20] ={0};
	int tmpsize = 0;
	for(int i= 0;i<ecg_data_index;i++)
	{
		
#ifdef TEST	  
		printf("%d ",ecg_data[i]);
#endif		
		if(i%LEN_SHOW == 0)
		{
			tmpsize = sprintf(stmp,"\n%03d ",ecg_data[i]);
		}
		else
		{
			tmpsize = sprintf(stmp,"%03d ",ecg_data[i]);
		}
		int num = fwrite(stmp,tmpsize,1,ecg_data_fd);
		if(0 == num)
		{
			printf("write ecg error!\n");
		}
	}
	printf("\n");

// 	int num = fwrite(ecg_data,ecg_data_index,1,ecg_data_fd);
// 	if(0 == num)
// 	{
// 		printf("write ecg error!\n");
// 	}
	
	printf("resp data num: %d\n",resp_data_index);

	for(int i= 0;i<resp_data_index;i++)
	{
#ifdef TEST		  
		printf("%02x ",resp_data[i]);
#endif	
		if(i%LEN_SHOW == 0)
		{
			tmpsize = sprintf(stmp,"\n%03d ",resp_data[i]);
		}
		else
		{
			tmpsize = sprintf(stmp,"%03d ",resp_data[i]);
		}		
		int resp_num = fwrite(stmp,tmpsize,1,resp_data_fd);	
		if(0 == resp_num)
		{
			printf("write resp error!\n");
		}
	}
	printf("\n");

// 	int resp_num = fwrite(resp_data,resp_data_index,1,resp_data_fd);	
// 	if(0 == resp_num)
// 	{
// 		printf("write resp error!\n");
// 	}
}

int  main()
{
	if( (all_ecg_fd = fopen(ALL_ECG,"r") )== NULL)
	{
		printf("open file hal ecg error!\n");
		return 1;
	}
	
	if( (ecg_data_array_fd = fopen(ECG_ARRAY,"wt+")) == NULL)
	{
		printf("open file  ecg error!\n");
		return 1;
	}	
	if( (ecg_data_fd = fopen(ECG_TXT,"wt+")) == NULL)
	{
		printf("open file  ecg error!\n");
		return 1;
	}
	if( (resp_data_fd = fopen(RESP_TXT,"wt+")) == NULL)
	{
		printf("open file  resp error!\n");
		return 1;
	}
	if( 0 ==readAllData())
	{
		return 1;
	}
	parseAllData();
	writeECgRespData();
	
	
	fflush(ecg_data_fd);
	fflush(resp_data_fd);
	fclose(all_ecg_fd);
	fclose(ecg_data_fd);
	fclose(resp_data_fd);
	
	printf("close all files \n");
	return 0;
}
