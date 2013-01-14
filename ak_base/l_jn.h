/*-----------------------------------------------------------------------*\
	file:		l_jn.h
	module:		jobnest for linux
	descript:	��ģ���ṩLinux�µ�����(���߳��������)����,�û��������
				�������ļ�����ʹ������ģ��
	by:			Rick Wang
	date:		2002/3
\*-----------------------------------------------------------------------*/
#ifndef L_JN_H_2E2076BC_B0C1_4a98_889F_8DAABC69D62E
#define L_JN_H_2E2076BC_B0C1_4a98_889F_8DAABC69D62E
#include "rkTypes.h"
/*---------------------------------------
	���ų�������
---------------------------------------*/
//��������ȼ�
#define JP_PRI_TOP			0u		//���
#define JP_PRI_HIGH			1u		//��
#define JP_PRI_NORMAL		2u		//ͨ��
#define JP_PRI_LOW			3u		//��
#define JP_PRI_IDEL			4u		//���

//�쳣�������
#define ERR_INVALID_PARAMETER	0x2010u		//�Ƿ�����
#define ERR_POOL_CREATE_FAILED	0x2011u		//�ڴ�ش���ʧ��

/*---------------------------------------
	�������ͼ��ṹ����
---------------------------------------*/
/*
	����ִ�к���
*/
typedef void (*JOBFUNCTION)(PVOID pvParam);

/*---------------------------------------
	�ඨ��
---------------------------------------*/
/*
	������ӿ���: CJobPkg
*/
class CJobPkg{
private:
protected:
public:
	CJobPkg();
	virtual ~CJobPkg()=0;

	//�����ڴ��
	virtual PVOID Alloc(SIZE size)=0;
	virtual PVOID Alloc(SIZE size, UCHAR InitVal)=0;

	//�ͷ��ڴ��
	//ע�⣺�ڱ��ڴ��֮��������ڴ�齫һ��������
	virtual void Free(PVOID pv)=0;

	//��������ִ��ʱ�������̵߳������ȼ�����
	virtual bool SetSchedule(UINT uSch)=0;

	//��ȡ��ǰ������̵߳������ȼ�����
	virtual UINT GetSchedule()=0;

	//������������ȼ�
	virtual bool SetPriroity(UINT uPri)=0;

	//��ȡ��ǰ��������ȼ�
	virtual UINT GetPriroity()=0;

	//��������ִ�к���
	virtual void SetExecFunction(JOBFUNCTION pfun)=0;

	//��������ִ�к����Ĳ���
	virtual void SetExecParam(PVOID pvParam)=0;
	
	//���������ID�����Ѿ��ύ�������������ͬID�����������
	//�������������ʹ�䱻ɾ����������IDΪ0
	//����ԭ����IDֵ
	virtual DWORD SetID(DWORD dwID)=0;
	virtual DWORD GetID()=0;

	virtual bool IsSubmitted()=0;

	//�ڲ����ߺ��������û���Ч
	virtual void* QueryIF()=0;
};

/*
	������: CJobNest
*/
class CJobNest{
private:
protected:
public:
	CJobNest();
	virtual ~CJobNest()=0;

	//�������(����Ѿ��ύ�����������)
	virtual void Empty()=0;
	//�������񳲵�����
	//virtual bool Suspend()=0;
	//�ָ����񳲵�����
	//virtual bool Resume()=0;

	//����һ����������������ָ��,������SubmitJobPkg��ReleaseJobPkg�����ɶ�ʹ��
	virtual CJobPkg* GetJobPkg(SIZE initsize)=0;			
	//�ύһ�����������(���������ִ�к��Զ����ͷţ��Ѿ��ͷŵİ������ύ
	//�������ǲ�ȷ����)
	//�������д�����ͬID�����ύ�������������������ɾ��
	virtual bool SubmitJobPkg(CJobPkg* pjpg)=0;				
	//�ͷ�һ������������Ӧ��(�Ѿ��ύ�İ������ͷţ��������ǲ�ȷ����)
	//ע��,���ͷź�,�û�����Ͳ�����ʹ�ø����������ָ��,�������ǲ�ȷ����
	virtual void ReleaseJobPkg(CJobPkg* pjpg)=0;
	//��ȡ�ڴ�ع�������ǰռ�õ����ڴ��ڴ�����
	virtual SIZE GetTotalMemPoolSize()=0;
	//��ȡ�ڴ�ع�������ǰ�����ڴ�����
	virtual SIZE GetMemPoolFreeSize()=0;
};

extern CJobNest* GetJobNest();
extern void ReleaseJobNest(CJobNest* pjn);

#endif //#ifndef L_JN_H_2E2076BC_B0C1_4a98_889F_8DAABC69D62E
