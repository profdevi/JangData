//v3.15 copyright Comine.com 20150818T0507
#ifndef MConfigFile_h
#define MConfigFile_h

/*
	Sample Config File

	#
	#	Pound Markers are comments
	#

	Name=Value
	Hello=World	

*/

//////////////////////////////////////////////////////
#include "MStdLib.h"

//*******************************************************
//*  Simple File Configuration Reader
//*******************************************************
class MConfigFile
	{
	struct GStringData *mList;		// List of Name/Value Pairs

	////////////////////////////////////////////////////////////
	void ClearObject(void);
	struct GStringData *Find(const char *name);
	
	////////////////////////////////////////////////////////////
	public:
	MConfigFile(void);
	~MConfigFile(void);
	bool Create(void);
	bool Destroy(void);
	bool Clear(void);									// Clear Name Value pairs
	bool ReadFile(const char *filename);				// Read from a config file
	bool ReadFileExeHome(const char *filename);			// Reads Config file from exe home
	bool WriteFile(const char *filename);				// Write Values to a file
	bool WriteFileExeHome(const char *filename);		// Write Config File to exe home
	bool Set(const char *name,const char *value=NULL);	// Set Name/Value Pair
	const char *Get(const char *name);					//="", if name is empty
	bool Sort(void);									// Sort All the items
	
	// Reader Code 
	bool Read(void *&handle);
	const char *ReadName(void *handle);
	const char *ReadValue(void *handle);

	// Reads/Writes/Deletes INI Files
	bool ReadIniFile(void);					// Reads ExeFileName.ini from exe home or in c:/etc
	bool WriteIniFile(void);				// Writes ExeFileName.ini File
	bool DeleteIniFile(void);				// Deletes INI File
	};

#endif // MConfigFile_h
