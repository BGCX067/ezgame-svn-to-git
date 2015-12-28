#pragma once

#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzCore.h"
#include "../EzCore/EzRTTI.h"
#include "../EzCore/EzSmartPointer.h"


class EzBound : public EzRefObject
{
	EzDeclareRTTI;

public:
	EzBound(void);
	~EzBound(void);

	inline void setBoundBox(const EzAabbox3d& bbox ) 
	{ 
		m_BBox = bbox; 
		
		EzPoint3 vExtent = m_BBox.getExtent();

		if(vExtent.X >= vExtent.Y)
		{
			m_fRadius = vExtent.X;
		}

		if(vExtent.Z >= m_fRadius)
		{
			m_fRadius = vExtent.Z;
		}
	}

	inline EIntersectionRelation3D classifyPlaneRelation(const EzPlane3d& Plane) const
	{
		float fDistance = Plane.getDistanceTo(m_vCenter);

		if (fDistance < -m_fRadius)
			return ISREL3D_BACK;
		else if (fDistance > m_fRadius)
			return ISREL3D_FRONT;
		
		return ISREL3D_PLANAR;
	}

	inline void setZero() 
	{
		m_fRadius = 0;
		m_BBox.reset(0.0f, 0.0f, 0.0f);
	}

	void computeFromData(unsigned int uiQuantity, const EzPoint3* pkData);

	inline const float& getSphereRadius() const;
	inline const EzPoint3& getCenter() const;
	inline const EzAabbox3d& getBoundBox() const;
	void update(const EzBound& bound, const EzMatrix4& mat);
	void merg(const EzBound& bound);

protected:
	EzAabbox3d			m_BBox;
	float				m_fRadius;
	EzPoint3			m_vCenter;
};

EzSmartPointer(EzBound);

inline const float& EzBound::getSphereRadius() const
{ 
	return m_fRadius; 
}
inline const EzPoint3& EzBound::getCenter() const
{
	return m_vCenter; 
}
inline const EzAabbox3d& EzBound::getBoundBox() const
{ 
	return m_BBox; 
}

