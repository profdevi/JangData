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


//v0.0 copyright Comine.com 20150816U1847
#ifndef MWUProcess_h
#define MWUProcess_h

/////////////////////////////////////////////////
#include "MStdLib.h"

/*
	Class gives an OS Neutral Version of Process Execution.
*/


//*********************************************************
//*   Windows Version of Class
//*********************************************************
#if defined(MSTDLIB_OS_WINDOWS)
#include <windows.h>

//////////////////////////////////////////
class MWUProcess
	{
	////////////////////////////////////////////////
	HANDLE mhProcess;
		
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MWUProcess(void);
	~MWUProcess(void);
	bool Create(const char *commandline,const char *currentdir,const char *environblock[]=NULL);
	bool Destroy(void);
	int Wait(void);								// Wait Till Process Finishes
	};


//*********************************************************
//*  Unix Implementation
//*********************************************************
#elif defined(MSTDLIB_OS_UNIX)
#include <unistd.h>

//////////////////////////////////////////
class MWUProcess
	{
	////////////////////////////////////////////////
	int mChildPID;
		
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MWUProcess(void);
	~MWUProcess(void);
	bool Create(const char *commandline,const char *currentdir,const char *environblock[]=NULL);
	bool Destroy(void);
	int Wait(void);								// Wait Till Process Finishes
	};



//******************************************************
//**  MWUProcess  for Unix
//******************************************************
class MWUProcess
	{
	////////////////////////////////////////////////
	int mChildPID;
		
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MWUProcess(void);
	~MWUProcess(void);
	bool Create(const char *commandline,const char *currentdir,const char *environblock[]=NULL);
	bool Destroy(void);
	int Wait(void);								// Wait Till Process Finishes
	};


//*********************************************************
//*  Unknown Operating System
//*********************************************************
#else
	#error "Unimplemented for current system "
#endif // MSTDLIB_OS_WINDOWS

#endif // MWUProcess_h

