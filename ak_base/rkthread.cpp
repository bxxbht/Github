/*****************************************************************************\
	module:	
	file:	rkthread.cpp
	by:		Rick Wang
	date:	2002/03
	description:
		����ļ�rkthread.h���������ļ����������ʽ��װ��Linux��pthread�߳�
		��Ĳ��ֹ��ܣ����ṩ�߳���ʱ������չ����
	-----------------------------------------------------------------------
	modification records:
	1.	by: ?			date: ?
		description: ?
	2.
\*****************************************************************************/
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include "rkthread.h"
#include <assert.h>

/*
	�̻߳�����
*/
CMutex::CMutex(){
	int iRetVal=pthread_mutex_init(&m_lock,NULL);
	assert(SUCCESS==iRetVal);

	m_InsideThread=NULL;
	m_uLockCount=0;
}

CMutex::~CMutex(){
	int iRetVal;
	iRetVal=pthread_mutex_destroy(&m_lock);
	assert(SUCCESS==iRetVal);
}

int CMutex::Lock(){
	//ͬһ���̲߳����ڶ�������
	if(m_InsideThread){
		if(pthread_equal(m_InsideThread, pthread_self())){
			m_uLockCount++;
			return SUCCESS;
		}
	}
	int iRetVal=pthread_mutex_lock(&m_lock);
	
	if(SUCCESS==iRetVal){
		m_InsideThread=pthread_self();
		m_uLockCount=1;
	}
	return iRetVal;
}

int CMutex::TryLock(){
	//ͬһ���̲߳����ڶ�������
	if(m_InsideThread){
		if(pthread_equal(m_InsideThread, pthread_self())){
			m_uLockCount++;
			return SUCCESS;
		}
	}

	int iRetVal=pthread_mutex_trylock(&m_lock);
	if(SUCCESS==iRetVal){
		m_InsideThread=pthread_self();
		m_uLockCount=1;
	}
	return iRetVal;
}

int CMutex::UnLock(){
	if(!m_InsideThread){
		return SUCCESS;
	}
	//ֱ��ͬһ���̵߳��������ü���Ϊ0����ʽ����
	if(pthread_equal(m_InsideThread, pthread_self())){
		m_uLockCount--;
		if(!m_uLockCount){
			pthread_t thTemp=m_InsideThread;
			m_InsideThread=NULL;
			int iRetVal=pthread_mutex_unlock(&m_lock);
			if(SUCCESS!=iRetVal){
				m_InsideThread=thTemp;
				m_uLockCount++;
			}
			return iRetVal;
		}
		else{
			assert(m_uLockCount!=0xffffffff);
			return SUCCESS;
		}
	}
	//������ٽ����е��̲߳��ǵ�ǰ�����̣߳��򷵻ش���
	else{
		return ENOLCK;
	}
}

CDelayer::CDelayer(){
}
CDelayer::~CDelayer(){
}
	
void CDelayer::Delay(ulong ms){
	struct timespec ts;
	struct timeval now;
	int ret=0;
	
	gettimeofday(&now,NULL);
	ts.tv_sec=now.tv_sec+(ms/1000);
	ts.tv_nsec=now.tv_usec*1000+(ms%1000)*1000000;
	
	m_lock.Lock();
	while(ETIMEDOUT!=ret){
		ret=pthread_cond_timedwait(&m_cond,m_lock.GetMutexPtr(),&ts);
	}
	m_lock.UnLock();
}


/*
	�߳�ͬ���¼���
*/
CEvent::CEvent(){
	int iRetVal=pthread_cond_init(&m_cond,NULL);
	assert(SUCCESS==iRetVal);
	
	m_MutexInsideThread=NULL;
	m_uMutexLockCount=0;
}

CEvent::~CEvent(){
	int iRetVal=pthread_cond_destroy(&m_cond);
	assert(SUCCESS==iRetVal);
}

int CEvent::SetEvent(){
	int iRetVal=pthread_cond_signal(&m_cond);
	return iRetVal;
}

int CEvent::Wait(){
	int iRetVal;
	m_lock.Lock();
	m_MutexInsideThread=m_lock.m_InsideThread;
	m_uMutexLockCount=m_lock.m_uLockCount;
	
   	iRetVal=pthread_cond_wait(&m_cond,m_lock.GetMutexPtr());
   	
	m_lock.m_InsideThread=m_MutexInsideThread;
	m_lock.m_uLockCount=m_uMutexLockCount;
	m_lock.UnLock();
	return iRetVal;
}

int CEvent::TimeWait(long lSec, long lNSec){
	struct timespec t={lSec,lNSec};
	m_lock.Lock();
	m_MutexInsideThread=m_lock.m_InsideThread;
	m_uMutexLockCount=m_lock.m_uLockCount;
	
	int iRetVal=pthread_cond_timedwait(&m_cond,m_lock.GetMutexPtr(), &t);
	
	m_lock.m_InsideThread=m_MutexInsideThread;
	m_lock.m_uLockCount=m_uMutexLockCount;
	m_lock.UnLock();
	return iRetVal;
}

/*��¶CMutex�Ľӿ�*/
//����
int CEvent::Lock(){
	return m_lock.Lock();
}
//��������
int CEvent::TryLock(){
	return m_lock.TryLock();
}
//����
int CEvent::UnLock(){
	return m_lock.UnLock();
}
