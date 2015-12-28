#pragma once

#include "../EzCore/EzCore.h"
#include "../EzCore/EzRefObject.h"

#define  Ez3DVisibleArray     EzArray<EzAVObject*>



class EzCamera;
class EzAVObject;


EzSmartPointer(EzCullingProcess);


class EzCullingProcess : public EzRefObject
{
	EzDeclareRootRTTI(EzCullingProcess);

public:
	EzCullingProcess(void);
	~EzCullingProcess(void);

	virtual void process(const EzCamera* pkCamera, EzAVObject* pScene, Ez3DVisibleArray* pVisibleSet);

	inline void append(EzAVObject* pVisible)
	{
		m_pVisibleSet->push_back(pVisible);
	}
	
protected:
	void process(EzAVObject* pAVObject);

	Ez3DVisibleArray*			m_pVisibleSet;
	const EzCamera* m_pCamera;

	friend class EzAVObject;



};
