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


//v1.2 copyright Comine.com 20160630R1222
#include "MStdLib.h"
#include "MSQLite.h"
#include "MFileOps.h"
#include "MConfigFile.h"
#include "MBuffer.h"
#include "MDirOps.h"
#include "MWUDirReader.h"
#include "MWUProcess.h"
#include "MFile.h"
#include "MIntList.h"
#include "MFilePathBuilder.h"
#include "MZipOps.h"
#include "MStringList.h"
#include "MStringBuffer.h"
#include "MString.h"
#include "MJangData.h"


//******************************************************
//**  Module Elements
//******************************************************
static const char GConfigFileName[]="MJangData.ini";
static const char GJangDataStore[]="MJangDataStore";
static const char GJangDataDB[]="MJangDataDB";
static const char GTmpZipName[]="__MJangData__.zip";

////////////////////////////////////////////////////////////////
// SQL For creation for data store.  Primary Keys>0.  First row is just a dummy rows 
// for testing purposes.
static const char *GSQLBuild[]=
		{
		"create table TModule"
			"("
			"CID integer primary key not null,"
			"CUser varchar(30) not null,"
			"CInfo varchar(250) not null,"
			"CDirName varchar(40) not null"
			")",
		"insert into TModule(CID,CUser,CInfo,CDirName) values(0,'DummyUser','Dummy Record Info','work')",
		0
		};


////////////////////////////////////////////////////////
static bool GCleanSQLInput(char *sql)
	{
	MStdStrRemoveChars(sql,"\\\t\r\n'");
	return true;
	}

//******************************************************
//**  MJangData Create/Destroy for infrstructure
//******************************************************
bool MJangDataCreate(const char *inifilename,const char *jangdatastore,const char *jangdatadb)
	{
	// Destroy if it exists already
	if(MJangDataDestroy(inifilename,jangdatastore,jangdatadb)==false)
		{
		return false;
		}

	// *** Create the ini file output ***
	MFileOutput fileout;
	if(fileout.Create(inifilename)==false)
		{
		return false;
		}

	fileout.WriteChars("#########################################\n");
	fileout.WriteChars("#  Config File Auto Generated\n");
	fileout.WriteChars("#########################################\n");
	fileout.WriteChars("\n");

	MBuffer buffer(2000);

	// Write the location of the data store
	fileout.WriteChars("#  Name of folder where all the files are stored\n");
	MStdSPrintf(buffer.GetBuffer(),buffer.GetSize()-2
			,"%s=%s\n",GJangDataStore,jangdatastore);
	fileout.WriteChars(buffer.GetBuffer() );
	fileout.WriteChars("\n\n");

	// Write the location of sqlite db file
	fileout.WriteChars("#  Name of the SQLite data base file\n");
	MStdSPrintf(buffer.GetBuffer(),buffer.GetSize()-2
			,"%s=%s\n",GJangDataDB,jangdatadb);
	fileout.WriteChars(buffer.GetBuffer() );

	fileout.WriteChars("\n");
	fileout.WriteChars("\n");
	fileout.Destroy();

	if(MJangDataCreate(jangdatastore,jangdatadb)==false)
		{
		MJangDataDestroy(inifilename,jangdatastore,jangdatadb);
		return false;
		}

	return true;
	}


///////////////////////////////////////////////////////////
bool MJangDataDestroy(const char *inifilename,const char *jangdatastore,const char *datadatadb)
	{
	MFileOps fileops(true);
	fileops.Delete(inifilename);

	if(MJangDataDestroy(jangdatastore,datadatadb)==false)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////////
bool MJangDataCreate(const char *jangdatastore,const char *jangdatadb)
	{
	// *** Create Folder for data store ***
	MDirOps dirops(true);
	if(dirops.MakeDir(jangdatastore)==false)
		{
		return false;
		}

	// *** Create the SQLite Data file ***
	MSQLite sqlite;
	if(sqlite.Create(jangdatadb)==false)
		{
		return false;
		}

	// Create the tables
	sqlite.EnableForeignKeys(true);
	
	// Call the SQL statements for build
	for(int i=0;GSQLBuild[i]!=0;++i)
		{
		if(sqlite.Exec(GSQLBuild[i])==false)
			{
			return false;
			}
		}

	sqlite.Destroy();	
	return true;
	}


///////////////////////////////////////////////////
bool MJangDataDestroy(const char *jangdatastore,const char *datadatadb)
	{
	MFileOps fileops(true);
	fileops.Delete(datadatadb);

	// Check if data store folder exists
	MDirOps dirops(true);
	if(dirops.Exists(jangdatastore)==false)
		{
		//=Directoy does not exist.
		return true;
		}

	// Open all the files and remove them from folder
	MWUDirReader dirreader;
	if(dirreader.Create(jangdatastore)==false)
		{
		return false;
		}

	while(dirreader.Read()==true)
		{
		const char *fullpath=dirreader.GetFullFilePath();
		MStdPrintf("Deleting file %s\n",fullpath);
		if(fileops.Delete(fullpath,true)==false)
			{
			return false;
			}
		}

	dirreader.Destroy();

	if(dirops.RemoveDir(jangdatastore)==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////////////
bool MJangDataExists(const char *inifilename)
	{
	// Read the configuration file
	MConfigFile configfile;
	if(configfile.Create()==false)
		{
		return false;
		}

	if(configfile.ReadFile(inifilename)==false)
		{
		return false;
		}
	
	//=We Have loaded the config file into memory
	
	// Check if ini config entriies are fine
	const char *jangdatastore=configfile.Get(GJangDataStore);
	if(jangdatastore==0)
		{
		return false;
		}

	const char *jangdatadb=configfile.Get(GJangDataDB);
	if(jangdatadb==0)
		{
		return false;
		}

	if(MJangDataExists(jangdatastore,jangdatadb)==false)
		{
		return false;
		}
	
	return true;
	}


/////////////////////////////////////////////////////////////////////
bool MJangDataExists(const char *jangdatastore,const char *jangdatadb)
	{
	MStdAssert(jangdatastore!=0 && *jangdatastore!=0);
	MStdAssert(jangdatadb!=0 && *jangdatadb!=0);

	MFileOps fileops(true);
	if(fileops.Exists(jangdatadb)==false)
		{
		return false;
		}

	MDirOps dirops(true);
	if(dirops.Exists(jangdatastore)==false)
		{
		return false;
		}
	
	return true;
	}


/////////////////////////////////////////////////////////////
bool MJangDataVerify(const char *inifilename)
	{
	// Read the configuration file
	MConfigFile configfile;
	if(configfile.Create()==false)
		{
		return false;
		}

	if(configfile.ReadFile(inifilename)==false)
		{
		MStdPrintf("**Unable to read ini file %s\n",inifilename);
		return false;
		}
	
	//=We Have loaded the config file into memory
	
	// Check if ini config entriies are fine
	const char *jangdatastore=configfile.Get(GJangDataStore);
	if(jangdatastore==0)
		{
		MStdPrintf("**Config file %s missing %s=<Data Store Directory>\n",inifilename,GJangDataStore);
		return false;
		}

	const char *jangdatadb=configfile.Get(GJangDataDB);
	if(jangdatadb==0)
		{
		MStdPrintf("**Config file %s missing %s=<DB File Path>\n",inifilename,GJangDataDB);
		return false;
		}

	if(MJangDataVerify(jangdatastore,jangdatadb)==false)
		{
		return false;
		}
	
	return true;	
	}


/////////////////////////////////////////////////////////////
bool MJangDataVerify(const char *jangdatastore,const char *jangdatadb)
	{
	MStdAssert(jangdatastore!=0 && *jangdatastore!=0);
	MStdAssert(jangdatadb!=0 && *jangdatadb!=0);

	MFileOps fileops(true);
	if(fileops.Exists(jangdatadb)==false)
		{
		MStdPrintf("**DB File %s is missing\n",jangdatadb);
		return false;
		}

	MDirOps dirops(true);
	if(dirops.Exists(jangdatastore)==false)
		{
		MStdPrintf("**Data Store directory %s is missing\n",jangdatastore);
		return false;
		}

	// Check if 
	MSQLite sqlite;
	if(sqlite.Create(jangdatadb)==false)
		{
		MStdPrintf("**Unable to open DB file %s\n",jangdatadb);
		return false;
		}

	// Check if tables are ok
	MSQLiteReader reader;
	if(sqlite.Exec(reader,"select CID from TModule where CID>0")==false)
		{
		MStdPrintf("**Unable to get tables in db file %s\n",jangdatadb);
		return false;
		}

	MIntList intlist(true);
	while(reader.Read()==true)
		{
		const char *data=reader.GetColumn(0);
		if(data==0)
			{
			MStdPrintf("**Bad data in DB file %s\n",jangdatadb);
			return false;
			}

		const int modkey=MStdAToI(data);
		if(modkey<=0)
			{
			MStdPrintf("**Bad data in DB file %s\n",jangdatadb);
			return false;
			}

		intlist.Add(modkey);
		}

	//=We now have a list of module keys
	//**Now Verify that the files exist
	MString absstorepath;
	if(fileops.GetAbsolutePath(jangdatastore,absstorepath)==false)
		{
		MStdPrintf("**Unable to get absolute path for %s\n",jangdatastore);
		return false;
		}

	intlist.ReadReset();
	bool flagfailed=false;
	int key;
	while(intlist.Read(key)==true)
		{
		MBuffer filepath(MStdPathMaxSize);
		filepath.SetString(absstorepath.Get() );
		filepath.StringAppend("/");
		filepath.StringAppend(MStdStr(key) );

		if(fileops.Exists(filepath.GetBuffer())==false)
			{
			MStdPrintf("**Missing file %d in data store\n",key);
			flagfailed=true;
			}
		}

	if(flagfailed==true) { return false; }
	reader.Destroy();
	sqlite.Destroy();

	return true;	
	}



//******************************************************
//**  MJangData class
//******************************************************
void MJangData::ClearObject(void)
	{
	}


////////////////////////////////////////////////
MJangData::MJangData(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MJangData::~MJangData(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MJangData::Create(const char *storagedir,const char *dbfile)
	{
	Destroy();
	// Check if Folder Exists

	MFileOps fileops(true);
	if(fileops.GetAbsolutePath(storagedir,mStorageDir)==false)
		{
		Destroy();
		return false;
		}

	MDirOps dirops(true);
	if(dirops.Exists(mStorageDir.Get() )==false)
		{
		Destroy();
		return false;
		}

	if(fileops.Exists(dbfile)==false)
		{
		Destroy();
		return false;
		}

	// Open the sqlite db
	if(mJangDB.Create(dbfile)==false)
		{
		Destroy();
		return false;
		}

	//**Get User Name
	char username[100];
	if(MStdGetUserName(username,sizeof(username)-2)==false)
		{
		Destroy();
		return false;
		}

	// Update the username info
	mUserName.Create(username);

	return true;
	}

////////////////////////////////////////////////
bool MJangData::Create(const char *configfilelocation)
	{
	Destroy();
	
	// Read the configuration file
	MConfigFile configfile;
	if(configfile.Create()==false)
		{
		Destroy();
		return false;
		}

	if(configfile.ReadFile(configfilelocation)==false)
		{
		Destroy();
		return false;
		}
	


	//=We Have loaded the config file into memory
	
	// Check if ini config entriies are fine
	const char *jangdatastore=configfile.Get(GJangDataStore);
	if(jangdatastore==0)
		{
		Destroy();
		return false;
		}

	const char *jangdatadb=configfile.Get(GJangDataDB);
	if(jangdatadb==0)
		{
		Destroy();
		return false;
		}

	if(Create(jangdatastore,jangdatadb)==false)
		{
		Destroy();
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MJangData::Destroy(void)
	{
	mStorageDir.Destroy();
	mJangDB.Destroy();
	ClearObject();
	return true;
	}


///////////////////////////////////////////////////////////////////////
bool MJangData::ModuleSearch(MStringList &searchwords,MIntList &retkeys)
	{
	
	// Clean out the search string of special sql characters
	MBuffer buffer(4096);
	// Build up a generic search string
	buffer.SetString("select CID from TModule where CID>0 ");
	searchwords.ReadReset();

	MBuffer cleanword(1000);
	for(const char *word=0;(word=searchwords.ReadString())!=0; )
		{
		cleanword.SetString(word);
		GCleanSQLInput(cleanword.GetBuffer());
		MStdStrTrimLeft(cleanword.GetBuffer());
		MStdStrTrimRight(cleanword.GetBuffer());

		buffer.StringAppend(" and CInfo like '%");
		buffer.StringAppend(cleanword.GetBuffer());
		buffer.StringAppend("%' ");
		}

	MSQLiteReader reader;
	if(mJangDB.Exec(reader,buffer.GetBuffer())==false)
		{
		return false;
		}

	if(retkeys.Create()==false)
		{
		return false;
		}

	for(reader.ReadReset();reader.Read()==true; )
		{
		const char *data=reader.GetColumn(0);
		MStdAssert(data!=0);
		const int keyret=MStdAToI(data);
		if(retkeys.Add(keyret)==false)
			{
			return false;
			}
		}

	return true;
	}


//////////////////////////////////////////////////////////
int MJangData::ModuleAdd(const char *directory,const char *info)
	{
	// No Empty Information in info
	MBuffer bufinfo(1000);
	bufinfo.SetString(info);
	GCleanSQLInput(bufinfo.GetBuffer() );
	
	MDirOps dirops(true);
	if(dirops.Exists(directory)==false)
		{
		MStdPrintf("** Add Failed: Directory %s does not exist\n",directory);
		return 0;
		}

	// Convert the directory to abspath
	MFileOps fileops(true);
	MString abspath;
	if(fileops.GetAbsolutePath(directory,abspath)==false)
		{
		MStdPrintf("**Unable to get absolute path to directory %s\n",directory);
		return 0;
		}

	// Extract the directory name
	MFilePathBuilder pathbuilder;
	if(pathbuilder.Create(abspath.Get())==false)
		{
		MStdPrintf("**Unable to parse path %s\n",abspath.Get() );
		return 0;
		}

	//**Insert new row into table
	MBuffer sql(2000);
	MStdSPrintf(sql.GetBuffer(),sql.GetSize()-2,
			"insert into TModule(CID,CUser,CInfo,CDirName) "
			"select Max(CID)+1,'%s','%s','%s' from TModule "
			,mUserName.Get(),bufinfo.GetBuffer(),pathbuilder.GetTop() );

	if(mJangDB.Exec(sql.GetBuffer())==false)
		{
		MStdPrintf("**Unable to add data to database\n");
		return 0;
		}

	// Get the new rowid
	MStdSPrintf(sql.GetBuffer(),sql.GetSize()-2,
			"select Max(CID) from TModule where CUser='%s' and CInfo='%s' and CDirName='%s'"
			,mUserName.Get(),bufinfo.GetBuffer(),pathbuilder.GetTop() );

	// Get the new rowid
	int newrowid;
	if(mJangDB.Exec(sql.GetBuffer(),newrowid)==false)
		{
		return 0;
		}	
	
	// ** Save the files
	MZipOps zipops(true);
	MString outputzipfile;	
	if(zipops.CompressFolder(abspath.Get(),GTmpZipName,outputzipfile)==false)
		{
		MStdPrintf("**Unable to compress folder %s\n",abspath.Get() );
		return 0;
		}

	//** Now We need to copy to target folder and delete the compressed file
	MFilePathBuilder targetpathbuilder;
	if(targetpathbuilder.Create(mStorageDir.Get())==false)
		{
		return 0;
		}

	targetpathbuilder.Push(MStdStr(newrowid) );

	MBuffer targetpathname;
	if(targetpathbuilder.GetFullPath(targetpathname)==false)
		{
		return 0;
		}
	
	if(fileops.Copy(outputzipfile.Get(),targetpathname.GetBuffer())==false)
		{
		return 0;
		}

	fileops.Delete(GTmpZipName);
	
	MStdPrintf("Compressed and saved folder %s as module %d\n",directory,newrowid);

	return newrowid;
	}


//////////////////////////////////////////////////////////
bool MJangData::ModuleDel(int modulekey)
	{
	if(modulekey<=0)
		{
		MStdPrintf("**Bad Module Key %d\n",modulekey);
		return false;
		}

	// Get Module Information
	MString modinfo,moduser,moddirname;
	if(ModuleGetInfo(modulekey,modinfo,moduser,moddirname)==false)
		{
		MStdPrintf("**Unable to get module information for %d\n",modulekey);
		return false;
		}

	// Confirm that the module owner is correct
	if(MStdStrCmp(moduser.Get(),mUserName.Get())!=0)
		{
		MStdPrintf("**Only module %d owner %s can delete\n",modulekey,moduser.Get() );
		return false;
		}

	MBuffer sql(1000);
	MStdSPrintf(sql,sql.GetSize()-2,"delete from TModule where CID=%d and CUser='%s'",modulekey,mUserName.Get() );

	if(mJangDB.Exec(sql.GetBuffer())==false)
		{
		MStdPrintf("**Module %d could not be deleted\n",modulekey);
		return false;
		}

	MFilePathBuilder pathbuilder;
	if(pathbuilder.Create(mStorageDir.Get())==false)
		{
		return false;
		}

	pathbuilder.Push(MStdStr(modulekey) );

	MBuffer fullpathtomodule;
	pathbuilder.GetFullPath(fullpathtomodule);

	MFileOps fileops(true);
	fileops.Delete(fullpathtomodule.GetBuffer() );

	MStdPrintf("Module %d has been deleted.\n",modulekey);
	return true;
	}


////////////////////////////////////////////////
bool MJangData::ModuleGetInfo(int modulekey,MString &info,MString &username,MString &dirinfo)
	{
	MBuffer sql(1000);
	MStdSPrintf(sql.GetBuffer(),sql.GetSize(),
		"select CInfo,CUser,CDirName from TModule where CID>0 and CID=%d",modulekey);

	// Execute SQL for data
	MSQLiteReader reader;
	if(mJangDB.Exec(reader,sql.GetBuffer())==false)
		{
		return false;
		}

	if(reader.GetRowCount()!=1 || reader.GetColumnCount()!=3)
		{
		return false;
		}

	reader.ReadReset();
	if(reader.Read()==false)
		{
		return false;
		}

	info.Create(reader.GetColumn(0) );
	username.Create(reader.GetColumn(1) );
	dirinfo.Create(reader.GetColumn(2) );

	return true;
	}


////////////////////////////////////////////////////////////////
bool MJangData::ModuleDump(int modulekey)
	{
	MStdAssert(modulekey>0);
	MString username;
	MString dirname;
	MString info;
	if(ModuleGetInfo(modulekey,info,username,dirname)==false)
		{
		MStdPrintf("**Unable to get information about module %d\n",modulekey);
		return false;
		}

	// Check if directory or file exists with the same name
	MFileOps fileops(true);
	MDirOps dirops(true);
	if(fileops.Exists(dirname.Get())==true || dirops.Exists(dirname.Get() )==true)
		{
		MStdPrintf("**File or Directory already exists with name %s for module %d\n"
				,dirname.Get(),modulekey);
		return false;
		}

	// Build up the source zip file
	MBuffer srczip(MStdPathMaxSize);
	srczip.SetString(mStorageDir.Get());
	srczip.StringAppend("/");
	srczip.StringAppend(MStdStr(modulekey) );

	fileops.Copy(srczip.GetBuffer(),GTmpZipName);
	
	// Uncompress the zip file
	MZipOps zipops(true);
	zipops.Decompress(GTmpZipName);
	fileops.Delete(GTmpZipName);

	MStdPrintf("Saved Module %d to directory %s\n",modulekey,dirname.Get() );
	return true;
	}	


