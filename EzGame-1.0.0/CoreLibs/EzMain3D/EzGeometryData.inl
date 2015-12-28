inline EzGeometryData::EzGeometryData(EPRIMITIVE_TYPE ePrimitiveType, EzVector3df* pVertices, EzVector3df* pNormal, EzVector2df* pTCoords, SColor* pColor,
		u16 usVertexSize, u16 usIndexSize)
: m_pVertices(pVertices), m_usVertexSize(usVertexSize), m_pNormal(pNormal), m_pColor(pColor), m_pTCoords(pTCoords),
   m_usDirtyFlags(0), m_pMeshBuffer(NULL), m_ePrimitiveType(ePrimitiveType),
  m_usIndexSize(usIndexSize)
{
	m_pMeshBuffer = EzNew EzMeshBuffer;

	m_Bound.computeFromData(usVertexSize, pVertices);
}

inline EzGeometryData::~EzGeometryData(void)
{
	EzDelete m_pMeshBuffer;
}

inline const EVERTEX_TYPE EzGeometryData::getVertexType()
{
	if(m_pVertices && m_pNormal && m_pTCoords)
	{
		return EVT_STANDARD;
	}
	else if(m_pVertices)
	{
		return EVT_VERTEX;
	}

	return EVT_STANDARD;
}


inline const EzVector3df* EzGeometryData::getVertices()
{ 
	return m_pVertices;
}

inline const EzBound& EzGeometryData::getBound() const
{ 
	return m_Bound;
}














