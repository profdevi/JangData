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


//v0.3 copyright Comine.com 20160620M1605
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
#include "MJangData.h"


//******************************************************
//**  Module Elements
//******************************************************
static const char GConfigFileName[]="MJangData.ini";
static const char GJangDataStore[]="MJangDataStore";
static const char GJangDataDB[]="MJangDataDB";

////////////////////////////////////////////////////////////////
// SQL For creation for data store.  Primary Keys>0.  First two rows are just dummy rows 
// for testing purposes.
static const char *GSQLBuild[]=
		{
		"create table TModule(CID integer primary key not null,CInfo varchar(250) not null)",
		"create table TVersion(CID integer primary key not null,CModuleId integer not null references TModule(CID) )",
		"insert into TModule(CID,CInfo) values(0,'Dummy Record')",
		"insert into TVersion(CID,CModuleId) values(0,0)",
		0
		};


/////////////////////////////////////////////////////////////////
/*
	Show ZIP Information
*/
static bool GZipInfo(const char *zipfile)
	{
	MFileOps fileops(true);

	MString abspath;
	if(fileops.GetAbsolutePath(zipfile,abspath)==false)
		{
		return false;
		}

	if(fileops.Exists(abspath.Get() )==false)
		{
		return false;
		}

	MBuffer commandline(2000);
	commandline.SetString("unzip -v ");
	commandline.StringAppend(abspath.Get() );

	MWUProcess process;
	if(process.Create(commandline.GetBuffer(),".")==false)
		{
		return false;
		}

	process.Wait();
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
bool MJangData::Create(void)
	{
	Destroy();
	
	MConfigFile configfile;
	if(configfile.Create()==false)
		{
		Destroy();
		return false;
		}

	// Check the etc folder
	MBuffer configpath(1000);
	MStdSPrintf(configpath,configpath.GetSize()-2,"/etc/%s",GConfigFileName);

	MFileOps fileops(true);
	if(fileops.Exists(GConfigFileName)==true)
		{
		if(configfile.ReadFile(GConfigFileName)==false)
			{
			Destroy();
			return false;
			}
		}
	else if(fileops.Exists(configpath.GetBuffer())==true)
		{
		if(configfile.ReadFile(configpath.GetBuffer())==false)
			{
			Destroy();
			return false;
			}
		}
	else
		{
		Destroy();
		return false;
		}

	//=We Have loaded the config file into memory
	
	// CHeck if ini config entriies are fine
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

	// Check if file and directory exists
	if(fileops.Exists(jangdatadb)==false)
		{
		Destroy();
		return false;
		}

	// Check if FOlder Exists
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

	return true;
	}


////////////////////////////////////////////////
bool MJangData::Destroy(void)
	{
	mJangDB.Destroy();
	ClearObject();
	return true;
	}


///////////////////////////////////////////////////////////////////////
bool MJangData::Search(const char *search,MIntList &retkeys)
	{
	MStdAssert(search!=0);

	// Clean out the search string of special sql characters
	MBuffer buffer(1000);
	buffer.SetString(search);
	MStdStrClean(buffer.GetBuffer());				// remove non ascii characters
	MStdStrRemoveChars(buffer,"\"\';\\");

	// Build up a generic search string
	buffer.StringPrepend("select CID from Module where CID>0 and CInfo like '%");
	buffer.StringAppend("%'");

	

	return true;
	}


