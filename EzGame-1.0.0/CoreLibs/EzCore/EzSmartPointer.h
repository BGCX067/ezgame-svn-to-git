#ifndef NISMARTPOINTER_H
#define NISMARTPOINTER_H


#include "EzMemObject.h"

//The class T must be derived from EzRefObject.
template <class T> 
class EzSmartPointer : public EzMemObject
{
public:
	EzSmartPointer(T* pObject = (T*) 0);
	EzSmartPointer(const EzSmartPointer& ptr);
	~EzSmartPointer();

	operator T*() const;
	T& operator*() const;
	T* operator->() const;

	EzSmartPointer& operator=(const EzSmartPointer& ptr);
	EzSmartPointer& operator=(T* pObject);

	bool operator==(T* pObject) const;
	bool operator!=(T* pObject) const;
	bool operator==(const EzSmartPointer& ptr) const;
	bool operator!=(const EzSmartPointer& ptr) const;

protected:
	T* m_pObject;
};



template <class T>
inline EzSmartPointer<T>::EzSmartPointer(T* pObject)
{
	m_pObject = pObject;
	if (m_pObject)
		m_pObject->addRef();
}

template <class T>
inline EzSmartPointer<T>::EzSmartPointer(const EzSmartPointer& ptr)
{
	m_pObject = ptr.m_pObject;
	if (m_pObject)
		m_pObject->addRef();
}

template <class T>
inline EzSmartPointer<T>::~EzSmartPointer()
{
	if (m_pObject)
		m_pObject->release();
}

template <class T>
inline EzSmartPointer<T>::operator T*() const
{
	return m_pObject;
}

template <class T>
inline T& EzSmartPointer<T>::operator*() const
{
	return *m_pObject;
}

template <class T>
inline T* EzSmartPointer<T>::operator->() const
{
	return m_pObject;
}

template <class T>
inline EzSmartPointer<T>& EzSmartPointer<T>::operator=(const EzSmartPointer& ptr)
{
	if (m_pObject != ptr.m_pObject)
	{
		if (m_pObject)
			m_pObject->release();
		m_pObject = ptr.m_pObject;
		if (m_pObject)
			m_pObject->addRef();
	}
	return *this;
}

template <class T>
inline EzSmartPointer<T>& EzSmartPointer<T>::operator=(T* pObject)
{
	if (m_pObject != pObject)
	{
		if (m_pObject)
			m_pObject->release();
		m_pObject = pObject;
		if (m_pObject)
			m_pObject->addRef();
	}
	return *this;
}

template <class T>
inline bool EzSmartPointer<T>::operator==(T* pObject) const
{
	return (m_pObject == pObject);
}

template <class T>
inline bool EzSmartPointer<T>::operator!=(T* pObject) const
{
	return (m_pObject != pObject);
}

template <class T>
inline bool EzSmartPointer<T>::operator==(const EzSmartPointer& ptr) const
{
	return (m_pObject == ptr.m_pObject);
}

template <class T>
inline bool EzSmartPointer<T>::operator!=(const EzSmartPointer& ptr) const
{
	return (m_pObject != ptr.m_pObject);
}


#define EzSmartPointer(classname) \
class classname; \
	typedef EzSmartPointer<classname> classname##Ptr


#define SmartPointerCast(type, smartptr) ((type*) (void*) (smartptr))


#endif 
