//v0.0 copyright Comine.com 20150220F0758
#include "MStdLib.h"
#include "BaseClass.h"


//******************************************************
//**  BaseClass class
//******************************************************
void BaseClass::ClearObject(void)
	{
	}


////////////////////////////////////////////////
BaseClass::BaseClass(void)
	{  ClearObject();  }


////////////////////////////////////////////////
BaseClass::~BaseClass(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool BaseClass::Create(void)
	{
	Destroy();
	return true;
	}


////////////////////////////////////////////////
bool BaseClass::Destroy(void)
	{
	ClearObject();
	return true;
	}



