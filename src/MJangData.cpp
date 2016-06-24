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


//v1.0 copyright Comine.com 20160624F1218
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


///////////////////////////////////////////////////////////
bool MJangDataDestroy(const char *inifilename,const char *jangdatastore,const char *datadatadb)
	{
	MFileOps fileops(true);
	fileops.Delete(inifilename);
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

	
	// Save folder
	MFileOps fileops(true);
	if(fileops.GetAbsolutePath(jangdatastore,mStorageDir)==false)
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

	// Check if file and directory exists
	if(fileops.Exists(jangdatadb)==false)
		{
		Destroy();
		return false;
		}

	// Check if Folder Exists
	MDirOps dirops(true);
	if(dirops.Exists(jangdatastore)==false)
		{
		Destroy();
		return false;
		}

	// Open the sqlite db
	if(mJangDB.Create(jangdatadb)==false)
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
		return false;
		}

	// Extract the directory name
	MFilePathBuilder pathbuilder;
	if(pathbuilder.Create(abspath.Get())==false)
		{
		MStdPrintf("**Unable to parse path %s\n",abspath.Get() );
		return false;
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
		return false;
		}

	// Get the new rowid
	MStdSPrintf(sql.GetBuffer(),sql.GetSize()-2,
			"select Max(CID) from TModule where CUser='%s' and CInfo='%s' and CDirName='%s'"
			,mUserName.Get(),bufinfo.GetBuffer(),pathbuilder.GetTop() );

	// Get the new rowid
	int newrowid;
	if(mJangDB.Exec(sql.GetBuffer(),newrowid)==false)
		{
		return false;
		}	
	
	// ** Save the files
	MZipOps zipops(true);
	MString outputzipfile;	
	if(zipops.CompressFolder(abspath.Get(),GTmpZipName,outputzipfile)==false)
		{
		MStdPrintf("**Unable to compress folder %s\n",abspath.Get() );
		return false;
		}

	//** Now We need to copy to target folder and delete the compressed file
	MFilePathBuilder targetpathbuilder;
	if(targetpathbuilder.Create(mStorageDir.Get())==false)
		{
		return false;
		}

	targetpathbuilder.Push(MStdStr(newrowid) );

	MBuffer targetpathname;
	if(targetpathbuilder.GetFullPath(targetpathname)==false)
		{
		return false;
		}
	
	if(fileops.Copy(outputzipfile.Get(),targetpathname.GetBuffer())==false)
		{
		return false;
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


