/*-----------------------------------------------------------------------*\
	file:		l_jn.cpp
	module:		jobnest for linux
	descript:	本模块提供Linux下的任务巢(多线程任务队列)功能
	by:			Rick Wang
	date:		2002/3
\*-----------------------------------------------------------------------*/
#include "l_jn_imp.h"

CJobNest* GetJobNest(){
	CJobNest_imp* pjn=new CJobNest_imp;
	assert(pjn);
	return (CJobNest*)pjn;
}

void ReleaseJobNest(CJobNest* pjn){
	if(pjn){
		delete pjn;
	}
}

/*
	CJobQueue类实现代码
*/
CJobPkg::CJobPkg(){
}
CJobPkg::~CJobPkg(){
}

CJobQueue::CJobQueue(){
	m_pPrev=m_pNext=NULL;
}
CJobQueue::~CJobQueue(){
}

inline CJobQueue* CJobQueue::GetNext(){
	return m_pNext;
}

inline CJobQueue* CJobQueue::GetPrev(){
	return m_pPrev;
}

inline void CJobQueue::SetNext(CJobQueue* pNext){
	m_pNext=pNext;
}

inline void CJobQueue::SetPrev(CJobQueue* pPrev){
	m_pPrev=pPrev;
}

inline void CJobQueue::InsertAfter(CJobQueue* pjq_before){
	if(pjq_before){
		//连前后
		m_pNext=pjq_before->m_pNext;
		m_pPrev=pjq_before;
		//断前
		pjq_before->m_pNext=this;
		//断后
		if(m_pNext){
			m_pNext->m_pPrev=this;
		}
	}
	else{
		m_pNext=m_pPrev=NULL;
	}
}

inline void CJobQueue::InsertBefore(CJobQueue* pjq_after){
	if(pjq_after){
		m_pNext=pjq_after;
		m_pPrev=pjq_after->m_pPrev;

		if(m_pPrev){
			m_pPrev->m_pNext=this;
		}

		pjq_after->m_pPrev=this;
	}
	else{
		m_pNext=m_pPrev=NULL;
	}
}

inline void CJobQueue::RemoveOut(){
	//连前后
	if(m_pPrev){
		m_pPrev->m_pNext=m_pNext;
	}
	if(m_pNext){
		m_pNext->m_pPrev=m_pPrev;
	}
	//断前
	m_pPrev=NULL;
	//断后
	m_pNext=NULL;
}

/*
	CJobPkg_imp类实现代码
*/
CJobPkg_imp::CJobPkg_imp():CJobQueue(){
	Init(NULL);
}

CJobPkg_imp::CJobPkg_imp(CJobNest_imp* pjn):CJobQueue(){
	Init(pjn);
}
inline void CJobPkg_imp::Init(CJobNest_imp* pjn){
	m_pjn=pjn;
	m_bSubmitted=false;
	m_uPriroity=JP_PRI_NORMAL;
	m_uSchedule=JN_SCH_INVALID;
	m_pvParam=NULL;
	m_dwID=0;
}

CJobPkg_imp::~CJobPkg_imp(){
	if(m_hPool){
		assert(m_pmpi);
		m_pmpi->DestroyPool(m_hPool);
	}
}

void* CJobPkg_imp::QueryIF(){
	return this;
}

inline void CJobPkg_imp::SetJobNest(CJobNest_imp* pjn){
	m_pjn=pjn;
}

inline bool CJobPkg_imp::IsSubmitted(){
	return m_bSubmitted;
}

inline void CJobPkg_imp::SetSubmit(bool bSubmit){
	m_bSubmitted=bSubmit;
}

inline PVOID CJobPkg_imp::Alloc(SIZE size){
	assert(m_hPool && m_pmpi);
	if(!(m_hPool && m_pmpi)){
		return NULL;
	}
	return m_pmpi->Alloc(m_hPool,size);
}

inline PVOID CJobPkg_imp::Alloc(SIZE size, UCHAR InitVal){
	assert(m_hPool && m_pmpi);
	if(!(m_hPool && m_pmpi)){
		return NULL;
	}
	
	return m_pmpi->AllocInit(m_hPool,size,InitVal);
}

inline void CJobPkg_imp::Free(PVOID pv){
	assert(m_hPool && m_pmpi);
	if(!(m_hPool && m_pmpi)){
		return;
	}
	m_pmpi->Free(m_hPool,pv);
}

inline UINT CJobPkg_imp::GetPriroity(){
	return m_uPriroity;
}

inline UINT CJobPkg_imp::GetSchedule(){
	return m_uSchedule;
}

bool CJobPkg_imp::SetPriroity(UINT uPri){
	assert(m_pjn);
	assert(uPri>=JP_PRI_HEIGHEST && uPri<=JP_PRI_HEIGHEST+JP_PRI_LEVELCOUNT);
	if(!(uPri>=JP_PRI_HEIGHEST && uPri<=JP_PRI_HEIGHEST+JP_PRI_LEVELCOUNT)){
		return false;
	}

	bool bRet=true;
	if(m_bSubmitted){
		bRet=m_pjn->SetJobPriroity(this,uPri);
	}
	if(bRet){
		m_uPriroity=uPri;
	}
	return bRet;
}

bool CJobPkg_imp::SetSchedule(UINT uSch){
	//本功能目前暂不支持
	return false;
}

inline void CJobPkg_imp::SetExecFunction(JOBFUNCTION pfun){
	m_pExecFunc=pfun;
}

inline void CJobPkg_imp::SetExecParam(PVOID pvParam){
	m_pvParam=pvParam;
}

inline void CJobPkg_imp::ExecJob(){
	assert(m_pExecFunc!=NULL);
	m_pExecFunc(m_pvParam);
}

void* CJobPkg_imp::operator new(SIZE size,SIZE initsize,CMemPoolInterface* pmpi){
	assert(pmpi);
	if(!pmpi){
		throw(ERR_INVALID_PARAMETER);
	}

	HANDLE hpool=pmpi->CreatePool(size+initsize);
	assert(hpool);
	if(!hpool){
		throw(ERR_POOL_CREATE_FAILED);
	}

	void* pv=pmpi->Alloc(hpool,size);
	if(pv){
		((CJobPkg_imp*)pv)->m_hPool=hpool;
		((CJobPkg_imp*)pv)->m_pmpi=pmpi;
	}
	else{
		assert(pv);
		throw(ERR_NOT_ENOUGH_MEMORY);
	}
	return pv;
}

//不要通过本函数删除任务包对象，应该通过Delete方法成员
void CJobPkg_imp::operator delete(void* pv){
	//不需要做任何事，后续被调用的析构函数将负责释放内存
	return;
}

void CJobPkg_imp::Delete(){
	//必需用下面的语法才能保证正常运行
	//否则会偶然发生纯虚函数调用错误（找不到vptr）
	this->CJobPkg_imp::~CJobPkg_imp();
}

DWORD CJobPkg_imp::SetID(DWORD dwID){
	DWORD dwtmp=m_dwID;
	m_dwID=dwID;
	return dwtmp;
}
DWORD CJobPkg_imp::GetID(){
	return m_dwID;
}


/*
	CJobNest_imp类实现代码
*/
CJobNest::CJobNest(){
}
CJobNest::~CJobNest(){
}

CJobNest_imp::CJobNest_imp(){
	m_bExit=false;
	m_bWorkerExist=false;
	memset(m_pJobPkgs,0,sizeof(m_pJobPkgs[0])*JP_PRI_LEVELCOUNT);
	memset(m_pJobPkgEnd,0,sizeof(m_pJobPkgEnd[0])*JP_PRI_LEVELCOUNT);
	m_thWorker=NULL;
	m_uJobCount=0;
	m_pUnSubmitPkgs=NULL;
}

CJobNest_imp::~CJobNest_imp(){
	//清除已提交的任务包
	Empty();

	//清除未提交任务包
	m_mtxUnSubmitPkgGuard.Lock();

	CJobPkg_imp* p;
	while(m_pUnSubmitPkgs && (p=(CJobPkg_imp*)m_pUnSubmitPkgs->GetNext())!=NULL){
		p->RemoveOut();
		p->Delete();
	}
	if(m_pUnSubmitPkgs){
		m_pUnSubmitPkgs->Delete();
		m_pUnSubmitPkgs=NULL;
	}

	m_mtxUnSubmitPkgGuard.UnLock();

	//退出工作线程,若返回true,表示工作线程原本存在,必须将其进行join操作,释放资源
	//若返回的是false,表示工作线程原本就不存在,不需要进行join操作
	if(ExitWorker()){
		//rick:debug
//		dbout("thread[%d]: Exiting work thread...",pthread_self());
		pthread_join(m_thWorker,NULL);	//等待线程退出
		//rick:debug
//		dbout("thread[%d]: Work thread exited.",pthread_self());
	}
}

//清除所有已经提交和未提交的任务包
void CJobNest_imp::Empty(){
	//清除已提交任务包
	m_evtWakeUp.Lock();

	for(int i=0;i<JP_PRI_LEVELCOUNT;i++){
		if(m_pJobPkgs[i]){
			CJobPkg_imp* p;
			while((p=(CJobPkg_imp*)m_pJobPkgs[i]->GetNext())!=NULL){
				p->RemoveOut();
				p->Delete();
			}
			m_pJobPkgs[i]->Delete();
			m_pJobPkgs[i]=NULL;
		}
		m_pJobPkgEnd[i]=NULL;
	}

	m_uJobCount=0;
	m_evtWakeUp.UnLock();
}

SIZE CJobNest_imp::GetMemPoolFreeSize(){
	return m_mpi.GetFreeMemSizeInPoolManeger();
}
SIZE CJobNest_imp::GetTotalMemPoolSize(){
	return m_mpi.GetTotalMemSizeInPoolManeger();
}

//令任务线程退出
bool CJobNest_imp::ExitWorker(){
	m_evtWakeUp.Lock();
	bool bRet=m_bWorkerExist;
	if(bRet){
		m_bExit=true;
		m_evtWakeUp.SetEvent();
	}
	m_evtWakeUp.UnLock();
	return bRet;
}

//从任务队列中取出任务包
//任务调度策略在此函数中实现
CJobPkg_imp* CJobNest_imp::GetPkg(){
	CJobPkg_imp* pkg=NULL;
	for(UINT uPri=JP_PRI_HEIGHEST;uPri<JP_PRI_HEIGHEST+JP_PRI_LEVELCOUNT;uPri++){
		if(m_pJobPkgs[uPri]){
			pkg=m_pJobPkgs[uPri];
			m_pJobPkgs[uPri]=(CJobPkg_imp*)pkg->GetNext();
			
			if(m_pJobPkgEnd[uPri]==pkg){
				m_pJobPkgEnd[uPri]=NULL;
			}
			pkg->RemoveOut();
			m_uJobCount--;

			break;
		}
	}
	return pkg;
}

//提取已提交的任务队列中下一个任务包
bool CJobNest_imp::GetNextPkg(CJobPkg_imp** ppJobPkg){
	bool bRetVal=true;
	//rick:debug
//	dbout("thread[%d]:******GetNextPkg event lock",pthread_self());
	m_evtWakeUp.Lock();

	//若系统已经要求退出工作线程，则返回false
	//否则检查任务队列，为空时挂起自身
	//使用循环以避免某些系统的线程会自动苏醒的问题，提高可移植性
	while(!m_bExit && !(m_uJobCount)){
     	//rick:debug
//     	dbout("thread[%d]:******GetNextPkg event wait",pthread_self());
		m_evtWakeUp.Wait();		//挂起
	}
	
	//苏醒后先检查是否要求退出线程
	if(m_bExit){
		bRetVal=false;
	}
	//否则就是任务队列中有新的任务包，提取之
	else{
		*ppJobPkg=GetPkg();
		//若没有任务包，则是内部异常，返回失败
		assert(*ppJobPkg);
		if(!*ppJobPkg){
			bRetVal=false;
		}
	}
	
	//rick:debug
//	dbout("thread[%d]:******GetNextPkg event unlock",pthread_self());
	m_evtWakeUp.UnLock();

	return bRetVal;
}

//创建新的任务包，并链入未提交任务包链
CJobPkg* CJobNest_imp::GetJobPkg(SIZE initsize){
	CJobPkg_imp* pjpg=new(initsize,&m_mpi) CJobPkg_imp(this);
	assert(pjpg);
	if(pjpg){
		m_mtxUnSubmitPkgGuard.Lock();

		pjpg->InsertBefore(m_pUnSubmitPkgs);
		m_pUnSubmitPkgs=pjpg;

		m_mtxUnSubmitPkgGuard.UnLock();
	}
	return (CJobPkg*)pjpg;
}

void CJobNest_imp::ReleaseJobPkg(CJobPkg* pjpg){
	if(!pjpg){
		return;
	}
	assert(!pjpg->IsSubmitted());
	if(pjpg->IsSubmitted()){
		return;
	}

	CJobPkg_imp* p=(CJobPkg_imp*)pjpg->QueryIF();
   	
	//rick:debug
//	dbout("thread[%d]:******ReleaseJobPkg unsubmitguard lock",pthread_self());
   	m_mtxUnSubmitPkgGuard.Lock();
		
	bool bInQueue=(p->GetNext() || p->GetPrev() || m_pUnSubmitPkgs==p);	

   	if(m_pUnSubmitPkgs==p){
   		m_pUnSubmitPkgs=(CJobPkg_imp*)p->GetNext();
   	}
   	p->RemoveOut();

	//rick:debug
//	dbout("thread[%d]:******ReleaseJobPkg unsubmitguard unlock",pthread_self());
   	m_mtxUnSubmitPkgGuard.UnLock();
   		
   	if(bInQueue){
   		p->Delete();                    
   	}
}

bool CJobNest_imp::Resume(){
	//暂不支持该功能
	return false;
}

//调换任务包到指定优先级的任务队列
bool CJobNest_imp::SetJobPriroity(CJobPkg_imp* pJobPkg, UINT uPri){
	assert(pJobPkg && pJobPkg->IsSubmitted());
	if(!pJobPkg || !pJobPkg->IsSubmitted()){
		return false;
	}

	m_evtWakeUp.Lock();

	if(m_pJobPkgs[pJobPkg->GetPriroity()]==pJobPkg){
		m_pJobPkgs[pJobPkg->GetPriroity()]=(CJobPkg_imp*)pJobPkg->GetNext();
	}
	if(m_pJobPkgEnd[pJobPkg->GetPriroity()]==pJobPkg){
		m_pJobPkgEnd[pJobPkg->GetPriroity()]=(CJobPkg_imp*)pJobPkg->GetPrev();
	}
	pJobPkg->RemoveOut();

	if(!m_pJobPkgs[uPri]){
		m_pJobPkgs[uPri]=m_pJobPkgEnd[uPri]=pJobPkg;
	}
	else{
		assert(m_pJobPkgEnd[uPri]);
		pJobPkg->InsertAfter(m_pJobPkgEnd[uPri]);
		m_pJobPkgEnd[uPri]=pJobPkg;
	}

	m_evtWakeUp.UnLock();

	return true;
}

bool CJobNest_imp::SetJobSchedule(CJobPkg_imp* pJobPkg, UINT uSch){
	//暂不支持该功能
	return false;
}

bool CJobNest_imp::SubmitJobPkg(CJobPkg* pjpg){
	if(!pjpg){
		return false;
	}
	
	assert(!pjpg->IsSubmitted());
	if(pjpg->IsSubmitted()){
		return false;
	}

	CJobPkg_imp* p=(CJobPkg_imp*)pjpg->QueryIF();
	
	//rick:debug
//	dbout("thread[%d]:******SubmitJobPkg unsubmitguard lock",pthread_self());
	m_mtxUnSubmitPkgGuard.Lock();
	if(m_pUnSubmitPkgs==p){
		m_pUnSubmitPkgs=(CJobPkg_imp*)p->GetNext();
	}
	p->RemoveOut();
	
	//rick:debug
//	dbout("thread[%d]:******SubmitJobPkg unsubmitguard unlock",pthread_self());
	m_mtxUnSubmitPkgGuard.UnLock();

	//rick:debug
//	dbout("thread[%d]:******SubmitJobPkg event lock",pthread_self());
	m_evtWakeUp.Lock();

	//若ID不为0，释放相同ID的已提交任务包
	if(p->GetID()!=0){
		for(int i=JP_PRI_HEIGHEST;i<JP_PRI_HEIGHEST+JP_PRI_LEVELCOUNT;i++){
			if(m_pJobPkgs[i]){
				CJobPkg_imp* pold=m_pJobPkgs[i];
				while(pold && pold->GetID()!=p->GetID()){
					pold=(CJobPkg_imp*)pold->GetNext();
				}
				if(pold){
					if(m_pJobPkgs[i]==pold){
						m_pJobPkgs[i]=(CJobPkg_imp*)pold->GetNext();
					}
					if(m_pJobPkgEnd[i]==pold){
						m_pJobPkgEnd[i]=(CJobPkg_imp*)pold->GetPrev();
					}
					
					pold->RemoveOut();
					pold->Delete();
					m_uJobCount--;
					break;
				}
			}
		}
	}

	//插入新包
	p->InsertAfter(m_pJobPkgEnd[p->GetPriroity()]);
	m_pJobPkgEnd[p->GetPriroity()]=p;
	if(!m_pJobPkgs[p->GetPriroity()]){
		m_pJobPkgs[p->GetPriroity()]=p;
	}

	m_uJobCount++;
    p->SetSubmit(true);
  	
	//rick:debug
//	dbout("thread[%d]:******SubmitJobPkg event unlock",pthread_self());
	m_evtWakeUp.UnLock();
	
	return WakeUpWorker();
}

bool CJobNest_imp::Suspend(){
	//暂不支持该功能
	return false;
}

bool CJobNest_imp::WakeUpWorker(){
	bool bRetVal=true;
	int iState=SUCCESS;

	m_evtWakeUp.Lock();

	//若工作线程未创建，则创建工作线程
	if(!m_bWorkerExist){
		m_bExit=false;

		pthread_attr_t attr;
		iState=pthread_attr_init(&attr);
		assert(SUCCESS==iState);
		iState=pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
		assert(SUCCESS==iState);
		iState=pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
		assert(SUCCESS==iState);

		if(SUCCESS!=iState){
			bRetVal=false;
		}
		else{
			m_bWorkerExist=true;
			iState=pthread_create(&m_thWorker,&attr,WorkerThread,this);
			assert(SUCCESS==iState);
			if(SUCCESS!=iState){
				m_bWorkerExist=false;
				bRetVal=false;
			}
		}
		pthread_attr_destroy(&attr);
	}
	//若工作线程已经创建，则唤醒工作线程
	else{
		m_evtWakeUp.SetEvent();
	}

	m_evtWakeUp.UnLock();
	return bRetVal;
}

//工作线程函数
void* CJobNest_imp::WorkerThread(void* pvParam){
	assert(pvParam);

	CJobNest_imp* pthis=(CJobNest_imp*)pvParam;
	
	CJobPkg_imp* pkg=NULL;
	while(pthis->GetNextPkg(&pkg)){
		assert(pkg);
		if(NULL == pkg){
			break;
		}
		
		//执行任务包
		pkg->ExecJob();
        
		//rick:debug
//        dbout("thread[%d]:........ExecJob ok........",pthread_self());
        
		pkg->Delete();
		//rick:debug
//        dbout("thread[%d]:........Delete pkg ok........",pthread_self());
		
		pkg=NULL;
	}

	//退出清理
	if(pkg){
		pkg->Delete();
	}
	pthis->m_evtWakeUp.Lock();

	//若不是被要求退出线程，则证明本线程发生异常而导致退出，
	//将本线程脱离原调用线程，避免产生僵尸
	if(!pthis->m_bExit){
		pthread_detach(pthread_self());
	}
	else{
		pthis->m_bExit=false;
	}
	pthis->m_bWorkerExist=false;

	pthis->m_evtWakeUp.UnLock();

	//rick:debug
//    dbout("thread[%d]: i am exiting...",pthread_self());
}

//end of file
