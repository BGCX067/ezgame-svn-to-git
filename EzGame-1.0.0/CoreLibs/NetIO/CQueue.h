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

   // Push 동작 - 큐에 빈 자리가 없으면 자리 생길때까지 블러킹 
   void Blocking_Push(Type PushingData)
   {
	   Push(PushingData,INFINITE);
   }

   // Pop 동작 - 큐에 아이템이 없으면 블러킹
   BOOL Blocking_Pop(Type* PopingData)
   {
	   return Pop(PopingData,INFINITE);
   }

   // Push 동작 - 큐에 빈 자리가 없으면 타임아웃 시간까지 블러킹, 자리 생기면 넣고 리턴
   BOOL Blocking_Push_Timeout(Type PushingData,DWORD dwTimeout)
   {
	  return Push(PushingData,dwTimeout);
   }

   // Pop 동작 - 큐에 아이템이 없으면 타임아웃 시간까지 블러킹, 아이템 있으면 리턴
   BOOL Blocking_Pop_Timeout(Type* PopingData, DWORD dwTimeout)
   {
	  return Pop(PopingData,dwTimeout);
   }

   // Push 동작 - 큐에 자리가 있으면 넣고 자리 없으면 바로 FALSE라는 결과와 함께 리턴
   BOOL NonBlocking_Push(Type PushingData)
   {	   
	   return Push(PushingData,0);
   }
   
   // Pop 동작 - 큐에 아이템이 있으면 가지고 오고 TRUE 리턴, 아이템 없으면 바로 FALSE 와 함께 리턴 
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


	// 초기화, IOCP 생성, 세마포어 초기화
	void Create(int iQueueSize)   
	{
		m_hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
		::InitializeCriticalSection(&cs);





	}
	// 자원정리, IOCP 객체 및 세마포어 정리
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