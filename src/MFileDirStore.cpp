//v1.1 copyright Comine.com 20160421R1356
#include "MStdLib.h"
#include "MString.h"
#include "MFileOps.h"
#include "MFileDirStore.h"


//******************************************************
//**  Module Elements
//******************************************************
static const int GMaxPathSize=1000;

//////////////////////////////////////////////////////////
static bool GIsCleanFileName(const char *filename)
	{	
	const char *p;
	for(p=filename;*p!=0;++p)
		{
		if(*p=='/' || *p=='\\') { return false; }
		}

	return true;
	}


//******************************************************
//**  MFileDirStore class
//******************************************************
void MFileDirStore::ClearObject(void)
	{
	}


////////////////////////////////////////////////
MFileDirStore::MFileDirStore(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MFileDirStore::MFileDirStore(const char *targetdir)
	{
	ClearObject();
	if(Create(targetdir)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MFileDirStore::~MFileDirStore(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MFileDirStore::Create(const char *dir)
	{
	Destroy();

	// Check if dir exists
	MFileOps fileops(true);
	if(fileops.IsDirectory(dir)==false)
		{
		Destroy();
		return false;
		}

	if(fileops.GetAbsolutePath(dir,mDirTarget)==false)
		{
		return false;
		}
	
	return true;
	}


////////////////////////////////////////////////
bool MFileDirStore::Destroy(void)
	{
	mDirTarget.Destroy();
	ClearObject();
	return true;
	}


bool MFileDirStore::Exists(const char *storename)
	{
	MString targetfilepath;
	if(Get(storename,targetfilepath)==false)
		{
		return false;
		}

	MFileOps fileops(true);
	if(fileops.Exists(targetfilepath.Get())==false)
		{
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MFileDirStore::Get(const char *storename,MString &fullpathdir)
	{
	if(GIsCleanFileName(storename)==false)
		{
		return false;
		}

	MBuffer targetname(GMaxPathSize);
	MStdSPrintf(targetname.GetBuffer(),targetname.GetSize(),"%s/%s",mDirTarget.Get(),storename);

	if(fullpathdir.Create(targetname.GetBuffer())==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MFileDirStore::Remove(const char *storename)
	{
	MString targetfilepath;
	if(Get(storename,targetfilepath)==false)
		{
		return false;
		}

	MStdFileRemove(targetfilepath.Get() );

	return true;
	}


////////////////////////////////////////////////
bool MFileDirStore::Save(const char *storename)
	{
	MString targetname;
	if(Get(storename,targetname)==false)
		{
		return false;
		}

	MFileOps fileops(true);
	if(fileops.Copy(storename,targetname.Get(),false,true)==false)
		{
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MFileDirStore::Save(const char *filename,const char *storefile)
	{
	MString targetname;
	if(Get(storefile,targetname)==false)
		{
		return false;
		}

	if(GIsCleanFileName(filename)==false)
		{
		return false;
		}

	MFileOps fileops(true);
	if(fileops.Copy(filename,targetname.Get(),false,true)==false)
		{
		return false;
		}
	
	return true;
	}


///////////////////////////////////////////////////////
bool MFileDirStore::Retrieve(const char *storename)
	{
	MString targetname;
	if(Get(storename,targetname)==false)
		{
		return false;
		}

	MFileOps fileops(true);
	if(fileops.Copy(targetname.Get(),storename,false,true)==false)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////////////
bool MFileDirStore::Retrieve(const char *storename,const char *localname)
	{
	MString targetname;
	if(Get(storename,targetname)==false)
		{
		return false;
		}

	if(GIsCleanFileName(localname)==false)
		{
		return false;
		}

	MFileOps fileops(true);
	if(fileops.Copy(targetname.Get(),localname,false,true)==false)
		{
		return false;
		}
	
	return true;
	}



