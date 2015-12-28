#pragma once

#include "MemPooler.h"


#include <vector>

template<class Type>
class CQueue
{
// Public Method
public:
   // Constructor and Destructor
	CQueue(int iQueueSize):m_iQueueSize(0), m_pLast(0), m_pPooler(0)
   {
	   m_pPooler = new MemPooler<SQueue>(iQueueSize);
	
	   Create(iQueueSize);
   }
   ~CQueue()
   {
       Destroy();	   
   }

   // Push ���� - ť�� �� �ڸ��� ������ �ڸ� ���涧���� ��ŷ 
   void Blocking_Push(Type PushingData)
   {
	   Push(PushingData,INFINITE);
   }

   // Pop ���� - ť�� �������� ������ ��ŷ
   BOOL Blocking_Pop(Type* PopingData)
   {
	   return Pop(PopingData,INFINITE);
   }

   // Push ���� - ť�� �� �ڸ��� ������ Ÿ�Ӿƿ� �ð����� ��ŷ, �ڸ� ����� �ְ� ����
   BOOL Blocking_Push_Timeout(Type PushingData,DWORD dwTimeout)
   {
	  return Push(PushingData,dwTimeout);
   }

   // Pop ���� - ť�� �������� ������ Ÿ�Ӿƿ� �ð����� ��ŷ, ������ ������ ����
   BOOL Blocking_Pop_Timeout(Type* PopingData, DWORD dwTimeout)
   {
	  return Pop(PopingData,dwTimeout);
   }

   // Push ���� - ť�� �ڸ��� ������ �ְ� �ڸ� ������ �ٷ� FALSE��� ����� �Բ� ����
   BOOL NonBlocking_Push(Type PushingData)
   {	   
	   return Push(PushingData,0);
   }
   
   // Pop ���� - ť�� �������� ������ ������ ���� TRUE ����, ������ ������ �ٷ� FALSE �� �Բ� ���� 
   BOOL NonBlocking_Pop(Type* PopingData)
   {
	   return Pop(PopingData,0);
   }

   int GetQueueSize()
   {
	   return m_iQueueSize; 
   }

// Private Method   
private:


	struct SQueue
	{
		SQueue *pParent;
		Type	pData;
	};


	// �ʱ�ȭ, IOCP ����, �������� �ʱ�ȭ
	void Create(int iQueueSize)   
	{
		m_hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
		::InitializeCriticalSection(&cs);





	}
	// �ڿ�����, IOCP ��ü �� �������� ����
	void Destroy()  
	{
	   if(m_hEvent!=NULL)
	   {
		   CloseHandle(m_hEvent);
		   m_hEvent=NULL;
	   }

	   ::DeleteCriticalSection(&cs);
	}

	// Push Operation
	BOOL Push(Type& data, DWORD dwTimeout)
	{
		   EnterCriticalSection( &cs );

		   SQueue* pCurrent = m_pPooler->Alloc();

		   if(!pCurrent)
		   {
			   LeaveCriticalSection(&cs);
			   return FALSE;
		   }

		   if(!m_pLast)
		   {
			   pCurrent->pParent = NULL;
			   pCurrent->pData = data;
			   m_pLast = pCurrent;
			   m_pLastlink = pCurrent;
		   }
		   else
		   {
			   pCurrent->pData = data;
			   pCurrent->pParent = NULL;
			   
			   m_pLastlink->pParent = pCurrent;

			   m_pLastlink = pCurrent;
		   }

		   m_iQueueSize++;
 
		   LeaveCriticalSection(&cs);

		   if(m_iQueueSize)
			   SetEvent(m_hEvent);

		   return TRUE;
	   //}
	   //else 
	   //{ 
		  // return FALSE;
	   //}
	}

	// Pop Operation
	BOOL Pop(Type* retData,DWORD dwTimeout)
	{

		DWORD ret;

		ret =  WaitForSingleObject(m_hEvent, dwTimeout);

		if(ret != WAIT_OBJECT_0)
            return FALSE;


	   EnterCriticalSection( &cs );

	   if(m_pLast)
       {

		   SQueue* temp = m_pLast;
           m_iQueueSize--;

           *retData = temp->pData;

		   m_pLast = temp->pParent;

		   m_pPooler->Free(temp);

		   LeaveCriticalSection(&cs);


		   if(!m_iQueueSize)
			   ResetEvent(m_hEvent);

		   
		   return TRUE;
	   }

	   LeaveCriticalSection(&cs);
	
	   return FALSE;



	}

// Private Member
private:

	HANDLE					m_hEvent;
	CRITICAL_SECTION		cs;
	int						m_iQueueSize;

	SQueue*					m_pLast;
	SQueue*					m_pLastlink;

	MemPooler<SQueue>*  m_pPooler;

};