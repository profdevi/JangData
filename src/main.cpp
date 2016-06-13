//v1.11 copyright Comine.com 20150810M1317
#include "MStdLib.h"
#include "MCommandArg.h"


//******************************************************
//* Module Elements
//******************************************************
static const char *GApplicationName="AppName";	// Used in Help
static const char *GApplicationVersion="0.0";	// Used in Help

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
				"   usage:  %s [-?]\n"
				"           v%s copyright Comine.com\n"
				"\n"
				"\n"
				,GApplicationName,GApplicationVersion);
	}


