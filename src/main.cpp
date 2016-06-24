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


//v0.1 copyright Comine.com 20160624F1239
#include "MStdLib.h"
#include "MCommandArg.h"
#include "MJangData.h"


//******************************************************
//* Module Elements
//******************************************************
static const char *GApplicationName="JData";	// Used in Help
static const char *GApplicationVersion="0.1";	// Used in Help

////////////////////////////////////////////////////
static void GDisplayHelp(void);

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

	args.Show();

	return 0;
	}


////////////////////////////////////////////////////
static void GDisplayHelp(void)
	{
	MStdPrintf(	"\n"
				"   usage:  %s [-?] [-ini=<inifile>] <ops> \n"
				"           v%s copyright Comine.com.\n"
				"           Open Source http://github.com/profdevi/JangData\n"
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


