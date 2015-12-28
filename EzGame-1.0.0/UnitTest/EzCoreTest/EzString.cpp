#include "stdafx.h"
#include "EzCore"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;

namespace EzCoreTest
{
	[TestClass]
	public ref class EzString_test
	{
	private:
		TestContext^ testContextInstance;

	public: 
		/// <summary>
		///Gets or sets the test context which provides
		///information about and functionality for the current test run.
		///</summary>
		property Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ TestContext
		{
			Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ get()
			{
				return testContextInstance;
			}
			System::Void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ value)
			{
				testContextInstance = value;
			}
		};

		#pragma region Additional test attributes
		//
		//You can use the following additional attributes as you write your tests:
		//
		//Use ClassInitialize to run code before running the first test in the class
		//[ClassInitialize()]
		//static void MyClassInitialize(TestContext^ testContext) {};
		//
		//Use ClassCleanup to run code after all tests in a class have run
		//[ClassCleanup()]
		//static void MyClassCleanup() {};
		//
		//Use TestInitialize to run code before running each test
		[TestInitialize()]
		void MyTestInitialize() 
		{
			EzCoreInit();
		};
		
		//Use TestCleanup to run code after each test has run
		[TestCleanup()]
		void MyTestCleanup() 
		{
			EzCoreShutdown();
		};
		
		#pragma endregion 

		[TestMethod]
		void EzStringTest()
		{
			EzStringc target;

			target = "1234567890";

			Assert::AreNotEqual(target.size(), 10);

			target = "";

			Assert::AreNotEqual(target.size(), 0);
		};

		[TestMethod]
		void EzPoint2Test()
		{
			EzPoint2 point1, point2;

			Assert::AreEqual<float>(point2.X, 0);
			Assert::AreEqual<float>(point2.Y, 0);


			point2.set(100, 100);

			Assert::AreEqual<float>(point2.X, 100);
			Assert::AreEqual<float>(point2.Y, 100);

			float fDistance;
			EzPoint2 point3;

			fDistance = point1.getDistanceFromSQ(point2);

			Assert::AreEqual<float>(fDistance, 100 * 100 + 100 * 100);

			point3 = point1.getInterpolated(point2, 0.5);

			Assert::AreEqual<float>(point3.X, 50, "Point X Value");
			Assert::AreEqual<float>(point3.Y, 50, "Point Y Value");

			float fAngle = point2.getAngle();

			Assert::AreEqual<float>(fAngle, 315);



		};
	};
}
