#pragma once


#include "EzProperty.h"


EzSmartPointer(EzMaterialProperty);

class EzMaterialProperty : public EzProperty 
{
	EzDeclareStream;
	EzDeclareRTTI;

public:
	EzMaterialProperty(void);
	~EzMaterialProperty(void);



	unsigned int getRevisonID() { return m_uiRevisionID; }
	void updateRevisonID() { m_uiRevisionID = m_uiGlobalRevisionID++; }

	inline ETYPE type() { return MATERIAL; }

	unsigned short getFlags() const;
	bool isLight() const;
	void setLight(bool bflag);
	void setFlags( unsigned short n );
	float getTransparency() const;
	void setTransparency( float n );
	EzColor getAmbientColor() const;
	void setAmbientColor( const EzColor & color );
	EzColor getDiffuseColor() const;
	void setDiffuseColor( const EzColor & color );
	EzColor getEmissiveColor() const;
	void setEmissiveColor( const EzColor & color );
	EzColor getSpecularColor() const;
	void setSpecularColor( const EzColor & color );
	float getGlossiness() const;
	void setGlossiness( float n );

protected:
	/*! Property flags. */
	unsigned short m_uiFlags;
	EzColor m_AmbientColor;
	EzColor m_DiffuseColor;
	EzColor m_SpecularColor;
	EzColor m_EmissiveColor;
	float m_fGlossiness;
	///*!
	// * The material transparency (1=non-transparant). Refer to a NiAlphaProperty object
	// * in this material's parent NiTriShape object, when alpha is not 1.
	// */
	float m_fAlpha;
	bool m_bLight;

	unsigned int m_uiRevisionID;
	static unsigned int m_uiGlobalRevisionID;
};
