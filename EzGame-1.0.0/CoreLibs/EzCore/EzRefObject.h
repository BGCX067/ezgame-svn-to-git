#pragma once

#include "EzRTTI.h"
#include "EzFlags.h"
#include "EzSmartPointer.h"
#include "EzMemObject.h"


#define EzDeclareClon(classname)		\
public: \
	classname* clon(); \
protected:\
	void copyMember(classname* pDest)

#define EzDeclareCopyMember(classname) \
protected: \
	void copyMember(classname* pDest)

#define EzCopyMember(classname)		\
	classname::copyMember(EzStaticCast(classname, pDest))


#define EzImplementClon(classname)  \
	classname* classname::clon()  \
	{ \
	classname* pDest = EzNew classname; \
	copyMember(pDest); \
	return pDest; \
} \

class EZCORE_ENTRY EzRefObject : public EzMemObject
{
public:
	EzRefObject(void) :m_uiRefCount(0)
	{
		ms_uiObjects++;

	}
	~EzRefObject(void) 
	{
		ms_uiObjects--;
	}

	inline void addRef() 
	{ 
		m_uiRefCount++; 
	}
	inline void release() 
	{ 
		m_uiRefCount--;
	
		if(m_uiRefCount == 0)
		{
			deleteThis();
		}
	}

    inline unsigned int getRefCount() const
	{
		return m_uiRefCount;
	}

    static unsigned int getTotalObjectCount()
	{
		return ms_uiObjects;

	}

protected:
    virtual void deleteThis();

private:
    unsigned int m_uiRefCount;
    static unsigned int ms_uiObjects;

};



