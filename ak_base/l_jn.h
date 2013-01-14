/*-----------------------------------------------------------------------*\
	file:		l_jn.h
	module:		jobnest for linux
	descript:	本模块提供Linux下的任务巢(多线程任务队列)功能,用户程序必须
				包含本文件才能使用任务巢模块
	by:			Rick Wang
	date:		2002/3
\*-----------------------------------------------------------------------*/
#ifndef L_JN_H_2E2076BC_B0C1_4a98_889F_8DAABC69D62E
#define L_JN_H_2E2076BC_B0C1_4a98_889F_8DAABC69D62E
#include "rkTypes.h"
/*---------------------------------------
	符号常量定义
---------------------------------------*/
//任务包优先级
#define JP_PRI_TOP			0u		//最高
#define JP_PRI_HIGH			1u		//高
#define JP_PRI_NORMAL		2u		//通常
#define JP_PRI_LOW			3u		//低
#define JP_PRI_IDEL			4u		//最低

//异常或错误码
#define ERR_INVALID_PARAMETER	0x2010u		//非法参数
#define ERR_POOL_CREATE_FAILED	0x2011u		//内存池创建失败

/*---------------------------------------
	数据类型及结构定义
---------------------------------------*/
/*
	任务执行函数
*/
typedef void (*JOBFUNCTION)(PVOID pvParam);

/*---------------------------------------
	类定义
---------------------------------------*/
/*
	任务包接口类: CJobPkg
*/
class CJobPkg{
private:
protected:
public:
	CJobPkg();
	virtual ~CJobPkg()=0;

	//申请内存块
	virtual PVOID Alloc(SIZE size)=0;
	virtual PVOID Alloc(SIZE size, UCHAR InitVal)=0;

	//释放内存块
	//注意：在本内存块之后申请的内存块将一并被回收
	virtual void Free(PVOID pv)=0;

	//设置任务执行时的任务线程调度优先级属性
	virtual bool SetSchedule(UINT uSch)=0;

	//读取当前任务的线程调度优先级属性
	virtual UINT GetSchedule()=0;

	//设置任务的优先级
	virtual bool SetPriroity(UINT uPri)=0;

	//读取当前任务的优先级
	virtual UINT GetPriroity()=0;

	//设置任务执行函数
	virtual void SetExecFunction(JOBFUNCTION pfun)=0;

	//设置任务执行函数的参数
	virtual void SetExecParam(PVOID pvParam)=0;
	
	//设置任务包ID，在已经提交的任务队列中相同ID的新任务包将
	//覆盖老任务包（使其被删除），除非ID为0
	//返回原来的ID值
	virtual DWORD SetID(DWORD dwID)=0;
	virtual DWORD GetID()=0;

	virtual bool IsSubmitted()=0;

	//内部工具函数，对用户无效
	virtual void* QueryIF()=0;
};

/*
	任务巢类: CJobNest
*/
class CJobNest{
private:
protected:
public:
	CJobNest();
	virtual ~CJobNest()=0;

	//清空任务巢(清除已经提交的所有任务包)
	virtual void Empty()=0;
	//挂起任务巢的运行
	//virtual bool Suspend()=0;
	//恢复任务巢的运行
	//virtual bool Resume()=0;

	//申请一个任务包对象的引用指针,必须与SubmitJobPkg或ReleaseJobPkg函数成对使用
	virtual CJobPkg* GetJobPkg(SIZE initsize)=0;			
	//提交一个任务包对象(该任务包被执行后将自动被释放，已经释放的包不能提交
	//否则结果是不确定的)
	//若任务巢中存在相同ID的已提交任务包，则旧任务包将被删除
	virtual bool SubmitJobPkg(CJobPkg* pjpg)=0;				
	//释放一个任务包对象的应用(已经提交的包不能释放，否则结果是不确定的)
	//注意,被释放后,用户程序就不能再使用该任务包对象指针,否则结果是不确定的
	virtual void ReleaseJobPkg(CJobPkg* pjpg)=0;
	//获取内存池管理器当前占用的总内存内存容量
	virtual SIZE GetTotalMemPoolSize()=0;
	//获取内存池管理器当前空闲内存容量
	virtual SIZE GetMemPoolFreeSize()=0;
};

extern CJobNest* GetJobNest();
extern void ReleaseJobNest(CJobNest* pjn);

#endif //#ifndef L_JN_H_2E2076BC_B0C1_4a98_889F_8DAABC69D62E
