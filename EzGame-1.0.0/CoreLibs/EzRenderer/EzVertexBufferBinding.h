#ifndef __EZVERTEXBUFFERBINDING_H_INCLUDE__
#define __EZVERTEXBUFFERBINDING_H_INCLUDE__


#include "../EzCore/EzArray.h"

class EzVertexBuffer;


class EzVertexBufferBinding
{
public:
	typedef EzArray<EzVertexBuffer*>		EzVertexBufferBindingArray;
	
	
	EzVertexBufferBinding(void);
	~EzVertexBufferBinding(void);

	void setBinding(u16 uiIndex, EzVertexBuffer* pVertexBuffer);
	void unsetBinding(u16 uiIndex);
	void unsetAllBindings();
	const EzVertexBufferBindingArray& getBindings(void);
	EzVertexBuffer* getBuffer(u16 uiIndex);


protected:
	EzVertexBufferBindingArray		m_aVertexBinding;
};


#endif

