
/*
file:main.cpp
by:wr
data:2002-5
discription:
*/

#include "func_trace.h"
#include "wr_state.h"
#include "wr_datamgr.h"
#include "cdisplang.h"		//ccz_2009_03_11 语言字符串显示界面
#include <sys/mount.h>	//ll_09_03_25
#include "kj_msg_define.h"	//ll_09_03_25
#include "softsound.h"

#include <FL/Fl_Pack.H>
class CE7Frame : public Fl_Widget
{
public:
        CE7Frame(int x, int y, int w, int h)
                : Fl_Widget(x, y, w, h, NULL)
        {
        }
public:
        virtual void draw()
        {
        }
};

#ifndef WIN32
#define NO_DATA_WAIT_TIME				8	//嵌入式系统中用
//#define NO_DATA_WAIT_TIME				4	//本机测试用
#else
#define NO_DATA_WAIT_TIME				1	//Windows下测试用
#endif

#define FORCE_WRITE_DATA_TO_BOARD		0	//非强制向硬件写入数据
//#define FORCE_WRITE_DATA_TO_BOARD		1	//强制向硬件写入数据

#define 	LC_MODE_DETECT_TIME			20

//define value
CDataMgr	g_mgr;
CState		g_st[BED_NUMBER];
double		g_dTmIntval;
int 			g_timer_per_second;
bool			g_bBigScreen=false;
int 			SPEED_POINT_0;
int 			SPEED_POINT_1;
int 			SPEED_POINT_2;
int 			SPEED_POINT_3;
bool			g_bGetIniData=false;
UINT			g_uMainId;
bool			g_bStartSendTimer=false;
UINT			g_uShowVersion = 0;
UINT            g_uDebugWinConuntDown=0 ;
#ifdef    ECG_GAIN_DUG
UINT            g_ecgDebugCount = 0 ;
UINT            g_ecgDebugCount2 = 0 ;
#endif
bool        	g_bQuit = false;
bool			g_show_product = false;

UINT 		g_uBedTmp=0;
UINT 		g_uMaxLenTmp=0;
UINT 		g_uNowLenTmp=0;
STATE_DATA 	g_sdTmp[100];

CImageLabel	*g_pDefFocus=NULL;
bool isBTopen = false;

CDispLang	g_displang;			//ccz_2009_03_11 语言字符串显示界面
UINT			g_uShowLangWin = 0;
bool			g_LcMode=false;
bool			g_temp_separate=false;
int			g_key_data_num=0;
bool			bKeystroke=false;//是否打开按键声音

//ST REPLAY
char PATH_BASE_ST[100];
char PATH_ST_REPLAY[100];
char PATH_ST_REPLAY_INDEX[100];
int ST_RECORD_COUNT;
//TD DISPLAY
char PATH_BASE_TD[100];
char PATH_TD_DISPLAY[100];
char PATH_TD_DISPLAY_INDEX[100];
//NIBP LIST
char PATH_NIBP_DATA[100];//存储  “长列表框” 中的数据
char PATH_NIBP_DATA_SHORT[100];//存储  “短列表框” 中的数据


/**
	打开声音使用发送消息机制
	接口为send_msg
*/
#include <sys/msg.h>

#define WAV_MSG_KEY 	2844

struct wav_msg_t {
	long int msg_type;
	int flag;
	int vol;
	int snd_id;
	wav_msg_t(){
		msg_type = 1;
		vol = 128;//0~128
	}
};

static int msg_id;
struct wav_msg_t wav_msg;

void send_msg(int type, int vol, int flag){
	wav_msg.msg_type = 1;
	wav_msg.vol = vol;
	wav_msg.flag = flag;
	wav_msg.snd_id = type;
	msgsnd(msg_id, (void *)&wav_msg, sizeof(struct wav_msg_t) - sizeof(long int),IPC_NOWAIT);
}

//define function
#ifdef	G80_NEW_HARDWARE_PLATFORM
void			G80_boot();
void			SendKeyBufTask(void *);
#endif
void			SendTimer(void *);
void			SendKbTimer(void *);
void			Flush_com(void *);
void			Ignore_Err(void *);
int				handle(int e);
void			InitClass();
void			GetSaveData(void *);

void      LoadSaveData();

void			Init(void *);
void			SendAskToHal(void *);
void			TimerSave(void *);
void			WriteDefaultData();
bool			IsDataValidity();
void			EndNightMode();
void			EndStandbyMode();
void            vitasQuitSys() ;
void            vitasResetState(void *) ;
//#define SUPPORT_CORE
#undef SUPPORT_CORE
#ifdef   SUPPORT_CORE

//hqj_09_07_19 start

/* vi: set sw=4 ts=4: */
/* dmesg.c -- Print out the contents of the kernel ring buffer
 * Created: Sat Oct  9 16:19:47 1993
 * Revised: Thu Oct 28 21:52:17 1993 by faith@cs.unc.edu
 * Copyright 1993 Theodore Ts'o (tytso@athena.mit.edu)
 * This program comes with ABSOLUTELY NO WARRANTY.
 * Modifications by Rick Sladkey (jrs@world.std.com)
 * Larger buffersize 3 June 1998 by Nicolai Langfeldt, based on a patch
 * by Peeter Joot.  This was also suggested by John Hudson.
 * 1999-02-22 Arkadiusz Mikiewicz <misiek@misiek.eu.org>
 * - added Native Language Support
 *
 * from util-linux -- adapted for busybox by
 * Erik Andersen <andersee@debian.org>. I ripped out Native Language
 * Support, replaced getopt, added some gotos for redundant stuff.
 */

//#include "internal.h"
#include <stdio.h>
#include <stdlib.h>

#if __GNU_LIBRARY__ < 5

#ifndef __alpha__
# define __NR_klogctl __NR_syslog
static inline _syscall3(int, klogctl, int, type, char *, b, int, len);
#else							/* __alpha__ */
#define klogctl syslog
#endif

#else
# include <sys/klog.h>
#endif

static const char dmesg_usage[] = "dmesg [-c] [-n LEVEL] [-s SIZE]\n"
#ifndef BB_FEATURE_TRIVIAL_HELP
	"\nPrints or controls the kernel ring buffer\n\n"
	"Options:\n"
	"\t-c\t\tClears the ring buffer's contents after printing\n"
	"\t-n LEVEL\tSets console logging level\n"
	"\t-s SIZE\t\tUse a buffer of size SIZE\n"
#endif
	;

#define DEBUG_DUMP_NAME			"/logdir/dump.data"
char *arg[] = {"dmesg"};
int dmesg_main(int argc, char **argv)
{
	FILE        *fp;
	char *buf;
	char *buf2;
	int bufsize = 10240;
	int i;
	int n;
	int level = 0;
	int lastc;
	int cmd = 3;
	int stopDoingThat;

	argc--;
	argv++;



	printf("11...\n");

	fp=fopen(DEBUG_DUMP_NAME,"wb");
	printf("22...\n");

	/* Parse any options */
	while (argc && **argv == '-') {
		stopDoingThat = false;
		while (stopDoingThat == false && *++(*argv)) {
			switch (**argv) {
			case 'c':
				cmd = 4;
				break;
			case 'n':
				cmd = 8;
				if (--argc == 0)
					goto end;
				level = atoi(*(++argv));
				if (--argc > 0)
					++argv;
				stopDoingThat = true;
				break;
			case 's':
				if (--argc == 0)
					goto end;
				bufsize = atoi(*(++argv));
				if (--argc > 0)
					++argv;
				stopDoingThat = true;
				break;
			default:
				printf("33...\n");
				goto end;
			}
		}
	}
	printf("44...\n");
	if (argc > 1) {
		printf("55...\n");
		goto end;
	}

	if (cmd == 8) {
		n = klogctl(cmd, NULL, level);
		if (n < 0) {
			goto klogctl_error;
		}

		exit(true);
	}
	printf("1...\n");
	bufsize=10240;
	buf = (char *) malloc(bufsize);
	n = klogctl(cmd, buf, bufsize);
	printf("n=%d\n");

	if (n < 0) {
		goto klogctl_error;
	}
	buf2=buf+(10240-4096);
	fprintf(fp,"%s",buf);
	fwrite(buf, 4096, 1, fp);
	printf("%s\n",buf);
	/*
	lastc = '\n';
	printf("2...\n");
	for (i = 0; i < n; i++) {
		if ((i == 0 || buf[i - 1] == '\n') && buf[i] == '<') {
			i++;
			while (buf[i] >= '0' && buf[i] <= '9')
				i++;
			if (buf[i] == '>')
				i++;
		}
		lastc = buf[i];
		//putchar(lastc);
		fprintf(fp,"%c",buf[i]);
		printf("%c",buf[i]);


	}
	if (lastc != '\n'){
		putchar('\n');
		//fprintf(fp,'\n');
	}
	*/
	sync();
	fclose(fp);
	printf("Close dump.data file...ok\n");
	return(true);
  end:
	//usage(dmesg_usage);
	fclose(fp);
	printf("Close dump.data file...2\n");
	return(false);
  klogctl_error:
	perror("klogctl");
	fclose(fp);
	printf("Close dump.data file...3\n");
	return(false);
}

// void sig_int(int sig){
// 	//system("dmesg >  /logdir/kernel.log");
// 	dmesg_main(1,arg);
// 	printf("Catch a termination single=%d\n",sig);
// 	exit(0);
// }


void sig_int(int sig){
	switch(sig){
		case SIGINT:
			printf("Rev SIGINT\n");
			break;
		case SIGILL:
			printf("Rev SIGILL\n");
			break;
		case SIGFPE:
			printf("Rev SIGFPE\n");
			break;
		case SIGABRT:
			printf("Rev SIGABRT\n");
			break;
		case SIGSEGV:
			printf("Rev SIGSEGV\n");
			break;
		case SIGPIPE:
			printf("Rev SIGPIPE\n");
			break;
		case SIGTERM:
			printf("Rev SIGTERM\n");
			break;
		case SIGHUP:
			printf("Rev SIGHUP\n");
			break;
		case SIGQUIT:
			printf("Rev SIGQUIT\n");
			break;
		default:
			printf("Rev Default Signal\n");
			break;	
	}
	exit(0);
}
#endif
//hqj_09_07_19 end

/*
主函数
main function
*/
CBox*  pBootBox;
CBox*	pLcInfoBox;
CDialog*	pLcInfoDialog;

void msg_init(){
	/* 初始化消息 */
	msg_id = msgget((key_t)WAV_MSG_KEY, 0666 | IPC_CREAT);

	if (msg_id == -1) {
		perror("can't create msg queue from system");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
   cyg_profile_init();
	dbout("main: Start Program...");
	g.m_Trace.RecLog("main: Start Program...", 0 );
#ifdef  SUPPORT_CORE
	//system("dmesg > /kernel.log" ) ;	//hqj_09_07_19 start
	msg_init();
	signal(SIGINT, &sig_int);
	signal(SIGILL, &sig_int);
	signal(SIGFPE, &sig_int);
	signal(SIGABRT, &sig_int);
	signal(SIGSEGV, &sig_int);
	signal(SIGPIPE, &sig_int);
	signal(SIGTERM, &sig_int);
	signal(SIGHUP, &sig_int);
	signal(SIGQUIT, &sig_int);

#endif
	//hqj_09_07_19 end


	LoadSaveData();


	STATE_DATA sd;
// 	//根据出厂设置初始化LC模式标志
// 	if(g_st[0].GetStateData(S_LC_MODE)){
// 		g_LcMode=true;
// 	}else{
// 		g_LcMode=false;
// 	}
//发布版本进行检测！
#ifndef G80_NEW_HARDWARE_PLATFORM
// 	Fl::add_timeout(LC_MODE_DETECT_TIME, SendKbTimer);//检测键盘接口是否正常初始化
#endif

	//根据出厂设置体温隔离
	if(g_st[0].GetStateData(S_TEMP_TWGL)){
		g_temp_separate=true;
	}else{
		g_temp_separate=false;
	}

	if(0 == g_st[0].GetStateData(S_WATCHDOG))//Liubaoli 手动控制看门狗开关
	{
		system("touch /cfgdir/ut5000.wd");
	}else{
		system("rm -f /cfgdir/ut5000.wd");
	}

	//ll_09_03_25 datamanage centre
	if(DATA_STOR_MODE_USB==g_st[0].GetStateData(S_DATA_STOR_MODE)
	   && 0!=g_st[0].GetStateData(S_SHOW_DATA_MGR_CEN)){
		printf("data storage mode: usb\n");
		system("umount /usb/");
		sleep(3);
		int imount=mount("/dev/sda1", "/usb/", "vfat", MS_SYNCHRONOUS, NULL);
		//int imount=mount("/dev/sda1", "/usb/", "vfat", MS_SYNCHRONOUS, "iocharset=utf8"); //pc
		if(imount==0){
			printf("mount OK\n");
			printf("data manage center path: /usb/\n");
			//ST
			strcpy(PATH_BASE_ST, "/usb/");
			strcpy(PATH_ST_REPLAY, "/usb/st_replay.tmp");
			strcpy(PATH_ST_REPLAY_INDEX, "/usb/stindex");
			ST_RECORD_COUNT=800;
			//TD
			strcpy(PATH_BASE_TD, "/usb/");
			strcpy(PATH_TD_DISPLAY, "/usb/td_display.tmp");
			strcpy(PATH_TD_DISPLAY_INDEX, "/usb/tdindex");
			//NIBP
			strcpy(PATH_NIBP_DATA, "/usb/nibp.save");
		}else{
			perror("mount error");
			printf("data manage center path: /logdir/\n");
			//ST
			strcpy(PATH_BASE_ST, "/logdir/");
			strcpy(PATH_ST_REPLAY, "/logdir/st_replay.tmp");
			strcpy(PATH_ST_REPLAY_INDEX, "/logdir/stindex");
			ST_RECORD_COUNT=10;
			//TD
			strcpy(PATH_BASE_TD, "/logdir/");
			strcpy(PATH_TD_DISPLAY, "/logdir/td_display.tmp");
			strcpy(PATH_TD_DISPLAY_INDEX, "/logdir/tdindex");
			//NIBP
			strcpy(PATH_NIBP_DATA, "/logdir/nibp.save");
		}
	}else{
		printf("data storage mode: flash\n");
		//ST
		strcpy(PATH_BASE_ST, "/logdir/");
		strcpy(PATH_ST_REPLAY, "/logdir/st_replay.tmp");
		strcpy(PATH_ST_REPLAY_INDEX, "/logdir/stindex");
		ST_RECORD_COUNT=10;
		//TD
		strcpy(PATH_BASE_TD, "/logdir/");
		strcpy(PATH_TD_DISPLAY, "/logdir/td_display.tmp");
		strcpy(PATH_TD_DISPLAY_INDEX, "/logdir/tdindex");
		//NIBP
		strcpy(PATH_NIBP_DATA, "/logdir/nibp.save");
		strcpy(PATH_NIBP_DATA_SHORT, "/logdir/nibp_short.save");
	}
	//ll_09_03_25 end

	//--------------------------------------------------------
	dbout("main: Fl::args");
	Fl::args(argc,argv);
	//putenv("DISPLAY=:0");

	//创建窗口(create window)
	dbout("main: Create windows...");
	g.m_Trace.RecLog("main: Create windows...", 0);
	g.m_pMainWin=new CDWin(MAIN_WINDOW_LEFT,MAIN_WINDOW_TOP,MAIN_WINDOW_WIDTH,MAIN_WINDOW_HEIGHT,"UT6000F MAIN");
	assert(g.m_pMainWin);
	g.m_pMainWin->color(0);
	g.m_pMainWin->border(0);

#ifdef	G80_NEW_HARDWARE_PLATFORM
#if     (__MODE__ == MODE_1280X1024)
    pBootBox=new CBox(550,510,200,30,g.GetTxt(TXT_QDJC));
#elif  (__MODE__ == MODE_1024X768)
    pBootBox=new CBox(412,350,200,30,g.GetTxt(TXT_QDJC));
#elif  (__MODE__ == MODE_800X600)
    pBootBox=new CBox(290,270,200,30,g.GetTxt(TXT_QDJC));//800*600
#endif
	pBootBox->SetBigFontStyle(1);
	pBootBox->color(FL_RED);
	//pBootBox->SetLabel(g.GetTxt(TXT_QDJC));
	//pBootBox->labelcolor(FL_BLUE);
	g.m_pMainWin->add(pBootBox);
pBootBox->Paint(DMW_BOX_PAINTALL,NULL,DP_UPDATE_CHILD) ;
#endif

	pLcInfoDialog = new CDialog(300, 270, 200, 70, "Prompt");
	pLcInfoBox=new CBox(50, 30, 70, 30, g.GetTxt(TXT_REBOOT));
	pLcInfoBox->labelcolor(FL_RED);
	pLcInfoDialog->add(pLcInfoBox);
	pLcInfoDialog->hide();

	g.m_pMainWin->end();

	g.m_pMainWin->SetOPaintMode();

	g.m_pMainWin->show();
	g.m_pMainWin->make_current();
	dbout("main: Create windows OK.");
	g.m_Trace.RecLog("main: Create windows OK.", 0);

	//加载图片
	dbout("main: LoadPics...");
	g.m_Trace.RecLog("main: LoadPics...", 0);
	g.LoadPics();
	dbout("main: LoadPics OK.");
	g.m_Trace.RecLog("main: LoadPics OK.", 0);
     	
// 	LoadSaveData();
// #endif
	//初始化类/得到保存的数据
	//init class start/get save data

	//取得语言版本
		STATE_DATA sd1={S_LANGUAGE_VISION_USR,0};
		g_st[0].GetStateData(&sd1,1);
		g.SetLanguageVision(sd1.data);


	InitClass();

	//create bed
	dbout("main: Create Bed...");
	g.m_Trace.RecLog("main: Create Bed...", 0);
	if(false==g_mgr.CreateBed()){
		dbout("main: Create Bed Error!");
		g.m_Trace.RecLog("main: Create Bed Error!", 0);
		exit(0);
	}

	dbout("main: Show Window...");
	g.m_Trace.RecLog("main: Show Window...", 0);
// #ifdef 
	
// 	g.m_pMainWin->set_non_modal();
// #else
	g.m_pMainWin->set_modal();
// #endif
	//hide mouse
	dbout("main: Hide Mouse...");
	if(!g_st[0].GetStateData(S_CMP)){
		g.m_pMainWin->cursor(FL_CURSOR_NONE);
	}

	dbout("main: Windows Show OK.");
	g.m_Trace.RecLog("main: Windows Show OK.", 0);

	//go
	GetSaveData(0);
#ifdef	G80_NEW_HARDWARE_PLATFORM
	pBootBox->SetLabel(g.GetTxt(TXT_QDJC));
	pBootBox->labelcolor(FL_BLUE);
#endif
	// 	Fl::add_timeout(0.5,GetSaveData);
	// 	dbout("main: Wait To Ask Save Data......");

        //added by lijia {{
        #if     0
        do {
                CE7Frame* p_frame = new CE7Frame(0, 0, 0, 50);
                /*Fl_Window *window = NULL;
                Fl_Pack *gp = NULL;
                Fl_Button* btn = new Fl_Button(0, 0, 0, 50);
                Fl_Button* btn2 = new Fl_Button(0, 0, 0, 20);

                window = new Fl_Window(300,180);
                window->color(FL_BLUE);
                window->begin();
                gp = new Fl_Pack(0, 0, 260,100);
                gp->color(FL_RED);
                gp->begin();
                gp->add(btn);
                gp->add(btn2);
                gp->end();
                window->end();
                window->show();*/
                p_frame->show();
                
                return Fl::run();
        } while (0);
        #endif
        //}} added by lijia

	return Fl::run();
	printf("end fltk run\n");
	exit(1);

}


void LoadSaveData(){
	g_st[0].LoadAllSaveData(true, true);
}


void TimerSaveData(UINT uBed,STATE_DATA *sd,UINT uLen){
	//双床位时要设置两个不同的Timer
	if(uBed!=g_uMainId) return;

	g_uBedTmp=uBed;
	if(uLen>100) uLen=100;
	g_uMaxLenTmp=uLen;
	g_uNowLenTmp=0;
	for(UINT i=0;i<uLen;i++) {
		g_sdTmp[i].id=sd[i].id;
		g_sdTmp[i].data=sd[i].data;
	}

	Fl::add_timeout(0,TimerSave);
}

void TimerSave(void *){
	STATE_DATA sd={g_sdTmp[g_uNowLenTmp].id,g_sdTmp[g_uNowLenTmp].data};
	g_st[g_uBedTmp].SetStateData(&sd,1,true,true,true);
	//dbout2("Write Ok......  %d   %d",sd.id,sd.data);
	g_uNowLenTmp++;
	if(g_uNowLenTmp<g_uMaxLenTmp){
		Fl::repeat_timeout(0.05,TimerSave);
	}
	else{
		Fl::remove_timeout(TimerSave);
	}
}

#ifdef	G80_NEW_HARDWARE_PLATFORM
void G80_boot(){
	printf("G80 boot...\n");
	g_mgr.init();
	pBootBox->hide();
	/*
	创建任务巢
	create jobnest
	*/
	dbout("main: Create Jobnest...");
	g.m_Trace.RecLog("main: Create Jobnest...", 0);

#ifndef WIN32
	g.m_pjnState=GetJobNest();
	assert(g.m_pjnState);
	g.m_pjnNet=GetJobNest();
	assert(g.m_pjnNet);
#endif//WIN32

	//hide mouse
	dbout("main: Hide Mouse...");
	g.m_Trace.RecLog("main: Hide Mouse...", 0);
	if(!g_st[0].GetStateData(S_CMP)){
		g.m_pMainWin->cursor(FL_CURSOR_NONE,FL_RED,FL_RED);
	}

	//keyboard handle
	dbout("main: Add Handler...");
	g.m_Trace.RecLog("main: Add Handler...", 0);
	Fl::add_handler(handle);

	//timer
	dbout("main: Add Timer...");
	g.m_Trace.RecLog("main: Add Timer...", 0);
	if(SAVE_TYPE==SAVE_TO_HAL){
		Fl::remove_timeout(Init);
	}
	Fl::add_timeout(0,SendTimer);
#ifdef	G80_NEW_HARDWARE_PLATFORM
	Fl::add_timeout(0,SendKeyBufTask);
#endif
	g_mgr.SetChannelFocus();
	dbout("main: Program Start OK!");
	g.m_Trace.RecLog("main: Program Start OK!", 0);
}
#endif

#ifdef	G80_NEW_HARDWARE_PLATFORM
void SendKeyBufTask(void *){
	Fl::repeat_timeout(0.005,SendKeyBufTask);
	g_mgr.SendKeyBufTask();
}
#endif

/*
整个系统所共用的时钟Timer
*/
void SendTimer(void *){

	Fl::repeat_timeout(g_dTmIntval,SendTimer);

	/*
	timer以12.5mm/s波形速度为基数
	TIMER_PER_SECOND 次/s
	*/

	static int index=0;
	g_mgr.SendTimer(index);
	index++;
	index%=g_timer_per_second;
#ifdef    ECG_GAIN_DUG
    if (index==0) {
       g_ecgDebugCount2=0 ;
    }
#endif
    if (0==index) {
        if (g_uDebugWinConuntDown>0) {
            g_uDebugWinConuntDown-- ;
        } else {
            g_uShowVersion = 0 ;
            g_uShowLangWin = 0 ;
            g_uDebugWinConuntDown = 0 ;
        }
    }
	if(!g_bStartSendTimer) {
		if(index==0){
			{
				//等待几秒钟，在状态显示后才可以进行菜单控制
				static int iTmpData=0;
				iTmpData++;

				if(iTmpData==2){
//					g_mgr.AnalyseCmd(0, S_SHOW_MODE_BIGFONT);
//					g_mgr.AnalyseCmd(0, S_SHOW_MODE_SPO2HR);//防止通道二次刷新
					/*
					//G80平台演示模式自动启动，测试需要！
					STATE_DATA sd = {S_DEMO, 1};
					sd.data = 1;
					g_st[0].SetStateData(&sd,1);
					*/
				}
				//初始化模块(等待所有模块上电后初始化)
				if(iTmpData>4){
					g_mgr.LastInit();
				}

				if(iTmpData>5){
					g_bStartSendTimer=true;

#ifdef WIN32
#ifndef NDEBUG
					g_mgr.SetShowTestTime(0,false);
					g_mgr.SetPatientInfo(0);
#endif//NDEBUG
#endif

				}
			}
		}
	}

}

void Calldump(){
	int data[10];
	for(int t=0;t<0xfffff;t++){
		data[t]=t;
	}
}

/*
系统按键控制
*/
int handle(int e){
	static bool m_bStopDebug=true;
	g_st[0].GetStateData(S_KEY_SOUND) == 0 ? bKeystroke = false : bKeystroke = true;
#ifndef NDEBUG
	if(NO_DATA_WAIT_TIME!=4){
#endif//#ifndef NDEBUG
		/*当应用程序失去窗口焦点时，调用下面语句会造成窗体焦点事件反复获得和失去！	*/

		if(!Fl::focus()){
			if(Fl::modal()){
				Fl::modal()->take_focus();
			}else{
				g.m_pMainWin->take_focus();
			}
		}

#ifndef NDEBUG
	}
#endif//#ifndef NDEBUG


#ifndef WIN32
	if(!g_bStartSendTimer){
		return 0;
	}

	if(g_bQuit){
		return 0;
	}
#endif//WIN32
	if(FL_SHORTCUT == e){
		//dbout1("*-------- %d ----------*",Fl::event_key());

		switch(Fl::event_key()){
		case FL_F+1:case 'a':    //通道菜单
#ifdef VITAS_HARDWARE_PLATFORM
{
            g.m_menu.FastOpenQSLB() ; //g.m_menu.FastOpenXSJMXC() ;
}
#else //#else VITAS_HARDWARE_PLATFORM
    #ifdef		ECG_GAIN_DUG
                if(0<g_st[0].GetStateData(S_ECG_GAIN_TEST)) {
                    g_ecgDebugCount2++ ;
                    if (g_ecgDebugCount2>3) {
                        g_ecgDebugCount2=0 ;
                        if ( 1==g_st[0].GetStateData(S_ECG_GAIN_TEST) ){
                            g_st[0].SetStateData(S_ECG_GAIN_TEST,2) ;
                        } else if ( 2==g_st[0].GetStateData(S_ECG_GAIN_TEST) ){
                            g_st[0].SetStateData(S_ECG_GAIN_TEST,3) ;
                        } else if ( 3==g_st[0].GetStateData(S_ECG_GAIN_TEST) ){
                            g_st[0].SetStateData(S_ECG_GAIN_TEST,4) ;
                        } else if ( 4<=g_st[0].GetStateData(S_ECG_GAIN_TEST) ){
                            g_st[0].SetStateData(S_ECG_GAIN_TEST,1) ;
                        }
                    }
                }
                printf ("g_st[0].GetStateData(S_ECG_GAIN_TEST)=%d\n" ,g_st[0].GetStateData(S_ECG_GAIN_TEST) ) ;
    #endif
			    if(g_st[0].GetStateData(S_STANDBY)==1){
				    EndStandbyMode();
				    break;
			    }
			    g_uShowVersion = 0;
			    g_uShowLangWin = 0;
                g_uDebugWinConuntDown = 0 ;
			    EndNightMode();//结束夜间模式
                if(g.m_menu.IsDialogOnShow() ||
                    g_mgr.GetDialogType()==DIALOG_TYPE_ST_REPLAY ||
                    g_mgr.GetDialogType()==DIALOG_TYPE_STFXM ||
                    g_mgr.GetDialogType()==DIALOG_TYPE_QS_SHOW ||
                    g_mgr.GetDialogType()==DIALOG_TYPE_VPC_REPLAY){
                    g_st[0].SetStateData(S_STARTUP_PATIENT_INFO ,false) ;
				    g.m_menu.FastReturn('a');
				    g_mgr.FastReturn('a');
			    }
			    else{
				    if(g.m_menu.IsMenuPopuped()){

					    g.m_menu.CloseMenu();

					    g.m_menu.CloseOnTopMenu();
				    }else{
					    g.m_menu.CloseAlarmMenu();
					    g.m_menu.CloseSetupMenu();
					    if(g_pDefFocus){
						    g_pDefFocus->take_focus();
					    }
					    g.m_menu.PopupChannelMenu((ALIGN_WINDOW_TYPE == INFOWIN_ALIGN_LEFT) ?
						    (SHOW_INFO_WIDTH + 2) : (MAIN_WINDOW_LEFT + 2),
						    MAIN_WINDOW_TOP + MAIN_WINDOW_HEIGHT - MAIN_MENU_HEIGHT, g_uMainId);
				    }
			    }
#endif //#else VITAS_HARDWARE_PLATFORM
			break;

		case FL_F+2:case 'd':    //波形冻结
            #ifdef		ECG_GAIN_DUG
                g_ecgDebugCount++;
                printf ("g_ecgDebugCount=%d\n" ,g_ecgDebugCount) ;
            #endif

			if(g_st[0].GetStateData(S_STANDBY)==1){
				EndStandbyMode();
				break;
			}
			if(5 != g_uShowVersion){
                g_uDebugWinConuntDown = 20 ;
				g_uShowVersion ++;
				if(g_uShowVersion > 10){
					if(!g_show_product){
						g.m_menu.DoCmd(MSG_DISP_SCREEN);

						g_show_product = true;
						g_uShowVersion = 0;
                        g_uDebugWinConuntDown = 0 ;
						g.m_DugWin.show();
					}
					else{
						g_mgr.ShowTestInfo("");
						g_show_product = false;
						g_uShowVersion = 0;
                        g_uDebugWinConuntDown = 0 ;
						g.m_DugWin.hide();
						Calldump();
					}
				}
			}

			if(5 != g_uShowLangWin){
				g_uShowLangWin++;
                g_uDebugWinConuntDown = 20 ;
				if(g_uShowLangWin > 10){
					g_displang.PopupModal(NULL, 0, 0);		//ccz_2009_03_11 语言字符串显示界面
					g_displang.ReFresh();
					g_uShowLangWin = 0;
                    g_uDebugWinConuntDown = 0 ;
				}
			}

			EndNightMode();//结束夜间模式

			//如果正在心电回放等,则按键无效
			if(DIALOG_TYPE_ST_REPLAY != g_mgr.GetDialogType()
				&& DIALOG_TYPE_QS_SHOW != g_mgr.GetDialogType()
				&& DIALOG_TYPE_VPC_REPLAY != g_mgr.GetDialogType()){
					g_mgr.StopWaveRun(!g_mgr.IsBxStop());
			}
			break;

		case FL_F+3:case 'c':    //NIBP测量
			if(g_st[0].GetStateData(S_STANDBY)==1){
				EndStandbyMode();
				break;
			}
			g_uShowVersion = 0;
			g_uShowLangWin = 0;
            g_uDebugWinConuntDown = 0 ;
			EndNightMode();//结束夜间模式

			if(!g_mgr.GetNibpRun(g_uMainId)){
				g_mgr.SetNibpRun(g_uMainId,true,true,false,true);
			}
			else{
				g_mgr.SetNibpRun(g_uMainId,false,true,true,true);
			}
			break;

		case FL_F+4:case 'b':    //报警切换
            #ifdef		ECG_GAIN_DUG
                if ( g_ecgDebugCount>4 ){
                    if ( 0==g_st[0].GetStateData(S_ECG_GAIN_TEST) ) {
                        g_st[0].SetStateData(S_ECG_GAIN_TEST ,1) ;
                        g_ecgDebugCount = 0 ;
                    } else {
                        g_ecgDebugCount = 0 ;
                        g_st[0].SetStateData(S_ECG_GAIN_TEST ,0) ;
                    }
                    printf ( "g_st[0].GetStateData=%d\n" ,g_st[0].GetStateData(S_ECG_GAIN_TEST ) ) ;
                    g_uShowLangWin = 0;
                    g_uShowVersion = 0 ;
                } 
            #endif
			if(g_st[0].GetStateData(S_STANDBY)==1){
				EndStandbyMode();
				break;
			}
			g_uShowLangWin = 0;
			if(5 == g_uShowVersion){
				g_uShowVersion ++;
                g_uDebugWinConuntDown = 20 ;
			} else {
                g_uDebugWinConuntDown = 0 ;
            }
			EndNightMode();//结束夜间模式

			g_mgr.PressSilent();
			break;

		case FL_F+5:case 'e':    //打印
			if(g_st[0].GetStateData(S_STANDBY)==1){
				EndStandbyMode();
				break;
			}
			g_uShowVersion = 0;
			g_uShowLangWin = 0;
            g_uDebugWinConuntDown = 0 ;
			EndNightMode();//结束夜间模式
			if(g.m_menu.IsDialogOnShow()){
				if(g_mgr.GetDialogType()== DIALOG_TYPE_ST_REPLAY){
					g_mgr.PrintWave(g_uMainId);
				}
				else{
					g.m_menu.FastReturn('f');
					g_mgr.FastReturn('f');
				}
			}
			else{
				g.m_menu.CloseOnTopMenu();
				if(g.m_menu.IsMenuPopuped()){
					g.m_menu.CloseMenu();
				}else{
					g.m_menu.CloseAlarmMenu();
					g.m_menu.CloseSetupMenu();
					if(g_pDefFocus){
						g_pDefFocus->take_focus();
					}
				}
				g_mgr.PrintWave(g_uMainId);
			}
			break;

		case FL_F+6:case 'f':    //系统菜单
			if(g_st[0].GetStateData(S_STANDBY)==1){
				EndStandbyMode();
				break;
			}
			g_uShowVersion = 0;
			if(5 == g_uShowLangWin){
				g_uShowLangWin++;
                g_uDebugWinConuntDown = 20 ;
			} else {
                g_uDebugWinConuntDown = 0 ;
            }
			EndNightMode();//结束夜间模式

            if(g.m_menu.IsDialogOnShow()||
                g_mgr.GetDialogType()==DIALOG_TYPE_ST_REPLAY ||
                g_mgr.GetDialogType()==DIALOG_TYPE_STFXM ||
                g_mgr.GetDialogType()==DIALOG_TYPE_QS_SHOW ||
                g_mgr.GetDialogType()==DIALOG_TYPE_VPC_REPLAY){
                g_st[0].SetStateData(S_STARTUP_PATIENT_INFO ,false) ;
				g.m_menu.FastReturn('f');
				g_mgr.FastReturn('f');
			}
			else{
				g.m_menu.CloseOnTopMenu();
				if(g.m_menu.IsMenuPopuped()){
					g.m_menu.CloseMenu();
				}else{
					g.m_menu.CloseAlarmMenu();
					g.m_menu.CloseSetupMenu();

					if(g_pDefFocus){
						g_pDefFocus->take_focus();
					}
					g.m_menu.PopupMenu((ALIGN_WINDOW_TYPE == INFOWIN_ALIGN_LEFT) ?
						(SHOW_INFO_WIDTH + 2) : (MAIN_WINDOW_LEFT + 2),
						MAIN_WINDOW_TOP + MAIN_WINDOW_HEIGHT - MAIN_MENU_HEIGHT, g_uMainId);
				}
			}
			break;
        case FL_F+7: {  //f7键
            g_st[0].SetStateData(S_SHOW_MODE_BIGFONT,!g_st[0].GetStateData(S_SHOW_MODE_BIGFONT)) ;
            printf ("FL_F+7\n") ;
            break ;
        }
		case 'h':case 'H'://第七键
			if(g_st[0].GetStateData(S_STANDBY)==1){
				EndStandbyMode();
				break;
			}
			EndNightMode();//结束夜间模式
			break;
		case 'i':case 'I':// 关机
            printf("received closed I Command... \n");
            if (g_st[0].GetStateData(S_DEMO)==1) {
                g_st[0].SetStateData(S_DEMO,0) ;
            }
#ifdef VITAS_HARDWARE_PLATFORM
            vitasQuitSys() ;
#else
			QuitSys();
#endif
			break;

        case 'j':case 'J':// VITAS 开机
#ifdef VITAS_HARDWARE_PLATFORM
            //清除病人旧数据
            g_mgr.VitasStartupCleanData() ;
            Fl::add_timeout( 5,vitasResetState );
#else
#endif
            break;

		case FL_F+12:
			if(g_st[0].GetStateData(S_STANDBY)==1){
				EndStandbyMode();
				break;
			}
			g_mgr.AnalyseCmd(g_uMainId, S_CLOSE_PC);
			break;

		case FL_Left:case '.':
			g_mgr.PressKey(FL_Left);
			break;

		case FL_Right:case ',':
			g_mgr.PressKey(FL_Right);
			break;

		case FL_Enter:case 'g':
			if(g_st[0].GetStateData(S_STANDBY)==1){
				EndStandbyMode();
				break;
			}
// 			{ //如果大字体打开了，就关了它
// 				STATE_DATA sd;
// 				sd.id=S_SHOW_MODE_BIGFONT;
// 				g_st[0].GetStateData(&sd,1);
// 				if(sd.data==1){
// 					sd.data=0;
// 				}
// 				g_st[0].SetStateData(&sd,1);
// 			}
			if(!g.m_menu.IsMenuPopuped() && !g.m_menu.IsDialogOnShow() ){
				if(g_st[0].GetStateData(S_SHOW_MODE_SPO2HR)==1){//spo2hr打开了就关了
					g_st[0].SetStateData(S_SHOW_MODE_SPO2HR,0);
				}
			}
			g_mgr.PressKey(Fl::event_key());
			break;

		case '0':
			g_mgr.Get_Kb_Data(0x0B);
			g_key_data_num++;
			break;

		case '1':
			g_mgr.Get_Kb_Data(0x02);
			g_key_data_num++;
			break;

		case '2':
			g_mgr.Get_Kb_Data(0x03);
			g_key_data_num++;
			break;

		case '3':
			g_mgr.Get_Kb_Data(0x04);
			g_key_data_num++;
			break;

		case '4':
			g_mgr.Get_Kb_Data(0x05);
			g_key_data_num++;
			break;

		case '5':
			g_mgr.Get_Kb_Data(0x06);
			g_key_data_num++;
			break;

		case '6':
			g_mgr.Get_Kb_Data(0x07);
			break;


//#ifdef UT_OPEN_TEST
//		case 'j':
//			g_mgr.CloseJob(true);//关闭任务巢
//			break;
//
//		case 'k':
//			g_mgr.CloseJob(false);//打开任务巢
//			break;

		case 'n':
			g.m_menu.PoPupIbpTl((ALIGN_WINDOW_TYPE==INFOWIN_ALIGN_LEFT)?
				(SHOW_INFO_WIDTH+(MAIN_WINDOW_WIDTH-SHOW_INFO_WIDTH-120)/2):
			((MAIN_WINDOW_WIDTH-SHOW_INFO_WIDTH-120)/2),
				(MAIN_WINDOW_HEIGHT-85)/2,260,85,0,1);
			break;
//		case 'o':
//			g_mgr.SetShowTestTime(0,true);
//			break;
//		case 'p':
//			g_mgr.SetShowTestTime(0,false);
//			break;
//		case 'q':
//			g_mgr.SetPatientInfo(g_uMainId);
//			break;
		case 'r':
			//Nibp list add
			g_mgr.AppendNibpItem(0);
			break;
		case 's':
			//向打包板请求数据
			dbout("main: Ask Init Data From Board...");
			g_mgr.AnalyseCmd(g_uMainId,S_ASK_SAVE_DATA);
			break;
		case 't':
			//设置硬件缺省数据
			bool bStart,bEnd;
			bStart=false;
			bEnd=false;
			for(int i=0;i<BED_NUMBER;i++) {
				if(i==0){
					bStart=true;
				}else{
					bStart=false;
				}
				if(i==BED_NUMBER-1){
					bEnd=true;
				}else{
					bEnd=false;
				}
				g_st[i].SetDefaultToHal(bStart,bEnd);
			}
			break;
		case 'u':
			if(!g.m_menu.IsMenuPopuped() && !g.m_menu.IsDialogOnShow()){
				g.m_menu.CloseAlarmMenu();
				g.m_menu.PopupSetupMenu((ALIGN_WINDOW_TYPE == INFOWIN_ALIGN_LEFT) ?
					(SHOW_INFO_WIDTH + 2) : (MAIN_WINDOW_LEFT + 2),
					MAIN_WINDOW_TOP + MAIN_WINDOW_HEIGHT - MAIN_MENU_HEIGHT, 0);
			}
			break;
		case 'v':
			//用缺省值重写工作区
			dbout("main: Use Default Value to Write Work Area!");
			g_mgr.AnalyseCmd(g_uMainId,S_W_DEFAULT_TO_WORK);
			break;

		case 'w'://演示模式
			{
				STATE_DATA sd = {S_DEMO, 1};
				if(0 == g_st[0].GetStateData(S_DEMO)){
					sd.data = 1;
					g_st[0].SetStateData(&sd,1);
				}
				else{
					sd.data = 0;
					g_st[0].SetStateData(&sd,1);
				}
			}
			break;

		case 'x':
			m_bStopDebug=!m_bStopDebug;
			g.m_DugWin.SetStopScreen(m_bStopDebug);
			break;

		case 'y':
			{
				CO_HD_REC chr;
				chr.tm = time(0);
				chr.dbCo = 12.8;
				chr.uHeight = 168;
				chr.dbWeight = 56.7;
				chr.uHr = 60;
				chr.iMap = 120;
				chr.iCvp = 110;
				chr.iPam = 100;
				chr.iPaw = 90;
				chr.iPad = 80;
				chr.iLa = 130;
				chr.dbBsa = 21.5;
				chr.dbCI = 12.9;
				chr.dbSv = 18.9;
				chr.dbSvi = 56.8;
				chr.dbSvr = 56.9;
				chr.dbSvri = 23.5;
				chr.dbPvr = 11.1;
				chr.dbPvri = 14.6;
				chr.dbLvswi = 15.7;
				chr.dbRvswi = 34.6;

				g_st[0].SetMgrCmd(MSG_PRINT_DTXYJC, 0, ((LPARAM)&chr));
			}
			break;

		case 'z':
			{
				char g_resp_demo_data[]={
					42,44,45,45,47,48,48,48,49,49,51,51,51,52,52,54,54,54,54,54,54,54,54,55,55,55,55,55,57,57,57,57,57,57,57,57,58,58,58,58,58,60,60,60,60,60,60,60,60,60,
					60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,58,55,54,51,48,45,42,39,34,32,26,23,19,
					18,15,12,11, 9, 8, 6, 5, 5, 5, 5, 5,5, 5, 5, 5, 6, 6, 8, 9, 11,12,13,16,18,19,19,20,22,22,22,22,22,22,22,20,19,19,19,19,20,20,22,22,22,23,25,25,26,29,
					30,32,33,34,35,36,38,39,41,42,42,44,45,45,47,48,48,48,49,49,51,51,51,52,52,54,54,54,54,54,54,54,54,55,55,
					55,55,55,57,57,57,57,57,57,57,57,58,58,58,58,58,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
					60,60,60,60,60,60,60,60,60,60,60,60,58,55,54,51,48,45,42,39,34,32,26,23,19,18,15,12,11, 9, 8, 6, 5, 5, 5, 5, 5,5, 5, 5, 5, 6, 6, 8, 9, 11,12,13,16,18,
					19,19,20,22,22,22,22,22,22,22,20,19,19,19,19,20,20,22,22,22,23,25,25,26,29,30,32,33,34,35,36,38,39,41,42,
					42,44,45,45,47,48,48,48,49,49,51,51,51,52,52,54,54,54,54,54,54,54,54,55,55,55,55,55,57,57,57,57,57,57,57,57,58,58,58,58,58,60,60,60,60,60,60,60,60,60,
					60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,58,55,54,51,48,45,42,39,34,32,26,23,19,
					18,15,12,11, 9, 8, 6, 5, 5, 5, 5, 5,5, 5, 5, 5, 6, 6, 8, 9, 11,12,13,16,18,19,19,20,22,22,22,22,22,22,22,20,19,19,19,19,20,20,22,22,22,23,25,25,26,29,
					30,32,33,34,35,36,38,39,41,42,42,44,45,45,47,48,48,48,49,49,51,51,51,52,52,54,54,54,54,54,54,54,54,55,55,
					55,55,55,57,57,57,57,57,57,57,57,58,58,58,58,58,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
					60,60,60,60,60,60,60,60,60,60,60,60,58,55,54,51,48,45,42,39,34,32,26,23,19,18,15,12,11, 9, 8, 6, 5, 5, 5, 5, 5,5, 5, 5, 5, 6, 6, 8, 9, 11,12,13,16,18,
					19,19,20,22,22,22,22,22,22,22,20,19,19,19,19,20,20,22,22,22,23,25,25,26,29,30,32,33,34,35,36,38,39,41,42,
					42,44,45,45,47,48,48,48,49,49,51,51,51,52,52,54,54,54,54,54,54,54,54,55,55,55,55,55,57,57,57,57,57,57,57,57,58,58,58,58,58,60,60,60,60,60,60,60,60,60,
					60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,58,55,54,51,48,45,42,39,34,32,26,23,19,
					18,15,12,11, 9, 8, 6, 5, 5, 5, 5, 5,5, 5, 5, 5, 6, 6, 8, 9, 11,12,13,16,18,19,19,20,22,22,22,22,22,22,22,20,19,19,19,19,20,20,22,22,22,23,25,25,26,29,
					30,32,33,34,35,36,38,39,41,42,42,44,45,45,47,48,48,48,49,49,51,51,51,52,52,54,54,54,54,54,54,54,54,55,55,
					55,55,55,57,57,57,57,57,57,57,57,58,58,58,58,58,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
					60,60,60,60,60,60,60,60,60,60,60,60,58,55,54,51,48,45,42,39,34,32,26,23,19,18,15,12,11, 9, 8, 6, 5, 5, 5, 5, 5,5, 5, 5, 5, 6, 6, 8, 9, 11,12,13,16,18,
					19,19,20,22,22,22,22,22,22,22,20,19,19,19,19,20,20,22,22,22,23,25,25,26,29,30,32,33,34,35,36,38,39,41,42,
					42,44,45,45,47,48,48,48,49,49,51,51,51,52,52,54,54,54,54,54,54,54,54,55,55,55,55,55,57,57,57,57,57,57,57,57,58,58,58,58,58,60,60,60,60,60,60,60,60,60,
					60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,58,55,54,51,48,45,42,39,34,32,26,23,19,
					18,15,12,11, 9, 8, 6, 5, 5, 5, 5, 5,5, 5, 5, 5, 6, 6, 8, 9, 11,12,13,16,18,19,19,20,22,22,22,22,22,22,22,20,19,19,19,19,20,20,22,22,22,23,25,25,26,29,
					30,32,33,34,35,36,38,39,41,42,42,44,45,45,47,48,48,48,49,49,51,51,51,52,52,54,54,54,54,54,54,54,54,55,55,
					55,55,55,57,57,57,57,57,57,57,57,58,58,58,58,58,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
					60,60,60,60,60,60,60,60,60,60,60,60,58,55,54,51,48,45,42,39,34,32,26,23,19,18,15,12,11, 9, 8, 6, 5, 5, 5, 5, 5,5, 5, 5, 5, 6, 6, 8, 9, 11,12,13,16,18,
					19,19,20,22,22,22,22,22,22,22,20,19,19,19,19,20,20,22,22,22,23,25,25,26,29,30,32,33,34,35,36,38,39,41,42
				};

				CO_M_REC cmr;
				cmr.uRecSec = 60;
				memcpy((void*)cmr.btData, (void*)g_resp_demo_data, MAX_CO_REC_TIME * CO_WAVE_DATA_PERSEC);
				cmr.tm = time(0);
				cmr.dbCo = 12.3;
				cmr.dbTb = 0.25;
				cmr.uZsl = 6;
				cmr.dbIT = 1.13;
				cmr.dbCI = 21;
				cmr.dbConst = 0.0009;

				g_st[0].SetMgrCmd(MSG_PRINT_CO, 0, ((LPARAM)&cmr));
			}
			break;
		case 'm':
			g_displang.PopupModal(NULL, 0, 0);		//ccz_2009_03_11 语言字符串显示界面
			g_displang.ReFresh();
			break;


//#endif//UT_OPEN_TEST

		default:
			break;
		}
	}else{
		switch(e){
		case FL_SHOW:
			g.m_pMainWin->take_focus();
			return 0;
		case FL_MOVE:case FL_ENTER:
			return 1;
		default:
			break;
		}
	}

	//if(e != 0){
	//    dbout1("--------- %d -----------",e);
	//}
	return e;
}

//结束夜间模式
void EndNightMode(){
	if(g_st[0].GetStateData(S_STANDBY)==0){
		g.m_menu.CloseAlarmMenu();
		if(g_mgr.IsNightMode()){
			g_mgr.CloseNightMode(true);
		}
	}
}

void EndStandbyMode(){
	//g_st[0].SetStateData(S_DEMO, 0);

	//如果待机前是演示模式，则其还原为正常模式，此时清空 所有数据
	if(1==g_st[0].GetStateData(S_DEMO))
	{
		g_st[0].SetStateData(S_DEMO, 0);
		g_st[0].SetStateData(S_CLEAR_OLD_DATA,0);
	}

	//否则不清空数据
	//g_st[0].SetStateData(S_CLEAR_OLD_DATA,0);
	g_st[0].SetStateData(S_NIBP_DTXYJC_OPEN,0,true);
	g_st[0].SetStateData(S_ECG_HRVA_CLEAR,0);
	//g_st[0].SetStateData(S_DATASAVE_CLEAR,0,true);

	if(b_standby_data_mgr){
		b_standby_data_mgr=false;
		g_st[0].SetStateData(S_SHOW_DATA_MGR_CEN, 1);
	}

	g_st[0].SetStateData(S_STANDBY,0);
	g.m_menu.AutoAlarmUpdateState(S_YJMS,0);
	g_mgr.SetSpecialSort(true);
	g_st[0].SetStateData(S_SORT_TD, 0);
	g_mgr.SetSpecialSort(false);
	m_standby_win->hide();
}

/*
初始化类
*/
void InitClass(){

	dbout("main: Init Class...");
	assert(BED_NUMBER<=2);

	int iHalStyle[2/*BED_NUMBER*/];
	iHalStyle[0]=CHAL_MAIN_STYLE;
	g_uMainId=0;

	if(2/*BED_NUMBER*/>1){
		iHalStyle[1]=CHAL_COMP_STYLE;
		assert(iHalStyle[1]!=iHalStyle[0]);
	}

	g_mgr.CreateHalObject(iHalStyle);
	g_mgr.CreateBedObject();

	for(int i=0;i<BED_NUMBER;i++) {
		g_st[i].SetMgrObject(&g_mgr);
		g_st[i].SetBedId(i);
	}

	g_mgr.InitHal(false);
}

/*
得到保存的数据
*/
void GetSaveData(void *){

	dbout("main: Get Save Data...");

	g_st[0].SetStateData(S_STARTUP_PATIENT_INFO ,true) ;

	//从文件中读取
	if(SAVE_TYPE==SAVE_TO_FILE){

		if(FORCE_WRITE_DATA_TO_BOARD!=0){
			dbout("main: Write Default Data to Board.");

			for(int i=0;i<BED_NUMBER;i++) {
				g_st[i].LoadAllSaveData(true, false);
			}

			WriteDefaultData();
		}

		if(!g_bGetIniData){

			for(int i=0;i<BED_NUMBER;i++) {
				g_st[i].LoadAllSaveData(0 == FORCE_WRITE_DATA_TO_BOARD, true);
			}

			SendAskToHal(0);
			Fl::flush();

			g_bGetIniData=true;
		}

		Init(0);

	}
	else if(SAVE_TYPE==SAVE_TO_HAL){

		//测试用，打包板第一次初始化时调用，这里用于强制写入
		if(FORCE_WRITE_DATA_TO_BOARD!=0){
			dbout("main: Write Default Data to Board.");
			WriteDefaultData();
		}
		else{
			//向打包板申请初始化数据
			SendAskToHal(0);
		}

	}
	else{
		dbout("main: Can not find save type!use default value...");
		//set default value
		if(!g_bGetIniData){
			g_bGetIniData=true;

			for(int i=0;i<BED_NUMBER;i++) {
				g_st[i].SetDefaultValue(S_DEF_SETUP_TYPE_1,false,true);
			}
		}

		Init(0);

	}
 //   g_st[0].SetStateData(S_STARTUP_PATIENT_INFO ,true) ;
}

void WriteDefaultData(){

	bool bStart,bEnd;
	bStart=false;
	bEnd=false;
	for(int i=0;i<BED_NUMBER;i++) {
		if(i==0){
			bStart=true;
		}else{
			bStart=false;
		}
		if(i==BED_NUMBER-1){
			bEnd=true;
		}else{
			bEnd=false;
		}
		g_st[i].SetDefaultToHal(bStart,bEnd);
	}

	//等一会后向打包板申请初始化数据
	Fl::add_timeout(5,SendAskToHal);
}

/*
向硬件请求保存数据
*/
void SendAskToHal(void *){
	if(!g_bGetIniData){
		dbout("main: Ask Init Data From Board...");
		g_mgr.AnalyseCmd(g_uMainId,S_ASK_SAVE_DATA);
		Fl::add_timeout(0.1,Flush_com);
		Fl::add_timeout(NO_DATA_WAIT_TIME,Ignore_Err);
	}
}

/*
硬件向系统发送保存数据过程中刷新串口
*/
void Flush_com(void *){

	if(!g_bGetIniData) {
		//dbout("main: Fl::flush()");
		Fl::flush();
		Fl::repeat_timeout(0.1,Flush_com);
	}
	else{
		Fl::remove_timeout(Flush_com);
	}
}

/*
等待硬件向系统发送保存数据
若一定时间后还没有保存数据，则忽略错误，用缺省值启动程序
*/
void Ignore_Err(void *){

	if(!g_bGetIniData){

		dbout("main: Ignore Err and Start Program...");
		g_bGetIniData = true;

		//取得缺省值
		for(int i=0;i<BED_NUMBER;i++) {
			g_st[i].SetDefaultValue(S_DEF_SETUP_TYPE_1,false,true);
		}

		//go on
		Fl::remove_timeout(Ignore_Err);
		Fl::add_timeout(0.5,Init);
	}
	else{
		Fl::remove_timeout(Ignore_Err);
	}
}

/*
硬件上的保存数据到达后，通知系统的回调函数
*/
void GetDataFromHal(bool bOk){

	if(bOk){
		if(!g_bGetIniData){

			//检测数据的有效性
			//如果数据有效则启动应用程序
			if(IsDataValidity()){

				//获得保存数据有效
				dbout("main: Get Save Data From Hal OK.");
				g_bGetIniData = true;

				dbout("main: Fl::remove_timeout(Ignore_Err)");
				Fl::remove_timeout(Ignore_Err);

				dbout("main: Fl::add_timeout(0.5,Init);");
				Fl::add_timeout(0.5,Init);
			}
			//否则视为系统已经升级，强制写入缺省值
			else{
				dbout(" ");
				dbout("Warning!!!");
				dbout("Data is invalid because of not the same version.");
				dbout("System Now Auto Update program.");
				dbout("main: Write Default Data to Board By Force.");
				WriteDefaultData();
			}
		}
		else{
			//保存数据到达太迟，或程序运行当中得到硬件来的保存数据
			dbout("main: Get Save Data From Hal OK.But Not Use it");
			dbout("main: Because Time is Out!");
		}
	}
	else{
		dbout("main: Get Save Data From Hal Fail!");

		if(!g_bGetIniData){
			//取得缺省值
			dbout("main: Get Default Value...");
			for(int i=0;i<BED_NUMBER;i++) {
				g_st[i].SetDefaultValue(S_DEF_SETUP_TYPE_1,false,true);
			}

			g_bGetIniData=true;
			Fl::add_timeout(0.5,Init);
		}
	}
}

/*检测得到的数据是否有效*/
bool IsDataValidity(){

	dbout("main: start check data...");

	//检测版本号是否一致
	if(CUR_VERSION_SIGN != g_st[0].GetStateData(S_VERSION_SIGN)){
		return false;
	}

	//检测其他内容是否一致
	//...

	dbout("main: check ok.");

	return true;
}

/*
系统初始化入口
*/
void Init(void *){

	dbout("main: Set Language...");
	g.m_Trace.RecLog("main: Set Language...", 0);
	//printf("main: Set Language...\n");
//	//取得语言版本
//	STATE_DATA sd={S_LANGUAGE_VISION_USR,0};
//	g_st[0].GetStateData(&sd,1);
//	g.SetLanguageVision(sd.data);

	//init mgr

	STATE_DATA screen={S_12_SCREEN,0};
	g_st[0].GetStateData(&screen,1);
	/*
	系数定义由 SPEED_POINT_0不同分辨率下比值决定！
	g_dTmIntval=m_uTimeBs(max)/SPEED_POINT_0
	TIMER_PER_SECOND=SPEED_POINT_0/m_uTimeBs(max)
	*/


//点距         0.297*0.297 
//长宽         304.128*228.096
//像素         1024*768

#ifdef MODE_1024X768
#ifdef G80_NEW_HARDWARE_PLATFORM
	if(screen.data==0){//10。4寸
        g.g_timer_per_second = 31 ;
		g_timer_per_second=31;
		g_dTmIntval	=	0.032;
		g_bBigScreen=false;
		SPEED_POINT_0=246;
		SPEED_POINT_1=123;
		SPEED_POINT_2=61;
		SPEED_POINT_3=31;
		printf("1024 10 Screen Mode\n");
	}
	else{
		g_bBigScreen=true;
		g_dTmIntval	=	0.0476;
        g.g_timer_per_second = 21 ;
		g_timer_per_second=21;
		SPEED_POINT_0=168;
		SPEED_POINT_1=84;
		SPEED_POINT_2=42;
		SPEED_POINT_3=21;
		printf("1024 12 Screen Mode\n");
	}
#else
	g_dTmIntval	=	0.04;
    g.g_timer_per_second = 25 ;
	g_timer_per_second=25;
	SPEED_POINT_0=200;
	SPEED_POINT_1=100;
	SPEED_POINT_2=50;
	SPEED_POINT_3=25;
#endif
#else
//点距         0.264*0.264 
//长宽         337.920*270.336
//像素         1280*1024
#ifdef MODE_1280X1024
		g_dTmIntval	=	0.0423;
        g.g_timer_per_second = 24 ;
		g_timer_per_second=24;
		g_bBigScreen=true;
		SPEED_POINT_0=189;
		SPEED_POINT_1=94;
		SPEED_POINT_2=47;
		SPEED_POINT_3=24;
		printf("Big Screen Mode\n");
#else
//点距         0.3075*0.3075 
//长宽         246*184.5
//像素         800*600
	if(screen.data==0){
        g.g_timer_per_second = 24 ;
		g_timer_per_second=24;
		g_dTmIntval	=	0.0416;
		g_bBigScreen=false;
		SPEED_POINT_0=192;
		SPEED_POINT_1=96;
		SPEED_POINT_2=48;
		SPEED_POINT_3=24;
		printf("Small Screen Mode\n");
	}
	else{
		g_dTmIntval	=	0.05;
        g.g_timer_per_second = 20 ;
		g_timer_per_second=20;
		g_bBigScreen=true;
		SPEED_POINT_0=160;
		SPEED_POINT_1=80;
		SPEED_POINT_2=40;
		SPEED_POINT_3=20;
		printf("Big Screen Mode\n");
	}
#endif
#endif
	//double(1)/(double)SPEED_POINT_2;

	//init
	dbout("main: Init...");
	g.m_Trace.RecLog("main: Init...", 0);
	//printf("main: Init...\n");
#ifdef	G80_NEW_HARDWARE_PLATFORM
	//Fl::add_timeout(G80_BOOT_APP_TIME+1,G80_boot);
#else
	g_mgr.init();
	/*
	创建任务巢
	create jobnest
	*/
	dbout("main: Create Jobnest...");

#ifndef WIN32
	g.m_pjnState=GetJobNest();
	assert(g.m_pjnState);
	g.m_pjnNet=GetJobNest();
	assert(g.m_pjnNet);
#endif//WIN32

	//hide mouse
	dbout("main: Hide Mouse...");
	if(!g_st[0].GetStateData(S_CMP)){
		g.m_pMainWin->cursor(FL_CURSOR_NONE,FL_RED,FL_RED);
	}

	//keyboard handle
	dbout("main: Add Handler...");
	Fl::add_handler(handle);

	//timer
	dbout("main: Add Timer...");
	if(SAVE_TYPE==SAVE_TO_HAL){
		Fl::remove_timeout(Init);
	}

	//----------------------------------------------------
	//temp
	//临时
	//if(SAVE_TYPE==SAVE_TO_FILE){
	//dbout("main: Ask Init Data From Board...");
	//g_mgr.AnalyseCmd(g_uMainId,S_ASK_SAVE_DATA);
	//}
	//----------------------------------------------------
#ifndef G80_NEW_HARDWARE_PLATFORM
	g_mgr.SetChannelFocus();

#endif
	Fl::add_timeout(0,SendTimer);
	dbout("main: Program Start OK!");
#endif
}

void SendKbTimer(void *p){
	char tmp[50]={0};
	STATE_DATA sd;
	sd.id=S_LC_MODE;
	g_st[0].GetStateData(&sd,1);

	if(g_key_data_num<1 || g_key_data_num>LC_MODE_DETECT_TIME){		//键盘口初始化不对
		if(sd.data){
			sd.data=0;
            if ( CHANNEL_12ECG == g_st[0].GetStateData(S_COM02)) {
                g_st[0].SetStateData(S_COM02, CHANNEL_12ECG);
            } else if(CHANNEL_PHILIPS_ECG ==  g_st[0].GetStateData(S_COM02)){
                g_st[0].SetStateData(S_COM02, CHANNEL_PHILIPS_ECG);
	    }else{
                g_st[0].SetStateData(S_COM02, CHANNEL_ECG);
            }
			g_st[0].SetStateData(S_ZYT_TX_MS, SYS_TXMS_WK);
			g_st[0].SetStateData(S_NURCALL_SWITCH, 1);
			g_st[0].SetStateData(S_ECG_SCKG, 1);
			g_st[0].SetStateData(S_TEMP_TWGL, 1);

			g_st[0].SetStateData(S_ZYT_TX_MS_CH, SYS_TXMS_NONE);
			g_st[0].SetStateData(S_12_SCREEN, 1);
			g_st[0].SetStateData(S_SHOW_DATA_MGR_CEN, 1);
		}else{
			sd.data=1;
			g_st[0].SetStateData(S_COM02, CHANNEL_ECG_SPO2);
			g_st[0].SetStateData(S_ZYT_TX_MS, SYS_TXMS_WK);
			g_st[0].SetStateData(S_ZYT_TX_MS_CH, SYS_TXMS_NONE);
			g_st[0].SetStateData(S_NURCALL_SWITCH, 0);
			g_st[0].SetStateData(S_ECG_SCKG, 0);
			g_st[0].SetStateData(S_TEMP_TWGL, 0);
			g_st[0].SetStateData(S_12_SCREEN, 0);
			g_st[0].SetStateData(S_SHOW_DATA_MGR_CEN, 0);
		}
		g_st[0].SetStateData(&sd, 1);
		//弹出对话框
		sprintf(tmp, "%s %d", g.GetTxt(TXT_REBOOT), g_key_data_num);
		pLcInfoBox->SetLabel(tmp);
		pLcInfoDialog->Popup(NULL, 300, 270);
	}
}

//退出系统
void QuitSys(){
        g_mgr.AnalyseCmd(g_uMainId, S_CLOSE_TIME_OUT);
	g_mgr.QuitSaveData();
       sync();
	if(g_st[0].GetStateData(S_STANDBY)==1){
		if(b_standby_data_mgr){
			b_standby_data_mgr=false;
			g_st[0].SetStateData(S_SHOW_DATA_MGR_CEN, 1);
		}
	}
	EndNightMode();//结束夜间模式
	dbout("main: Close Sound.");
	g.m_Trace.RecLog("main: Close Sound.", 0);
	g_mgr.AnalyseCmd(g_uMainId,S_CLOSE_SOUND);
	dbout("main: Close LED.");
	g.m_Trace.RecLog("main: Close LED.", 0);
	g_mgr.AnalyseCmd(g_uMainId,S_CLOSE_LED);

#ifdef G80_NEW_HARDWARE_PLATFORM
	dbout("main: Close DEV_LED.");
	g_mgr.AnalyseCmd(g_uMainId,S_CLOSE_DEV_LED);
#endif

	dbout("main: Close Menu.");
	g.m_Trace.RecLog("main: Close Menu.", 0);
	g.m_menu.CloseAlarmMenu();
	g.m_menu.CloseSetupMenu();
	if(g.m_menu.IsMenuPopuped()){
		g.m_menu.CloseMenu();
	}

	dbout("main: Remove Timer.");
	g.m_Trace.RecLog("main: Remove Timer.", 0);
	Fl::remove_timeout(SendTimer);
	g_mgr.end();

#ifndef WIN32
	if(g.m_pjnState){
		dbout("main: Release JobNest.");
		ReleaseJobNest(g.m_pjnState);
	}
	if(g.m_pjnNet){
		dbout("main: Release JobNest.");
		ReleaseJobNest(g.m_pjnNet);
	}
	if(g.m_pjnObs){
		dbout("obs: Release JobNest.");
	}
	if(g.m_pjnObsCom){
		dbout("ObsCom: Release JobNest.");
	}
#endif//WIN32

	dbout("Umount Save Device...");
	fflush(NULL);

#ifndef WIN32
	sync();
#endif//WIN32
	g.m_usbfile.UnLoadUsbDevice();
	if(g.m_usbfile.IsMountSaveDevice()){
		if(g.m_usbfile.UnLoadSaveDevice()){
			dbout("Umount Save Device...OK");
			g.m_Trace.RecLog("Umount Save Device...OK", 0);
		}
		else{
			dbout("Umount Save Device...Fail");
			g.m_Trace.RecLog("Umount Save Device...Fail", 0);
		}
	}
	else{
		dbout("Umount Save Device...OK");
		g.m_Trace.RecLog("Umount Save Device...OK", 0);
	}
	fflush(NULL);
	printf("Quit sys exit\n");
	g.m_Trace.RecLog("Quit sys exit", 0);
	g.m_pMainWin->hide();
#ifndef WIN32
	sync();
#endif//WIN32

	printf("Send Close PC cmd.");

    g_mgr.AnalyseCmd(g_uMainId, S_CLOSE_PC);


	g_bQuit = true;
	printf("main: Quit OK.");
	g.m_Trace.RecLog("main: Quit OK.", 0);

    printf ("exit(1)\n") ;
	exit(1);

}
void vitasQuitSys () {
    g_mgr.QuitSaveData();
    if(g_st[0].GetStateData(S_STANDBY)==1){
        if(b_standby_data_mgr){
            b_standby_data_mgr=false;
            g_st[0].SetStateData(S_SHOW_DATA_MGR_CEN, 1);
        }
    }
    g_mgr.AnalyseCmd(g_uMainId,S_CLOSE_SOUND);
    g_mgr.AnalyseCmd(g_uMainId,S_CLOSE_LED);
    g_mgr.AnalyseCmd(g_uMainId,S_CLOSE_DEV_LED);
    g.m_menu.CloseAlarmMenu();
    g.m_menu.CloseSetupMenu();
    if(g.m_menu.IsMenuPopuped()){
        g.m_menu.CloseMenu();
    }
    g_mgr.AnalyseCmd(g_uMainId, S_CLOSE_PC);
}
void vitasResetState(void *) {//void            vitasResetState(void *) ;
#ifdef G80_NEW_HARDWARE_PLATFORM	
    g_st[0].G80_ReFresh() ;
#endif
}
//end of file


