#include "stdafx.h"
#include "SampleAPP3D.h"
#include <Ez3DUtility>

EzPoint3 Bezier3(EzPoint3 p1,EzPoint3 p2,EzPoint3 p3,double mu)
{

	double mum1,mum12,mu2;

	EzPoint3 p;

	mu2 = mu * mu;

	mum1 = 1 - mu;

	mum12 = mum1 * mum1;

	p.X = p1.X * mum12 + 2 * p2.X * mum1 * mu + p3.X * mu2;

	p.Y = p1.Y * mum12 + 2 * p2.Y * mum1 * mu + p3.Y * mu2;

	p.Z = p1.Z * mum12 + 2 * p2.Z * mum1 * mu + p3.Z * mu2;

	return(p);

}


EzApplication* EzApplication::create()
{
	return EzNew CSampleAPP3D;
}
bool CSampleAPP3D::preinitialize()
{
	SCreationParameters3D param;

	EzExcelCsv* pCsv = CreateCSVReader("test.csv");

	char Buff[1024];

	sprintf(Buff, "%s   %s", GAMEEASY_EZF_VERSION_STRING, GAMEEASY_SDK_VERSION_STRING);



	EzConfigFile* pConfig = createConfigFile("BridgeServer.INI");

	if(!pConfig)
	{
		log(0x1111, "11111");
		return false;
	}

	f32 n;
	
	pConfig->getProfileFloat("STORE", "STORESERVER_WAIT_PORTNUM", n);

	param.eMultiSampleType = EMULTISAMPLE_2_SAMPLES;
	param.WindowSize.set(800, 600);

	setCreateOption(param);




	

	return true;
	
}


void CSampleAPP3D::update(float time)
{
//	m_pCostum->getGeometryNode()->update(time);

}

void CSampleAPP3D::createScene()
{
	setTextureFolder("D:\\test\\");

	EzStream steram;

	//steram.load("D:\\test\\DarkMap.ezf");

	steram.load("D:\\666.ezf");


	EzPerformanceCounterTimer  timer;
	EzPerformanceCounterTimer  timer2;


	EzMemPool<EzColor>  t1;

	EzColor* test1, test2;

	timer.start();

	for(int i = 0; i < 5100; i++)
	{
		test1 = new EzColor;

	//	delete test1;
	}

	timer.stop();

	CSP("time new %f\n", timer.getElapsed());

	timer.start();

	for(int i = 0; i < 5100; i++)
	{
		test1 = t1.alloc();

		test1->r = 20;

		//t1.release(test1);
	}

	timer.stop();

	CSP("time %f\n", timer.getElapsed());

	t1.clear();






	







	

	


//	EzStream steram;

	//steram.load("E:\\rr\\meshes\\marker_arrow.nif");

	//EzFile *pFile = EzFile::open("charecter.cst", EzFile::READ_ONLY);

	//EzLoaderCostum* pHero = new EzLoaderCostum(pFile);

	//m_pCostum = pHero->getCostumObject();

	//m_pCostum->makeShape(false);

	EzDirectionalLight* pDirLight = EzNew EzDirectionalLight;

	pDirLight->setPosition(EzPoint3(0, 0, 0));
	pDirLight->setRotation(EzPoint3(0, 60, 0));
	pDirLight->update(0.0f);

	pDirLight->setAmbientColor(EzColor(0, 0, 0, 0));
	pDirLight->setDiffuseColor(EzColor(0.7f, 0.7f, 0.85f));

	pDirLight->attachAffectedNode(getScene());
	getScene()->attachEffect(pDirLight);
	getScene()->addChild(pDirLight);

	//EzPoint3 LightDir = pDirLight->getDirection();
	//EzPoint3 vRot = pDirLight->getRotation();
	//LightDir.invert();
	//vRot.Y += 90.0f;

	//EzPoint3 pos = m_pCostum->getGeometryNode()->getPosition() +  LightDir * 70;


	//steram.getAt(0)->setScale(0.1f);
	//steram.getAt(0)->setPosition(pos);
	//steram.getAt(0)->setRotation(vRot);

	getScene()->addChild(steram.getAt(0));


	//getScene()->addChild(m_pCostum->getGeometryNode());

	//m_act.setActorNode(m_pCostum->getGeometryNode());

	////m_act.loadSequence(1, "D:\\Resource\\meshes\\characters\\_male\\blockattack.kf" );
	//m_act.loadSequence(1, "e:\\rr\\meshes\\characters\\_male\\idle.kf" );
	//m_act.activate(1);

	////
	getScene()->updateEffects();
	getScene()->update(0.0f);

	//steram.load("D:\\ppp\\LM.nif");

	//EzNode* pNode = EzDynamicCast(EzNode, steram.getAt(0));

	//EzTriBasedGeom *pTri = EzDynamicCast(EzTriBasedGeom, pNode->getObjectByName("Editable Mesh"));
	//EzTriBasedGeomData *pTriData = EzDynamicCast(EzTriBasedGeomData, pTri->getData());

	//

	//

	//makePartitions(pTriData, pTri->getSkinInstance()->getSkinData(), 4, 4, false);


}


void CSampleAPP3D::OnKeyUp(EKEY_CODE eKey, wchar_t Char, bool bShift, bool bControl)
{

	EzCamera*  pActiveCamera = (EzCamera*) getRender()->getActiveCamera();

	switch(eKey)
	{
		case KEY_F1:
			m_fRotY = 90;
			m_fRotX = 90; 
		
			break;

	}
	

}