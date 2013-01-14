/*================================================================
	file:	rktools.h
	by:		Rick Wang
	date:	2002/4
	discription:
		本文件定义了一些在LINUX下使用的工具
================================================================*/
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

class CHighClock{
private:
	struct timeval m_tstart;
	struct timeval m_tend;
	struct timezone m_tz;
public:
	CHighClock(){
		Start();
	}
	~CHighClock(){
	}

	//开始计时
	void Start(){
		gettimeofday(&m_tstart,&tz);
	}

	//停止计时
	void Stop(){
		gettimeofday(&m_tend,&tz);
	}

	//获取运行时间
	double getval(){
		double t1,t2;
		t1=(double)m_tstart.tv_sec+(double)m_tstart.tv_usec/(1000*1000);
		t2=(double)m_tend.tv_sec+(double)m_tend.tv_usec/(1000*1000);
		return t2-t1;
	}

	void printval(){
		printf("High-Clock: run time = %.6f sec.\n", getval());
	}
};
