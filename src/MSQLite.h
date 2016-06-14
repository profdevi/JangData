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


//v1.10 copyright Comine.com 20160108F1057
#ifndef MSQLite_h
#define MSQLite_h

/////////////////////////////////////////////////////
#include "MStdLib.h"
#include "MString.h"
#include "sqlite3.h"

//******************************************************
//**  MSQLite class
//******************************************************
class MSQLiteReader;
class MSQLite
	{
	////////////////////////////////////////////////
	sqlite3 *mDatabase;
	char *mLastErrorInfo;

	// Used by Call back function
	MSQLiteReader *mCurrentReader;
	int mCurrentReaderMaxRows;
	bool mCallbackFlag;

	////////////////////////////////////////////////
	static int SQLCallBack(void *pArg,int argc,char **argv
			,char **colname);
		
	////////////////////////////////////////////////
	void ClearObject(void);
	int GetLastErrorCode(void);
	const char *GetLastErrorMessage(void);
	
	////////////////////////////////////////////////
	public:
	MSQLite(void);
	MSQLite(const char *filename);
	~MSQLite(void);
	bool Create(const char *filename=0);
	bool Destroy(void);
	bool Exec(MSQLiteReader &reader,const char *sql,int maxrecords=0);
	bool Exec(const char *sql);
	bool Exec(const char *sql,int &result);
	bool Exec(const char *sql,double &result);
	bool Exec(const char *sql,MString &result);
	int ExecGetInt(const char *sql,bool &error);
	double ExecGetDouble(const char *sql,bool &error);
	const char *GetLastError(void);
	bool IsAutoCommitOn(void);
	bool EnableForeignKeys(bool enable);
	bool TransactionBegin(void);
	bool TransactionRollback(void);
	bool TransactionCommit(void);
	};


//******************************************************
//**  MSQLiteReader class
//******************************************************
struct MSQLiteReaderNode;
class MSQLiteReader
	{
	int mColumnCount;
	int mRowCount;
	MSQLiteReaderNode *mList;
	MSQLiteReaderNode *mReadPosition;
	bool mReadPositionReset;

	///////////////////////////////////////////
	void ClearObject(void);
	void ClearList(void);
	
	//////////////////////////////////////////
	public:
	MSQLiteReader(void);
	~MSQLiteReader(void);
	bool Create(int colcount);
	bool Destroy(void);

	/// Add Columns to node
	bool AddNode(void);
	bool AddNodeColumn(const char *column);

	// Reader Interface
	bool ReadReset(void);
	bool Read(void);
	const char *GetColumn(int index);
	int GetColumnCount(void);
	int GetRowCount(void);
	};

#endif // MSQLite_h

