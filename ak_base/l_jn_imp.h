/*-----------------------------------------------------------------------*\
	file:		l_jn_imp.h
	module:		jobnest for linux
	descript:	本模块提供Linux下的任务巢(多线程任务队列)功能,本文件定义了
				模块内部实现所需的类和符号等
	by:			Rick Wang
	date:		2002/3
\*-----------------------------------------------------------------------*/
#ifndef L_JN_IMP_H_2E2076BC_B0C1_4a98_889F_8DAABC69D62E
#define L_JN_IMP_H_2E2076BC_B0C1_4a98_889F_8DAABC69D62E
#include "l_jn.h"
#include "rkthread.h"
#include "l_mpm.h"
#include <string.h>
#include "stdio.h"
#include "assert.h"

#define JP_PRI_LEVELCOUNT			5				//任务优先级数
#define JP_PRI_HEIGHEST				JP_PRI_TOP		//最高优先级
#define JN_SCH_INVALID				0xffffffff		//无效的线程调度优先级

/*声明*/
class CJobNest_imp;

class CJobQueue:virtual public CJobPkg{
private:
protected:
	CJobQueue*		m_pNext;						//下一个任务
	CJobQueue*		m_pPrev;						//上一个任务
public:
	CJobQueue();
	~CJobQueue();

	CJobQueue* GetNext();
	CJobQueue* GetPrev();
	void SetNext(CJobQueue* pNext);
	void SetPrev(CJobQueue* pPrev);

	void InsertBefore(CJobQueue* pjq_after);
	void InsertAfter(CJobQueue* pjq_before);
	void RemoveOut();
};


class CJobPkg_imp:public CJobQueue{
private:
protected:
	CMemPoolInterface*	m_pmpi;						//内存池管理器接口对象指针
	HANDLE				m_hPool;					//对应的内存池对象句柄
	CJobNest_imp*		m_pjn;						//本对象所属的任务巢对象
	UINT				m_uPriroity;				//任务优先级
	UINT				m_uSchedule;				//线程调度优先级
	JOBFUNCTION			m_pExecFunc;				//执行任务的回调函数
	bool				m_bSubmitted;				//标志是否已经提交

	PVOID				m_pvParam;					//用户任务函数的参数

	DWORD				m_dwID;						//任务包ID，由用户指定，
													//相同ID的任务包在任务队
													//列中只能存在1个，除非ID值为0
    	
	void Init(CJobNest_imp* pjn);
public:
	CJobPkg_imp();
	CJobPkg_imp(CJobNest_imp* pjn);
	virtual ~CJobPkg_imp();

	//申请内存块
	virtual PVOID Alloc(SIZE size);
	virtual PVOID Alloc(SIZE size, UCHAR InitVal);

	//释放内存块
	virtual void Free(PVOID pv);

	//设置任务执行时的线程调度优先级属性
	virtual bool SetSchedule(UINT uSch);
	//读取当前任务的线程调度优先级属性
	virtual UINT GetSchedule();
	//设置任务的优先级
	virtual bool SetPriroity(UINT uPri);
	//读取当前任务的优先级
	virtual UINT GetPriroity();


	//设置任务执行函数
	virtual void SetExecFunction(JOBFUNCTION pfun);
	//设置任务函数执行时使用的参数
	virtual void SetExecParam(PVOID pvParam);
	//设置任务包ID，在已经提交的任务队列中相同ID的新任务包将
	//覆盖老任务包（使其被删除），除非ID为0
	//返回原来的ID值
	virtual DWORD SetID(DWORD dwID);
	virtual DWORD GetID();

	virtual void* QueryIF();
	virtual bool IsSubmitted();
	
	void ExecJob();						//执行任务

	void SetJobNest(CJobNest_imp* pjn);
	void SetSubmit(bool bSubmit);
	void* operator new(SIZE size, SIZE initsize, CMemPoolInterface* pmpi);
	void* operator new[](SIZE size){return NULL;};
	void operator delete(void* pv);
	void operator delete[](void* pv){return;};
	void Delete();
};

class CJobNest_imp:virtual public CJobNest{
private:
protected:
	/*内存池管理器接口对象*/
	CMemPoolInterface	m_mpi;		
				
	/*任务队列数组*/
	CJobPkg_imp*		m_pJobPkgs[JP_PRI_LEVELCOUNT];	//已提交任务队列队头数组
	CJobPkg_imp*		m_pJobPkgEnd[JP_PRI_LEVELCOUNT];//已提交任务队列队尾数组
	CJobPkg_imp*		m_pUnSubmitPkgs;				//未提交任务

	/*线程同步成员*/
	CEvent				m_evtWakeUp;					//线程唤醒事件
	CMutex				m_mtxUnSubmitPkgGuard;			//未提交任务链互斥量

	/*退出工作线程标志*/
	bool				m_bExit;

	/*线程句柄*/
	pthread_t			m_thWorker;

	/*线程存在标志*/
	bool				m_bWorkerExist;			

	/*已提交任务计数器*/
	UINT				m_uJobCount;
	
	static void* WorkerThread(void* pvParam);			//线程函数
	bool WakeUpWorker();								//唤醒或创建工作线程
	bool ExitWorker();									//唤醒工作线程并通知其退出,
														//返回true表示工作线程原本存在,
														//返回false表示工作线程原本不存在

	//取下一个任务包，如果系统已经出发工作线程退出事件，
	//则返回false，否则始终返回true，
	//如果队列为空，则进入挂起状态。
	//*ppJobPkg返回取得的任务包
	bool GetNextPkg(CJobPkg_imp** ppJobPkg);
	
	//从任务队列中取出任务包
	//任务调度策略在此函数中实现
	CJobPkg_imp* GetPkg();
	
public:
	CJobNest_imp();
	virtual ~CJobNest_imp();

	virtual void Empty();
	virtual bool Suspend();				//暂不支持
	virtual bool Resume();				//暂不支持

	virtual CJobPkg* GetJobPkg(SIZE initsize);
	virtual bool SubmitJobPkg(CJobPkg* pjpg);
	virtual void ReleaseJobPkg(CJobPkg* pjpg);
	virtual SIZE GetMemPoolFreeSize();
	virtual SIZE GetTotalMemPoolSize();

	bool SetJobPriroity(CJobPkg_imp* pJobPkg, UINT uPri);
	bool SetJobSchedule(CJobPkg_imp* pJobPkg, UINT uSch);
};

#endif //#ifndef L_JN_H_2E2076BC_B0C1_4a98_889F_8DAABC69D62E
