#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <asm/ioctl.h>
#include <ulimit.h>
#include <unistd.h>
#include <fcntl.h>

#define  PAR_NONE_MODE    0
#define  PARODD_MODE      1
#define  PAREVEN_MODE     2
#define  D5_BIT_MODE    0
#define  D6_BIT_MODE    1
#define  D7_BIT_MODE    2
#define  D8_BIT_MODE    4

#define BPS1200     0
#define BPS2400     1
#define BPS4800     2
#define BPS9600     3
#define BPS19200    4
#define BPS38400    5
#define BPS115200   6


#define BYTE char
int Fd;

int InitArmChannel(int bpsid,int parid,int serialnum,int leng = D8_BIT_MODE){
	int Fdt=-1;

	//打开串口，获得串口句柄(open serial)
	struct termios siosave;
	switch(serialnum){
	case 9:
		Fdt = open("/dev/ttyS0", O_RDWR|O_NONBLOCK);          //sysboard
		break;
	default:
		Fdt=-1;
		break;
	}

	if(Fdt < 0){
		goto Exit_P;
	}

	tcgetattr(Fdt,&siosave);
	siosave.c_iflag &= ~(BRKINT|ICRNL|ISTRIP|IXON);
	siosave.c_oflag = 0;
	siosave.c_cflag &= ~(CSIZE|PARENB);
	switch(parid){
	case PAR_NONE_MODE:
		break;
	case PARODD_MODE:
		siosave.c_cflag |= PARENB;
		siosave.c_cflag |= PARODD;
		break;
	case PAREVEN_MODE:
		siosave.c_cflag |= PARENB;
		break;
	}

	switch(leng){
	case D5_BIT_MODE:
		siosave.c_cflag |= CS5 | (CLOCAL|CREAD);
		break;
	case D6_BIT_MODE:
		siosave.c_cflag |= CS6 | (CLOCAL|CREAD);
		break;
	case D7_BIT_MODE:
		siosave.c_cflag |= CS7 | (CLOCAL|CREAD);
		break;
	case D8_BIT_MODE:
		siosave.c_cflag |= CS8 | (CLOCAL|CREAD);
		break;
	default:
		siosave.c_cflag |= CS8 | (CLOCAL|CREAD);
		break;
	}

	if(serialnum==9){
//		printf("Set Key Seril 2 stop bit mode\n")   ;
//		siosave.c_cflag |=CSTOPB;//键盘板通讯改2停止位，需配合更新的内核才能生效！
		siosave.c_cflag &=~CSTOPB;
	}
	siosave.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
	siosave.c_iflag |= IGNBRK;
	siosave.c_cc [VMIN] = 0;
	siosave.c_cc [VTIME] = 0;

	switch(bpsid){
	case BPS1200:
		cfsetispeed(&siosave,B1200);
		cfsetospeed(&siosave,B1200);
		break;
	case BPS2400:
		cfsetispeed(&siosave,B2400);
		cfsetospeed(&siosave,B2400);
		break;
	case BPS4800:
		cfsetispeed(&siosave,B4800);
		cfsetospeed(&siosave,B4800);
		break;
	case BPS9600:
		cfsetispeed(&siosave,B9600);
		cfsetospeed(&siosave,B9600);
		break;
	case BPS19200:
		cfsetispeed(&siosave,B19200);
		cfsetospeed(&siosave,B19200);
		break;
	case BPS38400:
		cfsetispeed(&siosave,B38400);
		cfsetospeed(&siosave,B38400);
		break;
	case BPS115200:
		cfsetispeed(&siosave,B115200);
		cfsetospeed(&siosave,B115200);
		break;
	}

	if(tcsetattr(Fdt,TCSANOW,&siosave)){
		Fdt=-1;
		goto Exit_P;
	}
	else{
		printf("Open com sucess.  handle = %d,serial=%d\n",Fdt,serialnum);
	}

	tcflush(Fdt, TCIFLUSH);//丢掉待发数据(flush data that will be send)
	//printf(" TCIFLUSH call\n");
	//设置回调函数(set recall func)
	//dbout("    Fl::add_fd...");
	//Fl::add_fd(m_iFdt,Data_Arrived,this);
	//出口(return)
Exit_P:
	return Fdt;
}

void SendCmd(BYTE *cmd,int len){
	int checkSun = 0 ;
	BYTE Bf_Cmd[30];
	Bf_Cmd[0]=0x55;//STX
	Bf_Cmd[1]=0x04;//VER
	Bf_Cmd[2]=(BYTE)(9+len);//Len
	Bf_Cmd[3]=0;//CHNNEL
	Bf_Cmd[4]=0x01;//SADD
	Bf_Cmd[5]=0x36;//m_uaddr;//TADD//根据不同链接对象
	Bf_Cmd[6]=0x01;//PID
	for (int i= 0 ;i<len ;i++) {//数据内容
		Bf_Cmd[7+i] = *(cmd+i) ;
	}
	Bf_Cmd[7+len] = 0 ;
	for(int k = 1; k< 7+len; k++){//校验位
		//printf ("Bf_Cmd[7+len]=%x ,Bf_Cmd[k]=%x \n",Bf_Cmd[7+len],Bf_Cmd[k] ) ;
		checkSun =checkSun+ Bf_Cmd[k];
	}
	Bf_Cmd[7+len]=checkSun%0x100 ;
	Bf_Cmd[8+len]=0xAA;//ETX
	write(Fd,Bf_Cmd,8+len+1);	//上行数据双通道通讯，下行数据选择其一！
}

void WriteWatchdog(){
	printf("************ close APP watchdog*****************\n");
	BYTE shopWatchKernelCmd[2]={0x23,1};//
	SendCmd(shopWatchKernelCmd,2);
	printf("************ close KERNEL watchdog*****************\n");
	BYTE shopWatchAppCmd2[2]={0x23,2};//
	SendCmd(shopWatchAppCmd2,2);
}

int main(){
	Fd = InitArmChannel(BPS4800,PAR_NONE_MODE,9);
	if(Fd >0){
		WriteWatchdog();
		close(Fd);
		printf("**************OK***********************\n");
	}
	else{
		printf("**********init fd error********\n");
	}
	return 0;
}
