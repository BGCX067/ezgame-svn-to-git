#ifndef __EZVERTEXELEMENT_H_INCLUDE__
#define __EZVERTEXELEMENT_H_INCLUDE__


#include "../EzCore/EzTypes.h"



class EzVertexElement
{
public:
	enum EVertexElementSemantic 
	{
		/// Position, 3 reals per vertex
		EVES_POSITION = 1,
		/// Blending weights
		EVES_BLEND_WEIGHTS = 2,
		/// Blending indices
		EVES_BLEND_INDICES = 3,
		/// Normal, 3 reals per vertex
		EVES_NORMAL = 4,
		/// Diffuse colours
		EVES_DIFFUSE = 5,
		/// Specular colours
		EVES_SPECULAR = 6,
		/// Texture coordinates
		EVES_TEXTURE_COORDINATES = 7,
		/// Binormal (Y axis if normal is Z)
		EVES_BINORMAL = 8,
		/// Tangent (X axis if normal is Z)
		EVES_TANGENT = 9

	};

	/// Vertex element type, used to identify the base types of the vertex contents
	enum EVertexElementType
	{
		EVET_FLOAT1 = 0,
		EVET_FLOAT2 = 1,
		EVET_FLOAT3 = 2,
		EVET_FLOAT4 = 3,
		/// alias to more specific colour type - use the current rendersystem's colour packing
		EVET_COLOUR = 4,
		EVET_SHORT1 = 5,
		EVET_SHORT2 = 6,
		EVET_SHORT3 = 7,
		EVET_SHORT4 = 8,
		EVET_UBYTE4 = 9,
		/// D3D style compact colour
		EVET_COLOUR_ARGB = 10,
		/// GL style compact colour
		EVET_COLOUR_ABGR = 11
	};

	EzVertexElement(u16 usSource, u32 uiOffset, EVertexElementType eType, EVertexElementSemantic eSemantic, u8 uIndex = 0);
	~EzVertexElement(void);

	const u16& getSource(void) const;
	const u32& getOffset(void) const;
	const EVertexElementType& getType(void) const;
	const EVertexElementSemantic& getSemantic(void) const;
	const u8& getIndex(void) const;
	u32 getSize(void) const;


	/// Utility method for helping 
	static u32 getTypeSize(EVertexElementType eType);
	static EVertexElementType getBestColourVertexElementType();

	bool operator== (const EzVertexElement& rhs) const;
	//for sort
	bool operator< (const EzVertexElement& other) const;
	

protected:
	EVertexElementType		 m_eType;
	EVertexElementSemantic	 m_eSemantic;
	u32						 m_uiOffset;
	u16						 m_usSource;
	u8						 m_uIndex;

};

inline bool EzVertexElement::operator<(const EzVertexElement& other) const
{
	// Sort by source first
	if (getSource() < other.getSource())
	{
		return true;
	}
	else if (getSource() == other.getSource())
	{
		// Use ordering of semantics to sort
		if (getSemantic() < other.getSemantic())
		{
			return true;
		}
		else if (getSemantic() == other.getSemantic())
		{
			// Use index to sort
			if (getIndex() < other.getIndex())
			{
				return true;
			}
		}
	}
	return false;
}

inline bool EzVertexElement::operator== (const EzVertexElement& rhs) const
{
	if (m_eType != rhs.m_eType ||
		m_uIndex != rhs.m_uIndex ||
		m_uiOffset != rhs.m_uiOffset ||
		m_eSemantic != rhs.m_eSemantic ||
		m_usSource != rhs.m_usSource)
		return false;
	else
		return true;
}


inline const u16& EzVertexElement::getSource(void) const 
{ 
	return m_usSource; 
}

inline const u32& EzVertexElement::getOffset(void) const 
{ 
	return m_uiOffset; 
}

inline const EzVertexElement::EVertexElementType& EzVertexElement::getType(void) const 
{ 
	return m_eType; 
}

inline const EzVertexElement::EVertexElementSemantic& EzVertexElement::getSemantic(void) const 
{ 
	return m_eSemantic; 
}

inline const u8& EzVertexElement::getIndex(void) const 
{ 
	return m_uIndex; 
}

inline u32 EzVertexElement::getSize(void) const
{
	return getTypeSize(m_eType);

}


#endif
