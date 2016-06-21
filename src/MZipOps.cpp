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


//v1.1 copyright Comine.com 20160621T1039
#include "MStdLib.h"
#include "MString.h"
#include "MDirOps.h"
#include "MFileOps.h"
#include "MFilePathBuilder.h"
#include "MBuffer.h"
#include "MWUProcess.h"
#include "MZipOps.h"


//******************************************************
//**  MZipOps class
//******************************************************
void MZipOps::ClearObject(void)
	{
	}


////////////////////////////////////////////////
MZipOps::MZipOps(bool create)
	{
	ClearObject();
	if(create==true && Create()==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MZipOps::~MZipOps(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MZipOps::Create(void)
	{
	Destroy();
	return true;
	}


////////////////////////////////////////////////
bool MZipOps::Destroy(void)
	{
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MZipOps::CompressFolder(const char *folder,const char *zipfile,MString& outzipfile)
	{
	MDirOps dirops(true);
	if(dirops.Exists(folder)==false)
		{
		return false;
		}

	MFileOps fileops(true);
	MString foldertozip;
	if(fileops.GetAbsolutePath(folder,foldertozip)==false)
		{
		return false;
		}

	// **Build ip the parent dir and target zip file
	MBuffer buffer(MStdPathMaxSize);
	buffer.SetString(foldertozip.Get());
	buffer.StringAppend("/..");
	
	// Get Parentdir
	MString parentdir;
	if(fileops.GetAbsolutePath(buffer.GetBuffer(),parentdir)==false)
		{
		return false;
		}

	// Get the zip file
	buffer.SetString(parentdir.Get() );
	buffer.StringAppend("/");
	buffer.StringAppend(zipfile);
	if(fileops.GetAbsolutePath(buffer.GetBuffer(),outzipfile)==false)
		{
		return false;
		}


	if(fileops.Exists(outzipfile.Get())==true)
		{
		return false;
		}

	// build up the zip command line
	MBuffer zipcommandline(MStdPathMaxSize);
	zipcommandline.SetString("zip -v -r ");
	zipcommandline.StringAppend(zipfile);
	zipcommandline.StringAppend(" ");


	// Build up the path
	MFilePathBuilder pathbuilder;	
	if(pathbuilder.Create(foldertozip.Get())==false)
		{
		return false;
		}

	MString dirname(pathbuilder.GetTop());
	zipcommandline.StringAppend(dirname.Get());

	// Start Zip Process
	MWUProcess zipprocess;
	if(zipprocess.Create(zipcommandline.GetBuffer(),parentdir.Get())==false)
		{
		return false;
		}

	zipprocess.Wait();	
	return true;
	}


///////////////////////////////////////////////////////
bool MZipOps::Decompress(const char *zipfile)
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

	MBuffer commandline(MStdPathMaxSize);
	commandline.SetString("unzip ");
	commandline.StringAppend(abspath.Get() );

	MWUProcess process;
	if(process.Create(commandline.GetBuffer(),".")==false)
		{
		return false;
		}

	process.Wait();	
	return true;
	}


//////////////////////////////////////////////////////////
bool MZipOps::ZipInfo(const char *zipfile)
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

	MBuffer commandline(MStdPathMaxSize);
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



