// EzCoreTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "EzCore"


void test2()
{

	char *p = (char*) EzMalloc(10);

	p = (char*) EzRealloc(p, 5);

	//EzFree(p);



}

void test_EzString()
{
	EzStringc temp;

	temp.format("%f", 0.55);

}

void test()
{
	char path[1024];

	EzPath::getExecutableDirectory(path, sizeof(path));

	EzPath::getCurrentWorkingDirectory(path, sizeof(path));

	EzMap<EzStringc, int> test;

	test.insert("1111", 1);

	test.find("1111");

	test.clear();


	//EZLOG(1, "test");

	EzPoint2* pPt = EzNew EzPoint2;

	pPt->set(100, 100);

	EzStringc  temp;


	printf("1111");


	EzDelete pPt;

}
int _tmain(int argc, _TCHAR* argv[])
{

	EzCoreInit();

	test_EzString();
	test();


	EzCoreShutdown();
	return 0;
}




