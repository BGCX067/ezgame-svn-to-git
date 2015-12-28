#ifndef __EZVERTEXBUFFERBINDING_H_INCLUDE__
#define __EZVERTEXBUFFERBINDING_H_INCLUDE__


#include "../EzCore/EzArray.h"
#include "../EzCore/EzRefObject.h"

class EzVertexBuffer;


class EzVertexBufferBinding : public EzRefObject
{
public:
	typedef EzArray<EzVertexBuffer*>		EzVertexBufferBindingArray;
	
	
	EzVertexBufferBinding(void);
	~EzVertexBufferBinding(void);

	void setBinding(unsigned short uiIndex, EzVertexBuffer* pVertexBuffer);
	void unsetBinding(unsigned short uiIndex);
	void unsetAllBindings();
	const EzVertexBufferBindingArray& getBindings(void);
	EzVertexBuffer* getBuffer(unsigned short uiIndex);


protected:
	EzVertexBufferBindingArray		m_aVertexBinding;
};


#endif

