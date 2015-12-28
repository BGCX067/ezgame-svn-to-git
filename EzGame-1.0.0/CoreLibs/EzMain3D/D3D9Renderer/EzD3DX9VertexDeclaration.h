#ifndef __EZD3DXVERTEXDECLARATION_H_INCLUDE__
#define __EZD3DXVERTEXDECLARATION_H_INCLUDE__

#include "../EzVertexDeclaration.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9shader.h>


class EzD3DX9VertexDeclaration : public EzVertexDeclaration
{

	EzDeclareRTTI;

public:
	EzD3DX9VertexDeclaration(LPDIRECT3DDEVICE9 device);
	~EzD3DX9VertexDeclaration();


	virtual const EzVertexElement& addElement(unsigned short usSource, unsigned int uiOffset, EzVertexElement::EVertexElementType eType,
		EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex = 0);

	virtual void removeElement(unsigned int uiElem_index);
	virtual void removeElement(EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex = 0);
	virtual void removeAllElements(void);


	virtual void modifyElement(unsigned int uiElem_index, unsigned short usSource, unsigned int uiOffset, EzVertexElement::EVertexElementType eType,
		EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex = 0);

	LPDIRECT3DVERTEXDECLARATION9 getD3DVertexDeclaration(void);

protected:
	LPDIRECT3DDEVICE9				m_lpD3DDevice;
	LPDIRECT3DVERTEXDECLARATION9	m_lpD3DDecl;
	bool							m_bRebuild;


};


#endif
