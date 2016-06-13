//v0.1 copyright Comine.com 20160613M1654
#ifndef MJangData_h
#define MJangData_h

/////////////////////////////////////////////////
#include "MStdLib.h"
#include "MSQLite.h"
#include "MFileDirStore.h"
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
	MSQLite mJangDB;
	MFileDirStore mFileStore;

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

