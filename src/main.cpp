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


//v1.2 copyright Comine.com 20160630R1223
#include "MStdLib.h"
#include "MCommandArg.h"
#include "MBuffer.h"
#include "MDirOps.h"
#include "MFileOps.h"
#include "MStringList.h"
#include "MIntList.h"
#include "MJangData.h"


//******************************************************
//* Module Elements
//******************************************************
static const char *GApplicationName="JData";	// Used in Help
static const char *GApplicationVersion="1.2";	// Used in Help
static const char *GLocalDir="jdata";
static const char *GLocalDB="jdata.db";
static const char *GGlobalDir="/jdata";
static const char *GGlobalDB="/jdata/jdata.db";

////////////////////////////////////////////////////
static void GDisplayHelp(void);
static bool GInitStorage(MJangData &jdata,bool localrepository=true);
static bool GArgGlobalInitialize(void);
static bool GArgLocalInit(void);			// Initialize the local storage
static bool GArgAdd(MJangData &jdata,const char *dirtostore,const char *info);
static bool GArgDel(MJangData &jdata,int moduleid);
static bool GArgGet(MJangData &jdata,int moduleid);
static bool GArgSearch(MJangData &jdata,MStringList &searchlist);

////////////////////////////////////////////////////
int main(int argn,const char *argv[])
	{
	MCommandArg args(argn,argv);

	///////////////////////////////////////////////
	if(args.GetArgCount()<2)
		{
		GDisplayHelp();
		return 0;
		}

	if(args.CheckRemoveHelp()==true)
		{
		GDisplayHelp();
		return 0;
		}

	// Init the local storage
	if(args.CheckRemoveArg("-localinit")==true)
		{
		if(GArgLocalInit()==false) {  return 1;  }
		return 0;
		}

	// Init the local storage
	if(args.CheckRemoveArg("-globalinit")==true)
		{
		if(GArgGlobalInitialize()==false) {  return 1;  }
		return 0;
		}


	MJangData jdata;
	bool flaglocalrepos=true;
	if(args.CheckRemoveArg("-global")==true)
		{
		flaglocalrepos=false;
		}

	if(GInitStorage(jdata,flaglocalrepos)==false)
		{
		MStdPrintf("**Unable to init services\n");
		return 1;
		}

	// Add a module
	if(args.CheckRemoveArg("-add")==true)
		{
		if(args.GetArgCount()<3)
			{
			MStdPrintf("** Bad usage of -add. You need the directory and info to add\n");
			return 2;
			}

		if(GArgAdd(jdata,args.GetArg(1),args.GetArg(2))==false)
			{
			MStdPrintf("**Module Add failed for directory %s\n",args.GetArg(1) );
			return 3;
			}

		return 0;
		}

	// Delete a module
	if(args.CheckRemoveArg("-del")==true)
		{
		if(args.GetArgCount()<2)
			{
			MStdPrintf("** Bad usage of -del. You need to pass the module id to delete\n");
			return 4;
			}

		const int moduleid=MStdAToI(args.GetArg(1) );
		if(moduleid<=0)
			{
			MStdPrintf("** Module Id %d is bad\n",moduleid);
			return 5;
			}

		if(GArgDel(jdata,moduleid)==false)
			{
			MStdPrintf("**Delete of module %d failed\n",moduleid);
			return 6;
			}

		return 0;
		}

	// Get a module
	if(args.CheckRemoveArg("-get")==true)
		{
		if(args.GetArgCount()<2)
			{
			MStdPrintf("** Bad usage of -get. You need to pass the module id to get.\n");
			return 4;
			}

		const int moduleid=MStdAToI(args.GetArg(1) );
		if(moduleid<=0)
			{
			MStdPrintf("** Module Id %d is bad\n",moduleid);
			return 5;
			}

		if(GArgGet(jdata,moduleid)==false)
			{
			return 7;
			}

		return 0;
		}

	// Search for modules
	if(args.CheckRemoveArg("-search")==true)
		{
		if(args.GetArgCount()<2)
			{
			MStdPrintf("** Bad usage of -search. You need to pass search words.\n");
			return 7;
			}
		
		const int argcount=args.GetArgCount();
		MBuffer word(1000);
		MStringList wordlist(true);
		for(int i=1;i<argcount;++i)
			{
			word.SetString(args.GetArg(i) );
			MStdStrTrimLeft(word.GetBuffer() );
			MStdStrTrimRight(word.GetBuffer() );
			wordlist.AddString(word.GetBuffer() );
			}

		if(GArgSearch(jdata,wordlist)==false)
			{
			return 8;
			}

		return 0;
		}
	jdata.Destroy();

	GDisplayHelp();
	return 0;
	}


////////////////////////////////////////////////////
static void GDisplayHelp(void)
	{
	MStdPrintf(	"\n"
				"   usage:  %s [-?] [-global|-localinit|-globalinit] <ops> \n"
				"           v%s copyright Comine.com.\n"
				"           Open Source http://github.com/profdevi/JangData \n"
				"\n"
				"          -global     : selects the shared global repository.\n"
				"          -localinit  : initialize the users personal storage folder\n"
				"          -globalinit : Initialize the global shared repository\n"
				"\n"
				"   Examples 1:  Search for modules containing dehppv\n"
				"        $ jdata -search dehppv \n"
				"\n"
				"   Examples 2:  Store the folder simdata with comment\n"
				"        $ jdata -add simdata \"Mol Dyn Simulation data for LH2 embedded in water at room temperature\" \n"
				"\n"
				"   Examples 3:  Retrive module 100 into current folder\n"
				"        $ jdata -get 100\n"
				"\n"
				"   Example 4: Delete module 1203 from storage\n"
				"        $ jdata -del 1203\n"
				"\n"
				,GApplicationName,GApplicationVersion);
	}


/////////////////////////////////////////////////////////////////////
static bool GInitStorage(MJangData &jdata,bool localrepository)
	{
	if(localrepository==true)
		{
		MBuffer userhomedir(MStdPathMaxSize);
		if(MStdGetUserHome(userhomedir.GetBuffer(),userhomedir.GetSize()-2)==false)
			{
			MStdPrintf("**Unable to get user home directory\n");
			return false;
			}

		userhomedir.StringAppend("/");
		userhomedir.StringAppend(GLocalDir);
		
		MBuffer userdb(MStdPathMaxSize);
		userdb.SetString(userhomedir.GetBuffer());
		userdb.StringAppend("/");
		userdb.StringAppend(GLocalDB);

		if(jdata.Create(userhomedir.GetBuffer(),userdb.GetBuffer())==false)
			{
			MStdPrintf("**Unable to open local storage\n");
			MStdPrintf("     You may need to -localinit the storage first\n");
			return false;
			}

		return true;
		}

	//= We should use the global repository

	if(jdata.Create(GGlobalDir,GGlobalDB)==false)
		{
		MStdPrintf("**Unable to open global data file for sharing\n");
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////
static bool GArgGlobalInitialize(void)
	{
	MFileOps fileops(true);
	MDirOps dirops(true);
	if(dirops.Exists(GGlobalDir)==false)
		{
		if(MJangDataCreate(GGlobalDir,GGlobalDB)==false)
			{
			MStdPrintf("**Unable to create global storage /jdata and db file /jdata/jdata.db\n");
			return false;
			}

		MStdPrintf("Created /jdata and /jdata/jdata.db file\n");
		return true;
		}

	MStdPrintf("**Global /jdata folder already exists");
	return false;
	}


////////////////////////////////////////////////////////
static bool GArgLocalInit(void)
	{
	//** Create the initial local storage dir and DB file.

	// Build up Target db store and path
	MBuffer homedir(MStdPathMaxSize);
	if(MStdGetUserHome(homedir.GetBuffer(),homedir.GetSize()-2)==false)
		{
		MStdPrintf("**Unable to get home directory\n");
		return false;
		}

	homedir.StringAppend("/jdata");
	
	MBuffer dbpath(MStdPathMaxSize);
	dbpath.SetString(homedir.GetBuffer() );
	dbpath.StringAppend("/jdata.db");

	MDirOps dirops(true);
	if(dirops.Exists(homedir.GetBuffer())==true)
		{
		MStdPrintf("**Local storage folder %s exists already.\n",homedir.GetBuffer() );
		return false;
		}

	//=Local folder does not exist

	// Create new storage directory.
	if(MJangDataCreate(homedir.GetBuffer(),dbpath.GetBuffer())==false)
		{
		MStdPrintf("**Unable to create a local storage directory %s.\n",homedir.GetBuffer());
		return false;
		}

	MStdPrintf("Created local storage folder %s\n",homedir.GetBuffer() );
	return false;
	}


/////////////////////////////////////////////////////
static bool GArgAdd(MJangData &jdata,const char *dirtostore,const char *info)
	{
	// Add Passed argument
	const int moduleid=jdata.ModuleAdd(dirtostore,info);
	if(moduleid<=0)
		{
		MStdPrintf("**Unable to add dir %s to storage\n",dirtostore);
		return false;
		}

	MStdPrintf("Added directory %s to storage as id %d\n",dirtostore,moduleid);
	return true;
	}


////////////////////////////////////////////////////////////////////
static bool GArgDel(MJangData &jdata,int moduleid)
	{
	MString username;
	MString modinfo;
	MString dirname;

	if(jdata.ModuleGetInfo(moduleid,modinfo,username,dirname)==false)
		{
		MStdPrintf("**Module ID is unknown. Delete failed for id %d\n",moduleid);
		return false;
		}

	MStdPrintf("**********************************************************\n");
	MStdPrintf("Module ID   : %d\n",moduleid);
	MStdPrintf("User Name   : %s\n",username.Get() );
	MStdPrintf("Info        : %s\n",modinfo.Get() );
	MStdPrintf("dirname     : %s\n",dirname.Get() );
	MStdPrintf("**********************************************************\n");
	MStdPrintf("\n");
	MStdPrintf("Do you really want to delete module %d? Type yes to continue: ",moduleid);
	
	char buf[100];
	MStdGetString(buf,sizeof(buf)-2);
	MStdStrTrim(buf);

	if(MStdStrCmp(buf,"yes")!=0)
		{
		MStdPrintf("**Module Delete cancelled since you did not type yes\n");
		return true;
		}

	if(jdata.ModuleDel(moduleid)==false)
		{
		MStdPrintf("Module delete of %d failed\n",moduleid);
		return false;
		}

	return true;
	}

/////////////////////////////////////////////////////////////
static bool GArgGet(MJangData &jdata,int moduleid)
	{
	MString username;
	MString modinfo;
	MString dirname;

	if(jdata.ModuleGetInfo(moduleid,modinfo,username,dirname)==false)
		{
		MStdPrintf("**Module ID is unknown. Get failed for id %d\n",moduleid);
		return false;
		}

		
	MDirOps dirops(true);
	MFileOps fileops(true);
	if(dirops.Exists(dirname.Get())==true || fileops.Exists(dirname.Get() )==true)
		{
		MStdPrintf("**File or Directory with name %s exists already in current folder\n"
				,dirname.Get() );
		return false;
		}

	if(jdata.ModuleDump(moduleid)==false)
		{
		return false;
		}

	return true;
	}

///////////////////////////////////////////////
static bool GArgSearch(MJangData &jdata,MStringList &searchlist)
	{
	MIntList intlist;
	if(jdata.ModuleSearch(searchlist,intlist)==false)
		{
		return false;
		}

	intlist.ReadReset();
	const int doccount=intlist.GetCount();
	for(int i=0;i<doccount;++i)
		{
		int modkey;
		if(intlist.Read(modkey)==false)
			{
			return false;
			}

		// Display info about module
		MString username;
		MString modinfo;
		MString dirname;

		if(jdata.ModuleGetInfo(modkey,modinfo,username,dirname)==false)
			{
			MStdPrintf("**Module ID is unknown. Get failed for id %d\n",modkey);
			return false;
			}
		
		MStdPrintf("%-4d %-10s %-10s %s\n",modkey,username.Get(),dirname.Get(),modinfo.Get() );
		}
	
	MStdPrintf("     --Search matched %d documents-- \n",doccount);

	return true;
	}

