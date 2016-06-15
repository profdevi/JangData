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


//v0.2 copyright Comine.com 20160614T1327
#ifndef MJangData_h
#define MJangData_h

/////////////////////////////////////////////////
#include "MStdLib.h"
#include "MSQLite.h"
#include "MIntList.h"

/*
	// Config File should be MJangData.ini located in /etc or current directory
	// The Format should be
	##################################
	# Pound marks comments
	MJangDataStore=/JangData				# Directory where data is stored
	MJangDataDB=/JangData/JangData.dat		# location of sqlite database


	Database Design:

		Module(Id,Info) ----< Version(ID,ModuleId)

	Note:	
		Id<=0 are used for testing purposes.


	Business Services:
	[Module]*		Search(SearchString)						: Find all modules that match version
	[Module]		ModuleAdd(ModuleInfo)						: Add a new module
					ModuleDel([Module])							: Remove a module if no versions
	[Version]*		ModuleGetVersions([Module])					: Find Versions for given modules
					VersionAdd([ModuleId],file_or_dir)			: Add a new version
					VersionDel([Version])						: Remove Version for some module
*/

//******************************************************
//**  MJangData Create/Destroy for infrstructure
//******************************************************
bool MJangDataCreate(const char *inifilename,const char *jangdatastore,const char *jangdatadb);
bool MJangDataDestroy(const char *inifilename,const char *jangdatastore,const char *datadatadb);

//******************************************************
//**  MJangData class
//******************************************************
class MJangData
	{
	////////////////////////////////////////////////
	MSQLite mJangDB;											// Backend Database
	
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MJangData(void);
	~MJangData(void);
	bool Create(void);
	bool Destroy(void);
	bool Search(const char *search,MIntList &retkeys);			//  Return the set of keys of Modules
	};

#endif // MJangData_h

