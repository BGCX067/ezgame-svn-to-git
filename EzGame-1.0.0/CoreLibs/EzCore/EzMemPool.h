#pragma once

#include "EzSet.h"
#include "EzAllocator.h"
#include "EzMemObject.h"


template <class T, typename TAlloc = EzAllocatorInterface<T> >
class EZCORE_ENTRY EzMemPoolContainer : public EzMemObject
{
public:
	EzMemPoolContainer(unsigned int uiSize);
	~EzMemPoolContainer();

	T* getObject(unsigned int uiIndex);

	void setNext(EzMemPoolContainer<T, TAlloc>* pNext);
protected:
	T* m_pObjectArray;
	unsigned int m_uiSize;

	EzMemPoolContainer<T,TAlloc>* m_pNext;

private:
	// To prevent an application from inadvertently causing the compiler to
	// generate the default copy constructor or default assignment operator,
	// these methods are declared as private. They are not defined anywhere,
	// so code that attempts to use them will not link.
	EzMemPoolContainer(const EzMemPoolContainer&);
	EzMemPoolContainer& operator=(const EzMemPoolContainer&);
};


template <class T, typename TAlloc>
inline EzMemPoolContainer<T,TAlloc>::EzMemPoolContainer(unsigned int uiSize)
: m_uiSize(uiSize), m_pNext(NULL)
{
	if (uiSize > 0)
	{
		m_pObjectArray = TAlloc::allocate(uiSize);
		EZASSERT(m_pObjectArray != NULL);
	}
	else
	{
		m_pObjectArray = NULL;
	}
}

template <class T, typename TAlloc>
inline EzMemPoolContainer<T,TAlloc>::~EzMemPoolContainer()
{
	TAlloc::deallocate(m_pObjectArray);
	EzDelete m_pNext;
}

template <class T, typename TAlloc> 
inline T* EzMemPoolContainer<T,TAlloc>::getObject(unsigned int uiIndex)
{
	if (uiIndex >= m_uiSize)
		return NULL;
	return &m_pObjectArray[uiIndex];
}

template <class T, typename TAlloc>
inline void EzMemPoolContainer<T,TAlloc>::setNext(EzMemPoolContainer<T,TAlloc>* pkNext)
{
	m_pNext = pkNext;
}

template <class T, typename TAlloc = EzAllocatorInterface<T> >
class EzMemPool  : public EzMemObject
{
public:
	EzMemPool(unsigned int uiInitialSize = 8);
	~EzMemPool();

	T* alloc();
	void free(T* pkObject);
	void clear();

protected: 
	void createNewObjects(unsigned int uiSize);

	EzSet<T*>							m_FreeObjects;
	EzMemPoolContainer<T, TAlloc>*		m_pContainers;

	unsigned int m_uiCurrentSize;
	unsigned int m_uiInitialSize;

private:
	// To prevent an application from inadvertently causing the compiler to
	// generate the default copy constructor or default assignment operator,
	// these methods are declared as private. They are not defined anywhere,
	// so code that attempts to use them will not link.
	EzMemPool(const EzMemPool&);
	EzMemPool& operator=(const EzMemPool&);
};


template <class T, typename TAlloc> 
inline EzMemPool<T,TAlloc>::EzMemPool(unsigned int uiInitialSize) :
m_pContainers(NULL),
m_uiCurrentSize(uiInitialSize),
m_uiInitialSize(uiInitialSize)
{
	EZASSERT(uiInitialSize > 0);
}
template <class T, typename TAlloc> 
inline EzMemPool<T,TAlloc>::~EzMemPool()
{
	m_FreeObjects.clear();
	EzDelete m_pContainers;
}

template <class T, typename TAlloc> 
inline T* EzMemPool<T,TAlloc>::alloc()
{
	if (m_FreeObjects.size() == 0)
	{
		createNewObjects(m_uiCurrentSize);
		m_uiCurrentSize *= 2;
	}

	T* pReturn = m_FreeObjects[0];

	T e;
	TAlloc::construct(pReturn, e);

	m_FreeObjects.remove(0);




	return pReturn;
}


template <class T, typename TAlloc> 
inline void EzMemPool<T,TAlloc>::createNewObjects(unsigned int uiSize)
{
	EzMemPoolContainer<T,TAlloc>* pNewContainer = EzNew EzMemPoolContainer<T,TAlloc>(uiSize);
	for (unsigned int i = 0; i < uiSize; i++)
	{
		m_FreeObjects.add(pNewContainer->getObject(i));
	}

	pNewContainer->setNext(m_pContainers);
	m_pContainers = pNewContainer;
}


template <class T, typename TAlloc> 
inline void EzMemPool<T,TAlloc>::free(T* pObject)
{

	TAlloc::destruct(pObject);

	m_FreeObjects.add(pObject);
}


template <class T, typename TAlloc> 
inline void EzMemPool<T,TAlloc>::clear()
{
	m_FreeObjects.clear();
	EzDelete m_pContainers;
	m_pContainers = NULL;
	m_uiCurrentSize = m_uiInitialSize;
}