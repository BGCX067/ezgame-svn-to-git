#pragma once

#include "../EzCore/EzRefObject.h"
#include "../EzCore/EzString.h"
#include "EzShader.h"

class EzShaderLibrary : public EzRefObject
{
public:
	EzShaderLibrary(void);
	~EzShaderLibrary(void);

    EzStringc getName();
    void setName(EzStringc Name);

	EzShader* getShader();
	void setShader(EzShader* pShader);

protected:
	EzStringc		m_Name;
	EzShader*		m_pShader;
};


inline EzShader* EzShaderLibrary::getShader()
{
	return m_pShader;
}

inline void EzShaderLibrary::setShader(EzShader* pShader)
{
	m_pShader = pShader;
}

inline EzStringc EzShaderLibrary::getName()
{
	return m_Name;

}

inline void EzShaderLibrary::setName(EzStringc Name)
{
	m_Name = Name;
}

