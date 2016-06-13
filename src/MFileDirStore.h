//v1.1 copyright Comine.com 20160421R1356
#ifndef MFileDirStore_h
#define MFileDirStore_h

/////////////////////////////////////////////////
#include "MStdLib.h"
#include "MString.h"

//******************************************************
//**  MFileDirStore class
//******************************************************
class MFileDirStore
	{
	////////////////////////////////////////////////
	MString mDirTarget;
	
	////////////////////////////////////////////////
	void ClearObject(void);
	
	////////////////////////////////////////////////
	public:
	MFileDirStore(void);
	MFileDirStore(const char *dir);
	~MFileDirStore(void);
	bool Create(const char *dir);
	bool Destroy(void);
	bool Exists(const char *storename);
	bool Get(const char *storename,MString &fullpathdir);
	bool Remove(const char *storename);
	bool Save(const char *storename);
	bool Save(const char *storename,const char *srcfile);
	bool Retrieve(const char *storename);
	bool Retrieve(const char *storename,const char *localname);
	};

#endif // MFileDirStore_h

