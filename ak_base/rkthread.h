/*****************************************************************************\
	module:	
	file:	rkthread.h
	by:		Rick Wang
	date:	2002/03
	description:
		相关文件rkthread.cpp，这两个文件用面向对象方式封装了Linux下pthread线程
		库的部分功能。目前的内容有：

	1.互斥类
	2.同步事件类
	3.线程延时器类
	-----------------------------------------------------------------------
	modification records:
	1.	by: ?			date: ?
		description: ?
	2.
\*****************************************************************************/
#ifndef RKTHREAD_H_A9B49AA5_3E20_47e3_8142_E8CA6F3F7A0B
#define RKTHREAD_H_A9B49AA5_3E20_47e3_8142_E8CA6F3F7A0B

#include <pthread.h>
#include "rkTypes.h"

class CEvent;
class CDelayer;

/*
	线程互斥类
*/
class CMutex{
protected:
	pthread_mutex_t				m_lock;			//互斥锁
	pthread_t					m_InsideThread;	//当前在临界区中的线程ID
	UINT						m_uLockCount;	//一个线程的上锁调用次数（从而确定所需的解锁调用次数）
	
	pthread_mutex_t* GetMutexPtr(){return &m_lock;}

public:
	CMutex();
	~CMutex();

	int Lock();									//上锁
	int TryLock();								//尝试上锁
	int UnLock();								//开锁
	
	friend class CEvent;
	friend class CDelayer;
};//end of class CMutex

/*
	线程同步事件类(可以用做互斥量)
*/
class CEvent{
protected:
	pthread_cond_t				m_cond;
	pthread_t					m_MutexInsideThread;
	UINT						m_uMutexLockCount;

	CMutex						m_lock;
public:
	CEvent();
	~CEvent();

	/*暴露CMutex的接口*/
	int Lock();									//上锁
	int TryLock();								//尝试上锁
	int UnLock();								//开锁

	int Wait();									//等待
	int TimeWait(long lSec, long lNSec);		//定时等待（绝对时间，lSec:秒, lNSec:纳秒）

	int SetEvent();
};//end of class CEvent

class CDelayer:public CEvent{
protected:
public:
	CDelayer();
	~CDelayer();
	
	void Delay(ulong ms);						//令线程等待相对长度的时间（毫秒）
};

#endif //#ifndef RKTHREAD_H_A9B49AA5_3E20_47e3_8142_E8CA6F3F7A0B
