/*-----------------------------------------------------------------------*\
	file:		l_mpm.cpp
	module:		memory pool manager
	descript:	内存池管理器，提供高速，低碎片的内存分配机制(只适用于Linux环境)
				本文件为内部实现文件
	by:			Rick Wang
	date:		2002/3
\*-----------------------------------------------------------------------*/
#include "l_mpm_imp.h"

DWORD g_dwDataAlign=0;
SIZE g_PageSize=0;				//在g_mpm初始化时，g_PageSize将被赋予操作系统的值
SIZE g_GrainSize=0;				//在g_mpm初始化时，g_GrainSize将根据g_PageSize进行圆整

inline DWORD GrainAlign(SIZE size){
	return ((DWORD)((size+g_GrainSize-1)/g_GrainSize));
}
inline DWORD PageAlign(SIZE size){
	return ((DWORD)((size+g_PageSize-1)/g_PageSize));
}

/*
	全局唯一的内存池管理器对象
*/
CMemPoolMgr g_mpm;


/*================================
	内存池模块接口类实现代码
================================*/
CMemPoolInterface::CMemPoolInterface(){
}

CMemPoolInterface::~CMemPoolInterface(){
}

/*
	内存池操作
*/
HANDLE CMemPoolInterface::CreatePool(SIZE initsize){
	return (HANDLE)g_mpm.CreatePool(NULL,initsize);
}

HANDLE CMemPoolInterface::CreatePool(HANDLE hParentPool, SIZE initsize){
	return (HANDLE)g_mpm.CreatePool((CMemPool*)hParentPool,initsize);
}

void CMemPoolInterface::EmptyPool(HANDLE hPool){
	assert(hPool);
	if(!hPool){
		return;
	}

	((CMemPool*)hPool)->Empty();
}

void CMemPoolInterface::FreePool(HANDLE hPool){
	assert(hPool);
	if(!hPool){
		return;
	}
	g_mpm.FreePool((CMemPool*)hPool);
}

void CMemPoolInterface::FreeAllPools(){
	g_mpm.FreeAllPools();
}

void CMemPoolInterface::DestroyPool(HANDLE hPool){
	assert(hPool);
	if(!hPool){
		return;
	}
	g_mpm.DestroyPool((CMemPool*)hPool);
}

void CMemPoolInterface::DestroyAllPools(){
	g_mpm.DestroyAllPools();
}

/*
	内存操作
*/
PVOID CMemPoolInterface::Alloc(HANDLE hPool, SIZE size){
	assert(hPool);
	if(!hPool){
		return NULL;
	}
	return ((CMemPool*)hPool)->Alloc(size);
}

PVOID CMemPoolInterface::AllocInit(HANDLE hPool,SIZE size, UCHAR ucInitVal){
	assert(hPool);
	if(!hPool){
		return NULL;
	}
	return ((CMemPool*)hPool)->Alloc(size,ucInitVal);
}

void CMemPoolInterface::Free(HANDLE hPool, PVOID pv){
	assert(hPool);
	if(!hPool){
		return;
	}
	((CMemPool*)hPool)->Free(pv);
}

void CMemPoolInterface::FreeAllMemory(){
	g_mpm.FreeAllBlks();
}

SIZE CMemPoolInterface::GetFreeMemSizeInPoolManeger(){
	return g_mpm.GetFreeBlkSize();
}

SIZE CMemPoolInterface::GetTotalMemSizeInPoolManeger(){
	return CBlkHdr::GetTotalMemSize();
}

/*=======================
	内存块头类实现代码
=======================*/
DWORD CBlkHdr::ms_TotalBlkCount=0;
SIZE CBlkHdr::ms_TotalMemSize=0;
CMutex CBlkHdr::ms_mtxGuard;

CBlkHdr::CBlkHdr(){
	m_blk.pNext=m_blk.pPrev=NULL;
	m_blk.pBigger=NULL;
	m_blk.pNextAvail=m_blk.ucData;
	m_blk.pEnd=(PUCHAR)this + (m_blk.dwGrainCount * g_GrainSize - 1);
//	dbout("****************Next avail address: 0x%08x",m_blk.pNextAvail);
}

CBlkHdr::~CBlkHdr(){
}

/*虚拟内存分配方案在此实现-------------------------------------------*/
void* CBlkHdr::operator new(SIZE size){
//	dbout("****************new blk: size=%d",size);
	
	assert(size>0);


	//计算圆整为特定分配粒度时需要的尺寸
	SIZE blk_size=g_GrainSize * GrainAlign(size);
	//分配内存，建立物理映射
	void* pv=mmap(NULL,blk_size, PROT_READ | PROT_WRITE,
				  MAP_ANON | MAP_PRIVATE, -1, 0);
	//若分配失败，将返回值置为空
	if(-1==(int)pv || !pv){
		pv=NULL;
		throw(ERR_NOT_ENOUGH_MEMORY);
	}
	else{
		((BLKHDR*)pv)->dwGrainCount=GrainAlign(size);
//		dbout("thread[%d]:******lock blk new",pthread_self());
		CBlkHdr::ms_mtxGuard.Lock();

		CBlkHdr::ms_TotalBlkCount++;
		CBlkHdr::ms_TotalMemSize+=blk_size;

		CBlkHdr::ms_mtxGuard.UnLock();
//		dbout("thread[%d]:******unlock blk new--------",pthread_self());
//		dbout("****************new blk size in fact: size=%d",blk_size);
//		dbout("****************blk address: 0x%08x",pv);
	}

	return pv;
}

void* CBlkHdr::operator new(SIZE size, SIZE initsize){
	assert(size>0);
	size+= initsize;
	return (operator new(size));
}

void CBlkHdr::operator delete(void* pblk){
//	dbout("*******************Delete blk: 0x%08x",pblk);
//	dbout("*******************%d byte memory will be free",((BLKHDR*)pblk)->dwGrainCount * g_GrainSize);
//	dbout("thread[%d]:******lock blk delete",pthread_self());
	CBlkHdr::ms_mtxGuard.Lock();

	CBlkHdr::ms_TotalBlkCount--;
	CBlkHdr::ms_TotalMemSize-=((BLKHDR*)pblk)->dwGrainCount * g_GrainSize;

	CBlkHdr::ms_mtxGuard.UnLock();
//	dbout("thread[%d]:******unlock blk delete--------",pthread_self());
	
	munmap(pblk,((BLKHDR*)pblk)->dwGrainCount * g_GrainSize);
}
/*-------------------------------------------------------------------*/

inline PVOID CBlkHdr::AlignAddress(PVOID pv) const{
	//只有当g_dwDataAlign=0x1、0x10、0x100、0x1000、0x10000...时，本算法才正确
	//即m_dwDataAlign=1、2、4、8、16...2^n
	//因m_dwDataAlign表示系统数据宽度（以字节为单位），所以g_dwDataAlign满足上述要求
	return (PVOID)(((DWORD)pv+(g_dwDataAlign-1)) & ~(g_dwDataAlign-1));
}

inline SIZE CBlkHdr::GetAvailSize() const{
	return (SIZE)(m_blk.pEnd >= m_blk.pNextAvail ? (DWORD)m_blk.pEnd - (DWORD)m_blk.pNextAvail + 1 : 0);
}

inline bool CBlkHdr::IsAddressInBlk(PVOID pv) const{
	return (DWORD)pv>=(DWORD)(m_blk.ucData) && (DWORD)pv<=(DWORD)(m_blk.pEnd);
}

inline bool CBlkHdr::IsFull() const{
	return m_blk.pEnd >= m_blk.pNextAvail;
}

inline bool CBlkHdr::IsFit(SIZE size) const{
	return (GetAvailSize() >= size);
}

inline DWORD CBlkHdr::GetGrainCount() const{
	return m_blk.dwGrainCount;
}

inline CBlkHdr* CBlkHdr::GetBigger() const{
	return m_blk.pBigger;
}

inline CBlkHdr* CBlkHdr::GetNext() const{
	return m_blk.pNext;
}

inline CBlkHdr* CBlkHdr::GetPrev() const{
	return m_blk.pPrev;
}

inline void CBlkHdr::SetBigger(CBlkHdr* pBigger){
	m_blk.pBigger=pBigger;
}

inline void CBlkHdr::SetNext(CBlkHdr* pNext){
	m_blk.pNext=pNext;
}

inline void CBlkHdr::SetPrev(CBlkHdr* pPrev){
	m_blk.pPrev=pPrev;
}

inline void CBlkHdr::InsertAfter(CBlkHdr* pblk_before){
	if(pblk_before){
		//连前后
		m_blk.pNext=pblk_before->GetNext();
		m_blk.pPrev=pblk_before;
		//断前
		pblk_before->SetNext(this);
		//断后
		if(m_blk.pNext){
			m_blk.pNext->SetPrev(this);
		}
	}
	else{
		m_blk.pNext=m_blk.pPrev=NULL;
	}
}

inline void CBlkHdr::InsertBefore(CBlkHdr* pblk_after){
	if(pblk_after){
		//连前后
		m_blk.pNext=pblk_after;
		m_blk.pPrev=pblk_after->GetPrev();
		//断前
		if(m_blk.pPrev){
			m_blk.pPrev->SetNext(this);
		}
		//断后
		pblk_after->SetPrev(this);
	}
	else{
		m_blk.pNext=m_blk.pPrev=NULL;
	}
}

inline void CBlkHdr::RemoveOut(){
	//连前后
	if(m_blk.pPrev){
		m_blk.pPrev->SetNext(m_blk.pNext);
	}
	if(m_blk.pNext){
		m_blk.pNext->SetPrev(m_blk.pPrev);
	}
	//断前
	m_blk.pPrev=NULL;
	//断后
	m_blk.pNext=NULL;
}

inline PVOID CBlkHdr::Alloc(SIZE size){
//	dbout("***************+blk alloc: size = %d",size);
	PVOID pv=NULL;
	if(size && IsFit(size)){
		pv=m_blk.pNextAvail;
		m_blk.pNextAvail=(PUCHAR)AlignAddress((char*)(m_blk.pNextAvail)+size);
		
		assert(m_blk.pNextAvail<=AlignAddress(m_blk.pEnd+1));
	}
//	dbout("***************+Next avail address: 0x%08x",m_blk.pNextAvail);
	return pv;
}

inline void CBlkHdr::Free(PVOID pv){
	if(pv){
		if((DWORD)pv>=(DWORD)(m_blk.ucData) && (DWORD)pv<=(DWORD)(m_blk.pEnd)){
			m_blk.pNextAvail=(PUCHAR)AlignAddress(pv);;
		}
	}
}

inline void CBlkHdr::Empty(){
	m_blk.pNextAvail=(PUCHAR)AlignAddress(m_blk.ucData);
}

inline DWORD CBlkHdr::GetTotalMemBlkCount(){
	return CBlkHdr::ms_TotalBlkCount;
};
inline SIZE CBlkHdr::GetTotalMemSize(){
	return CBlkHdr::ms_TotalMemSize;
};


/*
	内存池管理器类实现代码
*/
CMemPoolMgr::CMemPoolMgr(){
	//初始全局变量
	g_dwDataAlign=sizeof(int);
	if(!g_GrainSize){
		assert(MPM_GRAINSIZE);
		g_PageSize=getpagesize();
		g_GrainSize=g_PageSize * PageAlign(MPM_GRAINSIZE);
	}

	m_pFreeBlks=NULL;
	m_pLastFreeBlk=NULL;
	m_pTopLevelPools=NULL;
	
	m_dwFreeBlkCount=0;
	m_szFreeBlkSize=0;
}

CMemPoolMgr::~CMemPoolMgr(){
	//rick:debug
//	dbout("thread[%d]:CMemPoolMgr destroying...",pthread_self());
	
	//释放所有空闲块的内存
	FreeAllBlks();
	//释放所有内存池的内存
	FreeAllPools();
	
	//rick:debug
//	dbout("thread[%d]:CMemPoolMgr destroyed",pthread_self());
}

/*空闲块链操作函数----------------------------------------------------*/
CBlkHdr* CMemPoolMgr::AllocBlkFromFreeChain(DWORD dwGrainCount){
	m_mtxFreeBlksGuard.Lock();

	//查找合适的块
	CBlkHdr* pblk=m_pFreeBlks;
	CBlkHdr* pblk_smaller=NULL;
	while(pblk && pblk->GetGrainCount()<dwGrainCount){
		pblk_smaller=pblk;
		pblk=pblk->GetBigger();
	}

	//若找到
	if(pblk){
		CBlkHdr* pnext=pblk->GetNext();
		if(pnext){
			//下一块的大小必须是大于或等于本块
			assert(pnext->GetGrainCount() >= pblk->GetGrainCount());
			//处理大小顺序链
			if(pnext->GetGrainCount()==pblk->GetGrainCount()){
				pnext->SetBigger(pblk->GetBigger());
			}

		}
		else{
			//没有下一个块，则要修改尾块指针
			m_pLastFreeBlk=pblk->GetPrev();
		}

		if(pblk_smaller){
			pblk_smaller->SetBigger(pnext);
		}
		else{
			//没有更小的块，则本块必须是当前的首块
			assert(m_pFreeBlks==pblk);
			//修改首块指针，指向下一块
			m_pFreeBlks=pnext;
		}

		//将本块退出链表
		pblk->RemoveOut();
		pblk->SetBigger(NULL);

		m_dwFreeBlkCount--;
		m_szFreeBlkSize-=pblk->GetGrainCount()*g_GrainSize;
	}
	
//    dbout("...............Alloc -- : Cur block count %d",m_dwFreeBlkCount);
	m_mtxFreeBlksGuard.UnLock();
	
	return pblk;
}

//将一串内存块返还给空闲块链
//返还前，所有块都将清空，但不会释放已经提交的物理内存
//这些块将按其链接的反向顺序送入空闲块链(对相同大小的块总是将最后反还的块放在最前面)
//(即后申请的先反还，因为先申请的块其已提交物理内存可能比较多，将其放在空闲链的前端以方便
//下次用户申请块时先将其送出，从而减少物理内存的提交，也减少执行物理内存提交的时间开销)
void CMemPoolMgr::ReturnBlksToFreeChain(CBlkHdr* pblks){
	if(!pblks){
		return;
	}
	pblks->SetPrev(NULL);

	CBlkHdr* pblk_cur=NULL;
	CBlkHdr* pblk_end=pblks;
	CBlkHdr* pblk_smaller=NULL;
	CBlkHdr* pblk_after=NULL;

	//找到链中最后一块
	while(pblk_end->GetNext()){
//		assert((pblk_end->DebugDump(),true));
		pblk_end=pblk_end->GetNext();
	}

	m_mtxFreeBlksGuard.Lock();
	
	while(pblk_end){
		pblk_cur=pblk_end;
		pblk_end=pblk_end->GetPrev();

		//清空块
		pblk_cur->Empty();
		
		//寻找插入位置（按块的尺寸从小到大插入）
		pblk_after=m_pFreeBlks;
		pblk_smaller=NULL;
		while(pblk_after && pblk_after->GetGrainCount()<pblk_cur->GetGrainCount()){
			pblk_smaller=pblk_after;
			pblk_after=pblk_after->GetBigger();
		}
		
		//插入链表
		if(pblk_after){
			pblk_cur->InsertBefore(pblk_after);
			if(m_pFreeBlks==pblk_after){
				m_pFreeBlks=pblk_cur;
			}

			if(pblk_after->GetGrainCount()==pblk_cur->GetGrainCount()){
				pblk_cur->SetBigger(pblk_after->GetBigger());
			}
			else{
				pblk_cur->SetBigger(pblk_after);
			}
		}
		else{
			if(m_pLastFreeBlk){
				//没有大于等于本块的空闲块，则将本块插入到末尾
				pblk_cur->InsertAfter(m_pLastFreeBlk);
			}
			else{
				//若没有尾块，则空闲块链必须是空的
				assert(m_pFreeBlks==NULL);
				//将本块设为首块
				pblk_cur->SetPrev(NULL);
				pblk_cur->SetNext(NULL);
				m_pFreeBlks=pblk_cur;
			}
			//本块肯定也是尾块
			m_pLastFreeBlk=pblk_cur;

			pblk_cur->SetBigger(NULL);
		}//end if(pblk_after)

		if(pblk_smaller){
			pblk_smaller->SetBigger(pblk_cur);
		}
		
		//pblk_cur->DebugDump();
		m_dwFreeBlkCount++;
		m_szFreeBlkSize+=pblk_cur->GetGrainCount()*g_GrainSize;
	}//end of while(pblk_end)
	
//    dbout("...............Return ++ : Cur block count %d",m_dwFreeBlkCount);
	m_mtxFreeBlksGuard.UnLock();

}

void CMemPoolMgr::FreeAllBlks(){
	m_mtxFreeBlksGuard.Lock();
	//rick:debug
//	dbout("thread[%d]:FreeAllBlks...",pthread_self());

	while(m_pFreeBlks && m_pFreeBlks->GetNext()){
		m_pFreeBlks=m_pFreeBlks->GetNext();
		delete (m_pFreeBlks->GetPrev());
	}

	if(m_pFreeBlks){
		delete m_pFreeBlks;
	}

	m_pFreeBlks=m_pLastFreeBlk=NULL;
	m_dwFreeBlkCount=0;
	
	//rick:debug
//	dbout("thread[%d]:FreeAllBlks ok...",pthread_self());
	m_mtxFreeBlksGuard.UnLock();
}
/*--------------------------------------------------------------------*/

/*内存池操作函数------------------------------------------------------*/

//将一个池加入到顶级池链中
void CMemPoolMgr::AddPool(CMemPool* pPool){
	assert(pPool);
	if(!pPool){
		return;
	}
	m_mtxPoolsGuard.Lock();

	pPool->InsertBefore(m_pTopLevelPools);
	m_pTopLevelPools=pPool;

	m_mtxPoolsGuard.UnLock();
}

void CMemPoolMgr::AddPool(CMemPool* pParent, CMemPool* pPool){
	if(!pParent){
		AddPool(pPool);
		return;
	}

	assert(pPool);
	if(!pPool){
		return;
	}

	m_mtxPoolsGuard.Lock();

	pPool->InsertBefore(pParent->GetChild());
	pParent->SetChild(pPool);
	pPool->SetParent(pParent);

	m_mtxPoolsGuard.UnLock();
}

//从内存池树中移走一个池，这个池的所有子孙池也将一并移走
void CMemPoolMgr::RemovePool(CMemPool* pPool){
	assert(pPool);
	if(!pPool){
		return;
	}

	m_mtxPoolsGuard.Lock();

	if(pPool->GetParent()){
		//若本池是众兄弟池中的第一个，则要修改父池的子池指针，使其指向下一个兄弟池
		if(!pPool->GetPrev()){
			pPool->GetParent()->SetChild(pPool->GetNext());
		}

		pPool->SetParent(NULL);
	}

	if(m_pTopLevelPools==pPool){
		m_pTopLevelPools=pPool->GetNext();
	}
	
	pPool->RemoveOut();

	m_mtxPoolsGuard.UnLock();
}

void CMemPoolMgr::DestroyPool(CMemPool* pPool){
	assert(pPool);
	if(!pPool){
		return;
	}

	//将池从池树中移出
	RemovePool(pPool);
	pPool->Destroy();
}

void CMemPoolMgr::DestroyAllPools(){
	m_mtxPoolsGuard.Lock();
	
	while(m_pTopLevelPools && m_pTopLevelPools->GetNext()){
		m_pTopLevelPools=m_pTopLevelPools->GetNext();
		m_pTopLevelPools->GetPrev()->Destroy();
	}
	
	if(m_pTopLevelPools){
		m_pTopLevelPools->Destroy();
		m_pTopLevelPools=NULL;
	}

	m_mtxPoolsGuard.UnLock();
}

void CMemPoolMgr::FreePool(CMemPool* pPool){
	assert(pPool);
	if(!pPool){
		return;
	}

	//将池从池树中移出
	RemovePool(pPool);
	pPool->FreePool();
}

void CMemPoolMgr::FreeAllPools(){
	m_mtxPoolsGuard.Lock();
	//rick:debug
//	dbout("thread[%d]:FreeAllPools...",pthread_self());
	
	while(m_pTopLevelPools && m_pTopLevelPools->GetNext()){
		m_pTopLevelPools=m_pTopLevelPools->GetNext();
		m_pTopLevelPools->GetPrev()->FreePool();
	}
	
	if(m_pTopLevelPools){
		m_pTopLevelPools->FreePool();
		m_pTopLevelPools=NULL;
	}

	//rick:debug
//	dbout("thread[%d]:FreeAllPools ok...",pthread_self());
	m_mtxPoolsGuard.UnLock();
}

/*
	创建内存池，并链入池树
*/
CMemPool* CMemPoolMgr::CreatePool(CMemPool* pParent, SIZE initsize){
	
	CMemPool* pPool=new(initsize) CMemPool;
	AddPool(pParent,pPool);

	return pPool;
}
/*--------------------------------------------------------------------*/

/*=====================
	内存池类实现代码
=====================*/

CMemPool::CMemPool(){
	//初始化内部数据结构
	assert(m_pBlks);
	m_pLastBlk=m_pBlks;
	m_pNext=m_pPrev=m_pParent=m_pChild=NULL;
	m_bFree=false;
//	dbout("**************Availsize = %d",m_pLastBlk->GetAvailSize());
}

CMemPool::~CMemPool(){
}

/*内存块操作函数----------------------------------------------------------------*/
/*
	函数：AllocBlk
	向内存池管理器申请一块内存，如果内存池管理器没有合适的内存块，则创建一个新的
	内存块（向操作系统申请内存）
*/
CBlkHdr* CMemPool::AllocBlk(SIZE size){
//	dbout("**************AllocBlk dwGrainCount=%d",dwGrainCount);
//	dbout("--AllocBlk: Free blks count: %d",g_mpm.GetFreeBlkCount());
	//向内存池管理器申请
	DWORD dwGrain=GrainAlign(size+sizeof(CBlkHdr));
	CBlkHdr* pblk=g_mpm.AllocBlkFromFreeChain(dwGrain);
	//若内存池管理器没有合适的空闲内存块，则创建新的内存块
	if(!pblk){
//		dbout("----No comptible blk, create new blk...%s",NULL);
		pblk=new(size) CBlkHdr;
	}
//	assert((pblk->DebugDump(),true));	
	return pblk;
}
/*
	函数：ReturnBlks
	将指定内存块及其后续块交回给内存池管理器
*/
void CMemPool::ReturnBlks(CBlkHdr* pBlks){
	if(!pBlks){
		return;
	}

	if(pBlks->GetPrev()){
		pBlks->GetPrev()->SetNext(NULL);
	}

	pBlks->SetPrev(NULL);
	g_mpm.ReturnBlksToFreeChain(pBlks);
//	dbout("--ReturnBlk: Free blks count: %d",g_mpm.GetFreeBlkCount());
}
/*
	函数：FreeBlks
	将指定内存块及其后续块释放（交回操作系统）
*/
void CMemPool::FreeBlks(CBlkHdr* pBlks){
	if(!pBlks){
		return;
	}

	if(pBlks->GetPrev()){
		pBlks->GetPrev()->SetNext(NULL);
	}

	while(pBlks->GetNext()){
		pBlks=pBlks->GetNext();
		delete pBlks->GetPrev();
	}
	delete pBlks;
}

/*
	函数：AppendBlk
	向本池追加指定大小的一块内存块
*/
CBlkHdr* CMemPool::AppendBlk(SIZE size){
	if(!size){
		return NULL;
	}

	CBlkHdr* pblk=AllocBlk(size);

	if(pblk){
		pblk->InsertAfter(m_pLastBlk);
		m_pLastBlk=pblk;
	}

	return pblk;
}
/*-----------------------------------------------------------------------------*/
/*
	内存池操作
*/
void* CMemPool::operator new(SIZE size){
	assert(size);

	PVOID pv=NULL;
	CBlkHdr* pblk=AllocBlk(size);
	assert(pblk);
    if(!pblk){
    	throw(ERR_NOT_ENOUGH_MEMORY);
    }
    else{
    	pv=pblk->Alloc(sizeof(CMemPool));
    	assert(pv);
    	((CMemPool*)pv)->m_pBlks=pblk;
    }
	return pv;
}

void* CMemPool::operator new(SIZE size, SIZE initsize){
	assert(size);
	return operator new(size+initsize);
}

void CMemPool::operator delete(void* pv){
	if(!pv){
		return;
	}
	if(((CMemPool*)pv)->m_bFree){
		FreeBlks(((CMemPool*)pv)->m_pBlks);
	}
	else{
		ReturnBlks(((CMemPool*)pv)->m_pBlks);
	}
}

inline CMemPool* CMemPool::GetChild() const{
	return m_pChild;
}

inline CMemPool* CMemPool::GetParent() const{
	return m_pParent;
}

inline CMemPool* CMemPool::GetPrev() const{
	return m_pPrev;
}

inline CMemPool* CMemPool::GetNext() const{
	return m_pNext;
}

inline void CMemPool::SetChild(CMemPool* pChild){
	m_pChild=pChild;
}

inline void CMemPool::SetParent(CMemPool* pParent){
	m_pParent=pParent;
}

inline void CMemPool::SetPrev(CMemPool* pPrev){
	m_pPrev=pPrev;
}

inline void CMemPool::SetNext(CMemPool* pNext){
	m_pNext=pNext;
}

inline void CMemPool::InsertAfter(CMemPool* pBefore){
	if(pBefore){
		m_pPrev=pBefore;
		m_pNext=pBefore->GetNext();

		pBefore->SetNext(this);

		if(m_pNext){
			m_pNext->SetPrev(this);
		}
	}
	else{
		m_pPrev=NULL;
		m_pNext=NULL;
	}
}

inline void CMemPool::InsertBefore(CMemPool* pAfter){
	if(pAfter){
		m_pPrev=pAfter->GetPrev();
		m_pNext=pAfter;

		if(m_pPrev){
			m_pPrev->SetNext(this);
		}

		pAfter->SetPrev(this);
	}
	else{
		m_pPrev=NULL;
		m_pNext=NULL;
	}
}

inline void CMemPool::RemoveOut(){
	if(m_pPrev){
		m_pPrev->SetNext(m_pNext);
	}
	if(m_pNext){
		m_pNext->SetPrev(m_pPrev);
	}

	m_pPrev=m_pNext=NULL;
}

void CMemPool::Empty(){
	CBlkHdr* p=NULL;

	m_mtxBlksGuard.Lock();

	if(p=m_pBlks->GetNext()){
		m_pLastBlk=m_pBlks;
	}
	m_pBlks->Free((char*)this+sizeof(CMemPool));

//	dbout("**************Availsize = %d",m_pLastBlk->GetAvailSize());
	m_mtxBlksGuard.UnLock();

	if(p){
		ReturnBlks(p);
	}
}

void CMemPool::Destroy(){
	m_mtxBlksGuard.Lock();
	
	m_pLastBlk=NULL;

	m_mtxBlksGuard.UnLock();

	//第归调用以撤消所有子孙池
	if(m_pChild){
		CMemPool* pnext=NULL;
		while(pnext=m_pChild->GetNext()){
			pnext->RemoveOut();
			pnext->Destroy();
		}
		m_pChild->Destroy();
	}

/*
	用delete运算符的重载来回收或释放内存块是为了保证
	撤消或释放内存池时析构函数被正确调用
*/
	m_bFree=false;
	delete this;
}

void CMemPool::FreePool(){
	m_mtxBlksGuard.Lock();
	
	m_pLastBlk=NULL;

	m_mtxBlksGuard.UnLock();

	//第归调用以删除所有子孙池
	if(m_pChild){
		CMemPool* pnext=NULL;
		while(pnext=m_pChild->GetNext()){
			pnext->RemoveOut();
			pnext->FreePool();
		}
		m_pChild->FreePool();
	}
/*
	用delete运算符的重载来回收或释放内存块是为了保证
	撤消或释放内存池时析构函数被正确调用
*/
	m_bFree=true;
	delete this;
}

PVOID CMemPool::Alloc(SIZE size){
	if(!size){
		return NULL;
	}

	m_mtxBlksGuard.Lock();
	
	PVOID pv=NULL;
	if(m_pLastBlk){
		if(!m_pLastBlk->IsFit(size)){
			AppendBlk(size);
		}
		pv=m_pLastBlk->Alloc(size);
		assert(m_pLastBlk->IsAddressInBlk((char*)pv+size-1));
	}

	m_mtxBlksGuard.UnLock();
//	dbout("**************Alloced pv = 0x%08x",pv);
//	dbout("**************Availsize = %d",m_pLastBlk->GetAvailSize());
	return pv;
}

PVOID CMemPool::Alloc(SIZE size, UCHAR ucInitVal){
	PVOID pv=Alloc(size);
	if(pv){
		memset(pv,ucInitVal,size);
	}
	return pv;
}

void CMemPool::Free(PVOID pv){
	if(!pv){
		return;
	}

	m_mtxBlksGuard.Lock();

	CBlkHdr* pCut=NULL;
	CBlkHdr* pblk=m_pLastBlk;
	while(pblk){
		if(pblk->IsAddressInBlk(pv)){
			//若该欲释放的地址在内存池的首块内，应检查是否会破坏内存池
			if(pblk==m_pBlks){
				assert((DWORD)pv>=(DWORD)pblk+sizeof(CMemPool));
				//若内存池被破坏，抛出异常
				if(!((DWORD)pv >= (DWORD)pblk+sizeof(CMemPool))){
					throw(ERR_POOL_DAMAGED);		//池被破坏
					break;
				}
			}
			
			pblk->Free(pv);							//回收内存

			//找到指针所在的块，该块作为内存池内块链的最后一块，其他后续的
			//块将被回收
			pCut=pblk->GetNext();
			m_pLastBlk=pblk;
			pblk->SetNext(NULL);

			break;
		}
		pblk=pblk->GetNext();
	}

//	dbout("**************Availsize = %d",m_pLastBlk->GetAvailSize());
	m_mtxBlksGuard.UnLock();

	if(pCut){
		ReturnBlks(pCut);
	}
}

//end of file
