#pragma once

#include "../EzCore/EzCore.h"
#include "../EzCore/EzSingleton.h"
#include "EzVertexBuffer.h"
#include "EzIndexBuffer.h"
#include "EzVertexDeclaration.h"

#include "../EzMain3D/EzHardwareSkinPartition.h"


class EzGeometryData;	
class EzSkinInstance;
class EzMeshBufferData;
class EzVertexBufferBinding;

class Ez3DBufferManager : public EzSingleton<Ez3DBufferManager>
{
public:
	virtual EzVertexBuffer* createVertexBuffer(u32 uiStride, u32 uiNumVerts, Ez3DBuffer::EUsage eUsage,  bool bUseSystemMemory) = 0;
	virtual EzIndexBuffer* createIndexBuffer(EzIndexBuffer::EIndexType eType, u32 uiNumIndexes, Ez3DBuffer::EUsage eUsage,  bool bUseSystemMemory)  = 0;
	virtual EzVertexDeclaration* createVertexDeclaration()  = 0;

	EzVertexBufferBinding* createVertexBufferBinding();
	EzVertexDeclaration* generateVertexStream(EzGeometryData* pGemData, EzHardwareSkinPartition::Partition* pPartition, 
		u32& uiPosOffset, u32& uiWeightOffset, u32& uiBoneIndexOffset, u32& uiNormOffset,
		u32& uiColorOffset, u32& uiTexOffset);


protected:
	Ez3DBufferManager(void);
	~Ez3DBufferManager(void);
};



inline EzVertexBuffer* createVertexBuffer(u32 uiStride, u32 uiNumVerts, Ez3DBuffer::EUsage eUsage,  bool bUseSystemMemory = false)
{
	return Ez3DBufferManager::get().createVertexBuffer(uiStride, uiNumVerts, eUsage, bUseSystemMemory);

}

inline EzIndexBuffer* createIndexBuffer(EzIndexBuffer::EIndexType eType, u32 uiNumIndexes, Ez3DBuffer::EUsage eUsage,  bool bUseSystemMemory = false)
{
	return Ez3DBufferManager::get().createIndexBuffer(eType, uiNumIndexes, eUsage, bUseSystemMemory);

}

inline EzVertexDeclaration* createVertexDeclaration()
{
	return Ez3DBufferManager::get().createVertexDeclaration();
}

inline 	EzVertexBufferBinding* createVertexBufferBinding()
{
	return Ez3DBufferManager::get().createVertexBufferBinding();

}

inline EzVertexDeclaration* generateVertexStream(EzGeometryData* pGemData, EzHardwareSkinPartition::Partition* pPartition, 
	u32& uiPosOffset, u32& uiWeightOffset, u32& uiBoneIndexOffset, u32& uiNormOffset,
	u32& uiColorOffset, u32& uiTexOffset)
{
	return Ez3DBufferManager::get().generateVertexStream(pGemData, pPartition, 
		uiPosOffset, uiWeightOffset, uiBoneIndexOffset,uiNormOffset,
		uiColorOffset, uiTexOffset);
}

