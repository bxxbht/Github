/*-----------------------------------------------------------------------*\
	file:		l_mpm_imp.h
	module:		memory pool manager
	descript:	内存池管理器，提供高速，低碎片的内存分配机制(只适用于Linux环境)
				本文件为内部实现的头文件
	by:			Rick Wang
	date:		2002/3
\*-----------------------------------------------------------------------*/
#ifndef L_MPM_IMP_H_91491E4D_15D7_4887_BE65_8323308447CC
#define L_MPM_IMP_H_91491E4D_15D7_4887_BE65_8323308447CC
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "l_mpm.h"
#include "rkthread.h"
#include "rkTypes.h"

#include "stdio.h"
#include "assert.h"

#ifndef NDEBUG
#define dbout(x,y) \
	do{printf(x,y);printf("\n");}while(0)
#else
#define dbout(x,y)
#endif

#define MPM_GRAINSIZE	4096							//内存分配粒度(运行时将根据系统页面大小进行圆整)

extern DWORD	g_dwDataAlign;							//地址边界
extern SIZE		g_PageSize;								//系统内存页大小
extern SIZE		g_GrainSize;							//本模块分配粒度大小

class CMemPool;
class CMemPoolMgr;
/*
	内存块头
*/
class CBlkHdr{
private:
protected:
	/*
		内存块头信息结构 -----------------------------------------------------
	*/
	struct BLKHDR{
		DWORD			dwGrainCount;					//本块包含的分配颗粒数
		CBlkHdr*		pNext;							//下一块
		CBlkHdr*		pPrev;							//上一块
		CBlkHdr*		pBigger;						//较大的一块
		PUCHAR			pNextAvail;						//下一个可用的空闲指针
		PUCHAR			pEnd;							//数据区尾字节
		UCHAR			ucData[0];						//数据区
	}m_blk;
	/*------------------------------------------------------------------------*/
	static DWORD		ms_TotalBlkCount;
	static SIZE		ms_TotalMemSize;
	static CMutex		ms_mtxGuard;
public:
	CBlkHdr();
	~CBlkHdr();

	bool IsAddressInBlk(PVOID pv) const;				//判断一个地址是否在本内存块的数据区内						
	bool IsFit(SIZE size) const;						//判断本块是否适应需求
	DWORD GetGrainCount() const;						//获取本块的分配颗粒数
	bool IsFull() const;								//判断本块是否已满
	SIZE GetAvailSize() const;							//获取本块可用空闲区大小
	PVOID AlignAddress(PVOID pv) const;					//对指针进行数据对齐，返回对齐了的指针值

	void* operator new(SIZE size);						//创建一个新块
	void* operator new(SIZE size, 
						SIZE initsize);					//创建一个指定分配颗粒数的新块
	void operator delete(void* pblk);					//删除一块，释放物理内存

	void operator delete[](void* pv){return;};
	void* operator new[](SIZE size){return NULL;};

	CBlkHdr* GetNext() const;
	CBlkHdr* GetPrev() const;
	CBlkHdr* GetBigger() const;
	void SetNext(CBlkHdr* pNext);
	void SetPrev(CBlkHdr* pPrev);
	void SetBigger(CBlkHdr* pBigger);

	void InsertAfter(CBlkHdr* pblk_before);				//插入到指定块之后
	void InsertBefore(CBlkHdr* pblk_after);				//插入到指定块之前
	void RemoveOut();									//从当前块链中移出

	PVOID Alloc(SIZE size);								//分配内存
	void Free(PVOID pv);								//将本块内指定位置之后的内存设为可用（即回收）
	void Empty();										//清空本块

	static DWORD GetTotalMemBlkCount();
	static SIZE GetTotalMemSize();

	void DebugDump(){
		dbout("--! CBlkHdr dump : blk[0x%08x]----",this);
		dbout("--! dwGrainCount = %u",m_blk.dwGrainCount);
		dbout("--! pNext = 0x%08x",m_blk.pNext);
		dbout("--! pPrev = 0x%08x",m_blk.pPrev);		
		dbout("--! pBigger = 0x%08x",m_blk.pBigger);		
		dbout("--! pNextAvail = 0x%08x",m_blk.pNextAvail);		
		dbout("--! ucData = 0x%08x",m_blk.ucData);	
		dbout("--! Total blk count = %d",ms_TotalBlkCount);
		dbout("--! Total blk size = %d",ms_TotalMemSize);
		dbout("--! Dump end ---------------------------",NULL);
		assert(m_blk.pNextAvail && m_blk.dwGrainCount);
	}	
};

/*
	内存池管理器
*/
class CMemPoolMgr{
private:
protected:
	CBlkHdr*				m_pFreeBlks;				//空闲块链
	CBlkHdr*				m_pLastFreeBlk;				//空闲块链最后一块

	CMemPool*				m_pTopLevelPools;			//顶级内存池链


	CMutex					m_mtxFreeBlksGuard;
	CMutex					m_mtxPoolsGuard;
	
	DWORD					m_dwFreeBlkCount;
	SIZE					m_szFreeBlkSize;
public:
	CMemPoolMgr();
	~CMemPoolMgr();

	/*
		内存块链管理
	*/
	void FreeAllBlks();									//将空闲块链中所有的内存块释放
	CBlkHdr* AllocBlkFromFreeChain(DWORD dwGrainCount);	//从空闲块链中申请一块
	void ReturnBlksToFreeChain(CBlkHdr* pblks);			//将一串块放回空闲块链
	
	/*
		内存池链管理
	*/
	void AddPool(CMemPool* pPool);						//将一个顶级内存池加入到内存池链中，
														//该池的父池指针将被置空
	void AddPool(CMemPool* pParent,CMemPool* pPool);	//将一个池加入到内存池树中，pParent若为NULL，则该池加入到
														//顶级池链中

	void RemovePool(CMemPool* pPool);					//将一个内存池从内存池链中移出
	CMemPool* CreatePool(CMemPool* pParent,
						SIZE initsize);					//创建一个内存池
	void DestroyPool(CMemPool* pPool);
	void DestroyAllPools();
	void FreePool(CMemPool* pPool);
	void FreeAllPools();
	
	DWORD GetFreeBlkCount(){return m_dwFreeBlkCount;};

	SIZE GetFreeBlkSize(){return m_szFreeBlkSize;};
};



/*
	内存池
*/
class CMemPool{
private:
	CBlkHdr*		m_pBlks;							//块链
	CBlkHdr*		m_pLastBlk;							//末块
	
	CMemPool*		m_pNext;							//下一个兄弟池
	CMemPool*		m_pPrev;							//上一个兄弟池
	CMemPool*		m_pParent;							//父池
	CMemPool*		m_pChild;							//子池

	CMutex			m_mtxBlksGuard;						//保护内存块链的互斥量对象

	bool			m_bFree;

	//对内存块的操作
	static CBlkHdr* AllocBlk(SIZE size);				//申请一块
	static void ReturnBlks(CBlkHdr* pBlks);				//回收若干块
	static void FreeBlks(CBlkHdr* pBlks);				//释放若干块

	CBlkHdr* AppendBlk(SIZE size);						//向内存池追加一个指定大小的内存块
protected:

	CMemPool();
	~CMemPool();

	CMemPool* GetParent() const;
	CMemPool* GetChild() const;
	CMemPool* GetNext() const;
	CMemPool* GetPrev() const;
	void SetParent(CMemPool* pParent);
	void SetChild(CMemPool* pChild);
	void SetNext(CMemPool* pNext);
	void SetPrev(CMemPool* pPrev);

	void InsertAfter(CMemPool* pBefore);
	void InsertBefore(CMemPool* pAfter);
	void RemoveOut();

	void* operator new(SIZE size);
	void* operator new(SIZE size, 
						SIZE initsize);
	//重载delete运算符以确保撤消或释放内存池时，析构函数被正确调用
	void operator delete(void* pPool);

	void* operator new[](SIZE size){return NULL;};
	void operator delete[](void* pv){return;};

	void Destroy();
	void FreePool();

	friend class CMemPoolMgr;
public:
	PVOID Alloc(SIZE size);
	PVOID Alloc(SIZE size, UCHAR ucInitVal);

	void Free(PVOID pv);
	void Empty();
};

#endif //#ifndef L_MPM_IMP_H_91491E4D_15D7_4887_BE65_8323308447CC
