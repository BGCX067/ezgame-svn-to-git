#include "EzD3DX9VertexDeclaration.h"
#include "EzD3DX9Mappings.h"


EzImplementRTTI(EzD3DX9VertexDeclaration, EzVertexDeclaration);

EzD3DX9VertexDeclaration::EzD3DX9VertexDeclaration(LPDIRECT3DDEVICE9 device)
: m_lpD3DDevice(device), m_lpD3DDecl(NULL), m_bRebuild(true)
{

}
EzD3DX9VertexDeclaration::~EzD3DX9VertexDeclaration()
{

}

const EzVertexElement& EzD3DX9VertexDeclaration::addElement(u16 usSource, u32 uiOffset, EzVertexElement::EVertexElementType eType, 
 EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex)
{
	
	m_bRebuild = true;
	EzVertexSourceMap::Node* pNode;

	pNode = m_mapVertexSource.find(usSource);

	if(!pNode)
	{
		m_mapVertexSource.insert(usSource, 1);
	}
	else
	{
		pNode->setValue(pNode->getValue() + 1);
	}

	return EzVertexDeclaration::addElement(usSource, uiOffset, eType, eSemantic, uIndex);

}

void EzD3DX9VertexDeclaration::removeElement(u32 uiElem_index)
{
	m_bRebuild = true;
	EzVertexDeclaration::removeElement(uiElem_index);
	

}
void EzD3DX9VertexDeclaration::removeElement(EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex)
{
	m_bRebuild = true;
	EzVertexDeclaration::removeElement(eSemantic, uIndex);
	
	
}
void EzD3DX9VertexDeclaration::removeAllElements(void)
{
	m_bRebuild = true;
	EzVertexDeclaration::removeAllElements();
	
}


void EzD3DX9VertexDeclaration::modifyElement(u32 uiElem_index, u16 usSource, u32 uiOffset, EzVertexElement::EVertexElementType eType,
 EzVertexElement::EVertexElementSemantic eSemantic, u8 uIndex)
{
	m_bRebuild = true;
	EzVertexDeclaration::modifyElement(uiElem_index, usSource, uiOffset, eType, eSemantic, uIndex);
	

}

LPDIRECT3DVERTEXDECLARATION9 EzD3DX9VertexDeclaration::getD3DVertexDeclaration(void)
{
	if(m_bRebuild)
	{
		if(m_lpD3DDecl)
		{
			m_lpD3DDecl->Release();
			m_lpD3DDecl = NULL;
		}
		
		D3DVERTEXELEMENT9* d3delems = EzAlloc(D3DVERTEXELEMENT9, m_aVertexElement.size() + 1);

		u32 uiIdx = 0;

		for (; uiIdx < m_aVertexElement.size(); ++uiIdx)
		{
			d3delems[uiIdx].Method = D3DDECLMETHOD_DEFAULT;
			d3delems[uiIdx].Offset = static_cast<WORD>(m_aVertexElement[uiIdx].getOffset());
			d3delems[uiIdx].Stream = m_aVertexElement[uiIdx].getSource();
			d3delems[uiIdx].Type = EzD3DX9Mappings::get(m_aVertexElement[uiIdx].getType());
			d3delems[uiIdx].Usage = EzD3DX9Mappings::get(m_aVertexElement[uiIdx].getSemantic());
			// NB force index if colours since D3D uses the same usage for diffuse & specular
			if (m_aVertexElement[uiIdx].getSemantic() == EzVertexElement::EVES_SPECULAR)
			{
				d3delems[uiIdx].UsageIndex = 1;
			}
			else if (m_aVertexElement[uiIdx].getSemantic() == EzVertexElement::EVES_DIFFUSE)
			{
				d3delems[uiIdx].UsageIndex = 0;
			}
			else
			{
				d3delems[uiIdx].UsageIndex = m_aVertexElement[uiIdx].getIndex();
			}
		}
		// Add terminator
		d3delems[uiIdx].Stream = 0xFF;
		d3delems[uiIdx].Offset = 0;
		d3delems[uiIdx].Type = D3DDECLTYPE_UNUSED;
		d3delems[uiIdx].Method = 0;
		d3delems[uiIdx].Usage = 0;
		d3delems[uiIdx].UsageIndex = 0;

		HRESULT hr = m_lpD3DDevice->CreateVertexDeclaration(d3delems, &m_lpD3DDecl);

		if (FAILED(hr))
		{
			printf("111");

		}

		EzFree(d3delems);

		m_bRebuild = false;


	}

	return m_lpD3DDecl;

}

