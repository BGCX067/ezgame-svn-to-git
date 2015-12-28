#pragma once
#include <assert.h>

// Edited from C8MemPool class (Hitel, gma 7 3) - 1CoinClear -

template<class Type>
class MemPooler
{

public:
	// ������ 
	MemPooler(int nNumOfBlock):m_nNumofBlock(nNumOfBlock),
		                       m_pFreeList(NULL),
							   m_pMemBlock(NULL),
							   m_nAllocCount(0)
	{
		assert(nNumOfBlock>0);
		m_nListBlockSize=sizeof(BlockNode)+sizeof(Type);
		
		Create();
	}

    // �Ҹ���
	~MemPooler()
	{
		Destroy();
	}

	// �޸� �Ҵ� 
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
		
        const size_t AllocationSize=(m_nListBlockSize) * m_nNumofBlock; // �޸� �Ҵ��� ũ�� 

		m_pMemBlock=HeapAlloc(GetProcessHeap(),	HEAP_ZERO_MEMORY, AllocationSize);

		assert(m_pMemBlock);

		BlockNode* pNode=reinterpret_cast<BlockNode*>(m_pMemBlock);

		pNode =reinterpret_cast<BlockNode*>((reinterpret_cast<INT64>(pNode))+(m_nNumofBlock-1)* (m_nListBlockSize) );
		for(INT64 i=m_nNumofBlock-1; i>=0; i--)
		{
			pNode->pNext=m_pFreeList; // ó������ NULL , �� Tail �� NULL �� �Ѵ�.
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
	// ��ũ�� ����Ʈ ó�� ������ ���� ��� Ÿ��
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
	BlockNode* m_pFreeList; // ���� �ִ� �޸� �� ����Ʈ
	void* m_pMemBlock;

	size_t   m_nNumofBlock;      // �޸� �Ҵ��� �� ��
	size_t  m_nListBlockSize;   // �� �� ������ 
	size_t   m_nAllocCount;      // �Ҵ�� �޸� �� ����
};