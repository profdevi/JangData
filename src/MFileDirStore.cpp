/*
Copyright (C) 2011-2014, Comine.com ( profdevi@ymail.com )
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
- The the names of the contributors of this project may not be used to 
  endorse or promote products derived from this software without specific 
  prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
`AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


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



