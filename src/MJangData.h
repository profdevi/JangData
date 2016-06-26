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


//v1.1 copyright Comine.com 20160626U0906
#ifndef MJangData_h
#define MJangData_h

/////////////////////////////////////////////////
#include "MStdLib.h"
#include "MSQLite.h"
#include "MIntList.h"
#include "MStringList.h"
#include "MString.h"

/*
	// Config File should be MJangData.ini located in /etc or current directory
	// The Format should be
	##################################
	# Pound marks comments
	MJangDataStore=/JangData				# Directory where data is stored
	MJangDataDB=/JangData/JangData.dat		# location of sqlite database


	Database Design:
		Module(Id,User,Info,DirName)

	Note:	
		Id<=0 are used for testing purposes.


	Services
	-----------------

	[Module]*				ModuleSearch(Word*)				: Find all modules that match version
	[Module]				ModuleAdd(Info,SrcDir)			: Add a new module
							ModuleDel([Module])				: Remove a module if no versions
	(Info,User,DirName)		ModuleGetInfo([Module])			: Get Information about module				
							ModuleDump([Module])			: Dump module into current directory

*/

//******************************************************
//**  MJangData Create/Destroy for infrstructure
//******************************************************
bool MJangDataCreate(const char *inifilename,const char *jangdatastore,const char *jangdatadb);
bool MJangDataDestroy(const char *inifilename,const char *jangdatastore,const char *datadatadb);
bool MJangDataExists(const char *inifilename);				// Check if ini is fine
bool MJangDataExists(const char *jangdatastore,const char *jangdatadb);	// Check if data store and db file are fine
bool MJangDataVerify(const char *inifilename);				// Verify the ini file
bool MJangDataVerify(const char *jangdatastore,const char *jangdatadb);	// Verify the files


//******************************************************
//**  MJangData class
//******************************************************
class MJangData
	{
	////////////////////////////////////////////////
	MSQLite mJangDB;													// Backend Database
	MString mStorageDir;												// Directory where data is stored
	MString mUserName;													// User Name of the current user
	
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MJangData(void);
	~MJangData(void);
	bool Create(const char *storagedir,const char *dbfile);				// Init using storage folder and db file location
	bool Create(const char *configfilelocation);						// Read the configuration file
	bool Destroy(void);
	bool ModuleSearch(MStringList &searchwords,MIntList &retkeys);		//  Return the set of keys of Modules
	int ModuleAdd(const char *directory,const char *info);				//  Add a new Module Information
	bool ModuleDel(int modulekey);										//  Delete Module
	bool ModuleGetInfo(int modulekey,MString &info,MString &username,MString &dirinfo);	// Get info about Module	
	bool ModuleDump(int modulekey);										// Save Module to current directory
	};

#endif // MJangData_h

