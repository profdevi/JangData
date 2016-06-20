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


//v1.0 copyright Comine.com 20160620M1207
#include "MStdLib.h"
#include "TVector.h"
#include "MBuffer.h"
#include "MStringSplitter.h"
#include "MWUProcess.h"


//******************************************************
//**  Windows Implementation
//******************************************************
#if defined(MSTDLIB_OS_WINDOWS)
#include <windows.h>


////////////////////////////////////////////////////
void MWUProcess::ClearObject(void)
	{
	mhProcess=INVALID_HANDLE_VALUE;
	}


////////////////////////////////////////////////
MWUProcess::MWUProcess(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWUProcess::~MWUProcess(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWUProcess::Create(const char *commandline,const char *currentdir)
	{
	Destroy();

	// Init STARTUPINFOA
	STARTUPINFOA startinfo;
	MStdMemSet(&startinfo,0,sizeof(startinfo) );
	startinfo.cb=sizeof(startinfo);

	PROCESS_INFORMATION procinfo;
	MStdMemZero(&procinfo,sizeof(procinfo) );

	if(CreateProcessA(NULL,(LPSTR)commandline,NULL,NULL
			,FALSE,NORMAL_PRIORITY_CLASS
			,NULL,currentdir,&startinfo,&procinfo)==FALSE)
		{
		Destroy();
		return false;
		}

	// Release Thread handle
	CloseHandle(procinfo.hThread);

	mhProcess=procinfo.hProcess;
	return true;
	}


////////////////////////////////////////////////
bool MWUProcess::Destroy(void)
	{
	CloseHandle(mhProcess);
	ClearObject();
	return true;
	}




////////////////////////////////////////////////
int MWUProcess::Wait(void)
	{
	return (int)WaitForSingleObject(mhProcess,INFINITE);
	}



//******************************************************
//**  Linux Implemntation
//******************************************************
#elif defined(MSTDLIB_OS_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

////////////////////////////////////////////////////
void MWUProcess::ClearObject(void)
	{
	mChildPID=0;
	}


////////////////////////////////////////////////
MWUProcess::MWUProcess(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWUProcess::~MWUProcess(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWUProcess::Create(const char *commandline,const char *currentdir)
	{
	Destroy();

	mChildPID=fork();
	if(mChildPID<0)
		{
		Destroy();
		return false;
		}
	else if(mChildPID>0)
		{
		//=We have the parent process
		return true;
		}

	//=This system the child process

	MStringSplitter splitter;
	if(splitter.Create(commandline," \t\n\r")==false)
		{
		Destroy();
		return false;
		}

	const int argcount=splitter.GetCount();
	TVector<const char *> argv;
	if(argv.Create(argcount+1)==false)
		{
		Destroy();
		return false;
		}

	argv.Set(argcount,0);
	for(int i=0;i<argcount;++i)
		{
		argv.Set(i,splitter.Get(i));
		}

	// Set Current Directory
	if(currentdir!=0)
		{
		if(chdir(currentdir)<0)
			{
			Destroy();
			return false;
			}
		}

	//=argv is now contains the arguments
	const int retexec=execvp(argv.Get(0),(char *const *)argv.Get());
	if(retexec<0)
		{
		Destroy();
		exit(-1);
		return false;
		}
	
	Destroy();
	exit(1);
	return true;
	}


////////////////////////////////////////////////
bool MWUProcess::Destroy(void)
	{
	ClearObject();
	return true;
	}



///////////////////////////////////////////////
int MWUProcess::Wait(void)
	{
	int status;
	wait(&status);
	return status;
	}


//******************************************************
//**  Unknown System
//******************************************************
#else
	#error "Unimplemented for current system "
#endif // MSTDLIB_OS_WINDOWS


