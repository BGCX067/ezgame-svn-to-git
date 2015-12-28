#pragma once
#include <assert.h>

// Edited from C8MemPool class (Hitel, gma 7 3) - 1CoinClear -

template<class Type>
class MemPooler
{

public:
	// 생성자 
	MemPooler(int nNumOfBlock):m_nNumofBlock(nNumOfBlock),
		                       m_pFreeList(NULL),
							   m_pMemBlock(NULL),
							   m_nAllocCount(0)
	{
		assert(nNumOfBlock>0);
		m_nListBlockSize=sizeof(BlockNode)+sizeof(Type);
		
		Create();
	}

    // 소멸자
	~MemPooler()
	{
		Destroy();
	}

	// 메모리 할당 
	Type* Alloc()
	{
	    BlockNode* pNode=NULL;
		Type* pRet=NULL;

        pNode=m_pFreeList;
		if(pNode!=NULL)
		{
			m_pFreeList=m_pFreeList->pNext;
			m_nAllocCount++;
			pRet=reinterpret_cast<Type*>(pNode+1);
		}


		return pRet;
	}

	bool Free(Type* freeBlock)
	{
		BlockNode* pNode=NULL;
        bool bRet=false;


		pNode=( reinterpret_cast<BlockNode*>(freeBlock) ) - 1;
		if(m_nAllocCount>0)
		{
			pNode->pNext=m_pFreeList;
			m_pFreeList=pNode;
			m_nAllocCount--;
            bRet=true;
		}

		return bRet;
	}

	UINT GetCount()
	{
		return (UINT) m_nAllocCount;
	}
	
		
protected:
     void Create()
	 {
		
        const size_t AllocationSize=(m_nListBlockSize) * m_nNumofBlock; // 메모리 할당할 크기 

		m_pMemBlock=HeapAlloc(GetProcessHeap(),	HEAP_ZERO_MEMORY, AllocationSize);

		assert(m_pMemBlock);

		BlockNode* pNode=reinterpret_cast<BlockNode*>(m_pMemBlock);

		pNode =reinterpret_cast<BlockNode*>((reinterpret_cast<INT64>(pNode))+(m_nNumofBlock-1)* (m_nListBlockSize) );
		for(INT64 i=m_nNumofBlock-1; i>=0; i--)
		{
			pNode->pNext=m_pFreeList; // 처음에는 NULL , 즉 Tail 은 NULL 로 한다.
			m_pFreeList=pNode;
			pNode=reinterpret_cast<BlockNode*>((reinterpret_cast<INT64>(pNode)) - m_nListBlockSize);
		}

	 }

	 void Destroy()
	 {        
		if(m_pMemBlock)
		{
			HeapFree(GetProcessHeap(),	HEAP_ZERO_MEMORY, m_pMemBlock);
		}
	 }



    ///////////////////////////////////////////
	// 링크드 리스트 처럼 관리를 위한 노드 타입
	struct BlockNode
	{
		BlockNode* pNext;
		BlockNode()
		{
			pNext=NULL;
		}
	};
	//////////////////////////////////////////

protected:
	BlockNode* m_pFreeList; // 남아 있는 메모리 블럭 리스트
	void* m_pMemBlock;

	size_t   m_nNumofBlock;      // 메모리 할당할 블럭 수
	size_t  m_nListBlockSize;   // 한 블럭 사이즈 
	size_t   m_nAllocCount;      // 할당된 메모리 블럭 갯수
};