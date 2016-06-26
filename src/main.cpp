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


//v0.1 copyright Comine.com 20160625U0949
#include "MStdLib.h"
#include "MCommandArg.h"
#include "MBuffer.h"
#include "MJangData.h"


//******************************************************
//* Module Elements
//******************************************************
static const char *GApplicationName="JData";	// Used in Help
static const char *GApplicationVersion="0.1";	// Used in Help
static const char *GLocalDir="jdata";
static const char *GLocalDB="jdata.db";

////////////////////////////////////////////////////
static void GDisplayHelp(void);
static bool GInitStorage(MJangData &jdata,bool localrepository=true);
static bool GLocalStorageInit(void);			// Initialize the local storage

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

	if(args.CheckRemoveArg("-localinit")==true)
		{
		GLocalStorageInit();
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
		return false;
		}

	jdata.Destroy();
	return 0;
	}


////////////////////////////////////////////////////
static void GDisplayHelp(void)
	{
	MStdPrintf(	"\n"
				"   usage:  %s [-?] [-global|-localinit] <ops> \n"
				"           v%s copyright Comine.com.\n"
				"           Open Source http://github.com/profdevi/JangData\n"
				"\n"
				"     Option -global selects the shared global repository, otherwise the\n"
				"     local repository jdata folder in the users home directory is used \n"
				"     by default.\n"
				"\n"
				"     Option -localinit will initialize the users personal storage folder\n"
				"     and db files.\n"
				"\n"
				"       Examples 1:  Search for modules containing dehppv\n"
				"\n"
				"            $ jdata search dehppv \n"
				"\n"
				"       Examples 2:  Store the folder simdata with comment\n"
				"\n"
				"            $ jdata store simdata \"Mol Dyn Simulation data for LH2 embedded in water at room temperature\" \n"
				"\n"
				"       Examples 3:  Retrive module 100 into current folder\n"
				"\n"
				"            $ jdata get 100\n"
				"\n"
				"       Example 4: Delete module 1203 from storage\n"
				"\n"
				"            $ jdata delete 1203\n"
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
			MStdPrintf("     You may need to init the storage first\n");
			return false;
			}

		return true;
		}

	//= We Should use the global ini file in etc
	MStdPrintf("**Incomplete Global Shared Implementation\n");
	return false;
	}


////////////////////////////////////////////////////////
static bool GLocalStorageInit(void)
	{
	//** Create the initial local storage dir and DB file.
	return false;
	}
