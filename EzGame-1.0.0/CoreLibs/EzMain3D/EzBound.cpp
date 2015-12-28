#include "EzBound.h"


EzImplementRootRTTI(EzBound);


EzBound::EzBound(void)
:m_fRadius(0.0f)
{
}

EzBound::~EzBound(void)
{
}

void EzBound::update(const EzBound& bound, const EzMatrix4& mat)
{
	m_BBox = bound.m_BBox;
	m_fRadius = bound.m_fRadius;
	m_vCenter = bound.m_vCenter;

	mat.transformBoxEx(m_BBox);
	mat.transformVect(m_vCenter);
}

void EzBound::merg(const EzBound& bound)
{
	m_BBox.addInternalBox(bound.m_BBox);
	
	EzPoint3 vExtent = m_BBox.getExtent();

	if(vExtent.X >= vExtent.Y)
	{
		m_fRadius = vExtent.X;
	}

	if(vExtent.Z >= m_fRadius)
	{
		m_fRadius = vExtent.Z;
	}

	m_fRadius = m_fRadius * 0.5f;
	m_vCenter = m_BBox.getCenter();


}

void EzBound::computeFromData(unsigned int uiQuantity, const EzPoint3* pkData)
{

	if (uiQuantity == 0)
	{
		m_vCenter = EzPoint3(0.0f, 0.0f, 0.0f);
		m_fRadius = 0.0f;
		return;
	}

	m_BBox.MinEdge = pkData[0];
	m_BBox.MaxEdge = pkData[0];

	for(unsigned int n = 1; n < uiQuantity; ++n)
	{
		m_BBox.addInternalPoint(pkData[n]);
	}

	EzPoint3 vExtent = m_BBox.getExtent();

	if(vExtent.X >= vExtent.Y)
	{
		m_fRadius = vExtent.X;
	}

	if(vExtent.Z >= m_fRadius)
	{
		m_fRadius = vExtent.Z;
	}

	m_fRadius = m_fRadius * 0.5f;
	m_vCenter = m_BBox.getCenter();
}
