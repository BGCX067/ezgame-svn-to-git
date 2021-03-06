#pragma once

#include "../EzCore/EzCore.h"
#include "EzTextureStage.h"
#include "EzGpuProgramUsage.h"
#include "EzGpuProgramManager.h"


enum ESHADE_MODE
{
	ESHADE_FLAT = 0,        
	ESHADE_GOURAUD,      
	ESHADE_PHONG       
};

class EzTextureStage;
class EzGeometry;
class EzVideoDriver;
class EzRendererData;


EzSmartPointer(EzRenderPass);

class EzRenderPass : public EzRefObject
{
public:


	EzRenderPass(void);
	~EzRenderPass(void);


	static void initializePools();
	static void shutdownPools();
	static void setMaxTextureBlendStages(unsigned int uiMaxTextureBlendStages);
	static EzRenderPassPtr createNewPass();
	void returnPassToPool();

	void setupRenderingPass(EzGeometry* pGeometry, EzRendererData* pRendererData);
	void setVertexProgram(const char* Name, bool resetParams = true);
	void setVertexProgramParameters(EzGpuProgramParameters* pParams);
	const char* getVertexProgramName(void) const;
	EzGpuProgramParameters* getVertexProgramParameters(void);
	EzGPUProgram* getVertexProgram(void);
	
	const EzRenderPass* getNext() const;
	EzRendererData*	 getRenderData() { return m_pRenderData; }

	bool setStage(unsigned int uiStageNum, EzTextureStage* pStage);
	EzTextureStage* getStage(unsigned int uiStageNum);
	inline bool appendStage(EzTextureStage* pStage);
	void updateStage(EzVideoDriver* pDriver);


	static unsigned int ms_uiMaxTextureBlendStages;


protected:

	unsigned int							m_uiStageCount;
	unsigned int							m_uiCurrentStage;

	EzTransform					m_World;

	//material 

	EzColor						m_AmbientColor; 
	EzColor						m_DiffuseColor;
	EzColor						m_SpecularColor;
	EzColor						m_EmissiveColor;

	float							m_fSininess;

	// shademode
	ESHADE_MODE					m_eShading;

	// lighting
	bool						m_bLighting;

	EzGpuProgramUsage*			m_pVertexprogramUsage;
	EzGpuProgramUsage*			m_pPixelprogramUsage;

	EzRenderPass*				m_pNextPass;
	EzRendererData*				m_pRenderData;

	EzSet<EzRendererData*>		m_SetRenderData;
	EzArray<EzTextureStagePtr>	m_aStages;
	bool						m_bSetupRenderPass;

	static EzCriticalSection ms_PassPoolCriticalSection;
	static EzMemPool<EzRenderPass>* ms_pRenderPassPool;
};



inline EzTextureStage* EzRenderPass::getStage(unsigned int uiStageNum)
{
	EZASSERT(uiStageNum >= ms_uiMaxTextureBlendStages);

	return m_aStages[uiStageNum];
}

inline bool EzRenderPass::appendStage(EzTextureStage* pStage)
{
    return setStage(m_uiCurrentStage, pStage);
}


inline void EzRenderPass::setMaxTextureBlendStages(unsigned int uiMaxTextureBlendStages)
{
	ms_uiMaxTextureBlendStages = uiMaxTextureBlendStages;
}

inline const EzRenderPass* EzRenderPass::getNext() const 
{
	return m_pNextPass;
}


inline void EzRenderPass::setVertexProgram(const char* Name, bool resetParams)
{

	EzGPUProgram* pGpuProgram;
	pGpuProgram = findGPUProgram(Name);

	if(pGpuProgram)
	{

		if(!m_pVertexprogramUsage)
		{
			m_pVertexprogramUsage = EzNew EzGpuProgramUsage();
		}

		m_pVertexprogramUsage->setProgram(pGpuProgram);
		m_pVertexprogramUsage->setParameters(pGpuProgram->createParameters());
	}
	
}


inline EzGpuProgramParameters* EzRenderPass::getVertexProgramParameters(void)
{
	EZASSERT(m_pVertexprogramUsage);
	return m_pVertexprogramUsage->getParameters();

}
inline EzGPUProgram* EzRenderPass::getVertexProgram(void)
{
	if(m_pVertexprogramUsage)
		return m_pVertexprogramUsage->getProgram();

	return NULL;
}

inline void EzRenderPass::setVertexProgramParameters(EzGpuProgramParameters* pParams)
{

	EZASSERT(m_pVertexprogramUsage);

	return m_pVertexprogramUsage->setParameters(pParams);

}

