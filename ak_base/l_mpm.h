/*-----------------------------------------------------------------------*\
	file:		l_mpm.h
	module:		memory pool manager
	descript:	内存池管理器，提供高速，低碎片的内存分配机制(只适用于Linux环境)
				本文件为用户程序要包含的头文件
	by:			Rick Wang
	date:		2002/3
\*-----------------------------------------------------------------------*/
#ifndef L_MPM_H_91491E4D_15D7_4887_BE65_8323308447CC
#define L_MPM_H_91491E4D_15D7_4887_BE65_8323308447CC

#include "rkTypes.h"

//异常代码定义
#define ERR_POOL_DAMAGED					0x1010u
#define ERR_NOT_ENOUGH_MEMORY				0x1011u

/*
	内存池接口类: CMemPoolInterface
*/
class CMemPoolInterface{
private:
protected:
public:
	CMemPoolInterface();
	~CMemPoolInterface();

	/*
		内存池操作
	*/
	HANDLE CreatePool(SIZE initsize);								//创建内存池，可以指定池的初始大小
	HANDLE CreatePool(HANDLE hParentPool, SIZE initsize);			//创建指定池的子池
	void EmptyPool(HANDLE hPool);									//清空内存池，回收原先向它申请的所有内存
	void FreePool(HANDLE hPool);									//释放内存池，把它占用的交还给操作系统
	void FreeAllPools();											//释放所有内存池，把占用的内存交还给操作系统
	void DestroyPool(HANDLE hPool);									//撤消内存池，把它占用的内存交还给内存池管理器
	void DestroyAllPools();											//撤消所有内存池，把占用的内存交还给内存池管理器
	void FreeAllMemory();											//将内存池管理器中所有空闲的内存交还给操作系统
	/*
		内存操作
	*/
	PVOID Alloc(HANDLE hPool, SIZE size);							//从指定内存池申请内存
	PVOID AllocInit(HANDLE hPool,SIZE size, UCHAR ucInitVal=0);		//从指定内存池申请内存并初始化每个字节为指定的值
	void Free(HANDLE hPool, PVOID pv);								//回收申请的内存（注意，在该回收的指针之后申请的内存也将一并被回收）
	SIZE GetFreeMemSizeInPoolManeger();						
	SIZE GetTotalMemSizeInPoolManeger();
};

//工具函数
extern DWORD GrainAlign(SIZE size);									//计算指定的尺寸可圆整为多少个分配粒度
extern DWORD PageAlign(SIZE size);									//计算指定的尺寸可圆整为多少个内存页

#endif //#ifndef L_MPM_H_91491E4D_15D7_4887_BE65_8323308447CC
