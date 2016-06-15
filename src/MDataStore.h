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


//v0.1 copyright Comine.com 20160615W01323
#ifndef MDataStore_h
#define MDataStore_h

/////////////////////////////////////////////////
#include "MStdLib.h"
#include "MString.h"
#include "MStringList.h"

/*
	This class stores directories and files as a single compressed 
	file in some storage directory.  All the files are saved with a 
	single integer file name.

			Delete([Data])
			Add([Data],File*,Info)
			Save([Data],TargetDir)
				
*/


//******************************************************
//**  MDataStore class
//******************************************************
class MDataStore
	{
	////////////////////////////////////////////////
	MString mDirStorePath;

	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MDataStore(void);
	~MDataStore(void);
	bool Create(const char *dirstore);
	bool Destroy(void);
	const char *GetStoreDir(void);
	bool Print(void);									// Print Current status
	bool Delete(int dataid);							// Delete the passed data in file store
	bool Add(int dataid,MStringList &filelist,const char *info=0);	// Save Passed files to data store as id
	bool Save(int dataid,const char *targetdir);		// Save the data to passed directory
	};

#endif // MDataStore_h

