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
#include "MStdLib.h"
#include "MString.h"
#include "sqlite3.h"
#include "MSQLite.h"


//******************************************************
//**  Module Elements
//******************************************************
struct GReturnInfo
	{
	int ErrorNumber;
	const char *ErrorInfo;
	};


/////////////////////////////////////////////////////////
struct MSQLiteReaderNode
	{
	char **ColData;
	int CurrentCount;
	MSQLiteReaderNode *Next;
	MSQLiteReaderNode *Last;

	MSQLiteReaderNode(void)
		{
		ColData=NULL;
		Next=NULL;
		Last=NULL;
		}
	};

/////////////////////////////////////////////////////////
static GReturnInfo GReturnInfoArray[]=
	{
	SQLITE_OK,"Successful result",
	SQLITE_ERROR,"SQL error or missing database",
	SQLITE_INTERNAL,"An internal logic error in SQLite",
	SQLITE_PERM,"Access permission denied",
	SQLITE_ABORT,"Callback routine requested an abort",
	SQLITE_BUSY,"The database file is locked",
	SQLITE_LOCKED,"A table in the database is locked",
	SQLITE_NOMEM,"A malloc() failed",
	SQLITE_READONLY,"Attempt to write a readonly database",
	SQLITE_INTERRUPT,"Operation terminated by sqlite_interrupt()",
	SQLITE_IOERR,"Some kind of disk I/O error occurred",
	SQLITE_CORRUPT,"The database disk image is malformed",
	SQLITE_NOTFOUND,"(Internal Only) Table or record not found",
	SQLITE_FULL,"Insertion failed because database is full",
	SQLITE_CANTOPEN,"Unable to open the database file",
	SQLITE_PROTOCOL,"Database lock protocol error",
	SQLITE_EMPTY,"(Internal Only) Database table is empty",
	SQLITE_SCHEMA,"The database schema changed",
	SQLITE_TOOBIG,"Too much data for one row of a table",
	SQLITE_CONSTRAINT,"Abort due to contraint violation",
	SQLITE_MISMATCH,"Data type mismatch",
	SQLITE_MISUSE,"Library used incorrectly",
	SQLITE_NOLFS,"Uses OS features not supported on host",
	SQLITE_AUTH,"Authorization denied",
	SQLITE_FORMAT,"Auxiliary database format error",
	SQLITE_RANGE,"2nd parameter to sqlite3_bind out of range",
	SQLITE_NOTADB,"File opened that is not a database file",
	SQLITE_NOTICE,"Notifications from sqlite3_log()",
	SQLITE_WARNING,"Warnings from sqlite3_log()",
	SQLITE_ROW,"sqlite_step() has another row ready",
	SQLITE_DONE,"sqlite_step() has finished executing",
	-1,NULL
	};


/////////////////////////////////////////////////////////
static const char *GGetError(int value)
	{
	for(int i=0;GReturnInfoArray[i].ErrorInfo!=NULL;++i)
		{
		if(GReturnInfoArray[i].ErrorNumber == value)
			{  return GReturnInfoArray[i].ErrorInfo; }
		}

	return NULL;
	}

//******************************************************
//**  MSQLite class
//******************************************************
int MSQLite::SQLCallBack(void *pArg,int argc,char **argv
			,char **colname)
	{
	MSQLite *mainobj=(MSQLite *)pArg;
	if(mainobj==NULL)
		{
		return 1;
		}
	
	if(mainobj->mCurrentReader==NULL)
		{
		return 1;
		}

	if(mainobj->mCallbackFlag==true)
		{
		mainobj->mCallbackFlag=false;
		if(mainobj->mCurrentReader->Create(argc)==false)
			{
			return 1;
			}
		}

	// Now Add new node and columns
	if(mainobj->mCurrentReader->AddNode()==false)
		{
		return 1;
		}

	if(argv==NULL)
		{
		return 1;
		}

	for(int i=0;i<argc;++i)
		{
		if(mainobj->mCurrentReader->AddNodeColumn(argv[i])==false)
			{
			return 1;
			}
		}

	// Check row count to see if max is reached
	if(mainobj->mCurrentReaderMaxRows>0 &&
			mainobj->mCurrentReader->GetRowCount() >= 
				mainobj->mCurrentReaderMaxRows )
		{  return 1;  }

	return 0;
	}


///////////////////////////////////////////////////
void MSQLite::ClearObject(void)
	{
	mDatabase=NULL;
	mLastErrorInfo=NULL;
	mCurrentReader=NULL;
	mCallbackFlag=false;
	}


//////////////////////////////////////////////////
int MSQLite::GetLastErrorCode(void)
	{  return sqlite3_errcode(mDatabase);  }


//////////////////////////////////////////////////
const char *MSQLite::GetLastErrorMessage(void)
	{	return sqlite3_errmsg(mDatabase);  }


////////////////////////////////////////////////
MSQLite::MSQLite(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MSQLite::MSQLite(const char *filename)
	{
	ClearObject();
	if(Create(filename)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MSQLite::~MSQLite(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MSQLite::Create(const char *dbname)
	{
	Destroy();

	// ref http://www.sqlite.org/inmemorydb.html
	if(dbname==NULL || *dbname==0 )
		{
		dbname=":memory:";
		}

	if(sqlite3_open(dbname,&mDatabase)!=SQLITE_OK)
		{
		const char *error=GetLastErrorMessage();
		if(error==NULL) { error="Unknown Error";  };
		Destroy();  return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MSQLite::Destroy(void)
	{
	if(mDatabase!=NULL){  sqlite3_close(mDatabase); }
	ClearObject();
	return true;
	}



//////////////////////////////////////////////
bool MSQLite::Exec(MSQLiteReader &reader,const char *sql,int maxrecords)
	{
	// Init reader
	if(reader.Create(0)==false)
		{
		return false;
		}

	mCurrentReader=&reader;
	mLastErrorInfo=NULL;
	mCallbackFlag=true;
	mCurrentReaderMaxRows=maxrecords;
	int ret=sqlite3_exec(mDatabase,sql,SQLCallBack,this,&mLastErrorInfo);
	mCurrentReader=NULL;
	mCallbackFlag=false;
	mCurrentReaderMaxRows=0;

	if(ret==SQLITE_ABORT)
		{  return true;  }

	if(ret!=SQLITE_OK)
		{ return false; }

	return true;
	}


///////////////////////////////////////////////////
bool MSQLite::Exec(const char *sql)
	{
	int ret=sqlite3_exec(mDatabase,sql,NULL,NULL,&mLastErrorInfo);
	if(ret!=SQLITE_OK){ return false; }
	return true;
	}


///////////////////////////////////////////////////
bool MSQLite::Exec(const char *sql,int &result)
	{
	MSQLiteReader reader;
	if(Exec(reader,sql)==false)
		{
		return false;
		}

	if(reader.Read()==false || reader.GetColumnCount()<=0 )
		{
		return false;
		}

	result=MStdAToI(reader.GetColumn(0));

	return true;
	}


/////////////////////////////////////////////////
bool MSQLite::Exec(const char *sql,double &result)
	{
	MSQLiteReader reader;
	if(Exec(reader,sql)==false)
		{
		return false;
		}

	if(reader.Read()==false || reader.GetColumnCount()<=0 )
		{
		return false;
		}

	result=MStdAToF(reader.GetColumn(0));

	return true;
	}


/////////////////////////////////////////////////
bool MSQLite::Exec(const char *sql,MString &result)
	{
	MSQLiteReader reader;
	if(Exec(reader,sql)==false)
		{
		return false;
		}

	if(reader.Read()==false || reader.GetColumnCount()<=0 )
		{
		return false;
		}

	return result.Create(reader.GetColumn(0) );
	}


//////////////////////////////////////////////////
int MSQLite::ExecGetInt(const char *sql,bool &error)
	{
	MSQLiteReader reader;
	if(Exec(reader,sql)==false)
		{
		error=true;
		return 0;
		}

	if(reader.Read()==false || reader.GetColumnCount()<=0 )
		{
		error=true;
		return 0;
		}

	const int retval=MStdAToI(reader.GetColumn(0) );	
	error=false;
	return retval;
	}


//////////////////////////////////////////////////
double MSQLite::ExecGetDouble(const char *sql,bool &error)
	{
	MSQLiteReader reader;
	if(Exec(reader,sql)==false)
		{
		error=true;
		return 0;
		}

	if(reader.Read()==false || reader.GetColumnCount()<=0 )
		{
		error=true;
		return 0;
		}

	const double retval=MStdAToF(reader.GetColumn(0) );	
	error=false;

	return retval;
	}


//////////////////////////////////////////////////
const char *MSQLite::GetLastError(void)
	{
	return mLastErrorInfo;
	}


////////////////////////////////////////////////////
bool MSQLite::IsAutoCommitOn(void)
	{
	if(sqlite3_get_autocommit(mDatabase)==1)
		{  return true;  }

	return false;
	}


/////////////////////////////////////////////////////
bool MSQLite::EnableForeignKeys(bool enable)
	{
	if(enable==true)
		{
		if(Exec("PRAGMA foreign_keys = ON")==false)
			{
			return false;
			}

		return true;
		}
	else
		{
		if(Exec("PRAGMA foreign_keys = OFF")==false)
			{
			return false;
			}

		return true;
		}
	}


/////////////////////////////////////////////////
bool MSQLite::TransactionBegin(void)
	{
	return Exec("BEGIN");
	}


/////////////////////////////////////////////////
bool MSQLite::TransactionRollback(void)
	{
	return Exec("ROLLBACK");
	}


////////////////////////////////////////////////
bool MSQLite::TransactionCommit(void)
	{
	return Exec("COMMIT");
	}


//******************************************************
//**  MSQLiteReader class
//******************************************************
void MSQLiteReader::ClearObject(void)
	{
	mColumnCount=0;
	mRowCount=0;
	mList=NULL;
	mReadPosition=NULL;
	mReadPositionReset=true;
	}


//////////////////////////////////////////////////////
void MSQLiteReader::ClearList(void)
	{
	while(mList!=NULL)
		{
		// Release the strings
		for(int i=0;i<mColumnCount;++i)
			{
			if(mList->ColData[i]!=NULL)
				{
				delete[] mList->ColData[i];
				mList->ColData[i]=NULL;
				}
			}

		// Release the row data
		delete[] mList->ColData;
		mList->ColData=NULL;
		
		MSQLiteReaderNode *next=mList->Next;
		if(next!=NULL) { next->Last=NULL; }

		delete mList;
		mList=next;
		}
	}

//////////////////////////////////////////////////////
MSQLiteReader::MSQLiteReader(void)
	{  ClearObject();  }


//////////////////////////////////////////////////////
MSQLiteReader::~MSQLiteReader(void)
	{  Destroy();  }


//////////////////////////////////////////////////////
bool MSQLiteReader::Create(int colcount)
	{
	Destroy();
	mColumnCount=colcount;
	return true;
	}


//////////////////////////////////////////////////////
bool MSQLiteReader::Destroy(void)
	{
	ClearList();
	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////
bool MSQLiteReader::AddNode(void)
	{
	// Allocate new node
	MSQLiteReaderNode *newnode=new MSQLiteReaderNode;
	if(newnode==NULL)
		{
		return false;
		}

	newnode->CurrentCount=0;
	newnode->Last=NULL;
	newnode->Next=NULL;

	// Allocate space for array
	newnode->ColData=new char *[mColumnCount];
	if(newnode->ColData==NULL)
		{
		delete newnode;
		return false;
		}

	// Init data
	for(int i=0;i<mColumnCount;++i)
		{
		newnode->ColData[i]=NULL;
		}

	// Add to Link
	newnode->Next=mList;
	if(mList!=NULL) { mList->Last=newnode; }
	mList=newnode;	

	// Increment row count
	mRowCount = mRowCount + 1;
	return true;
	}


//////////////////////////////////////////////////////
bool MSQLiteReader::AddNodeColumn(const char *column)
	{
	if(mList==NULL)
		{
		return false;
		}

	if(mList->CurrentCount>=mColumnCount)
		{
		return false;
		}

	// If we are adding a NULL column
	if(column==NULL)
		{
		mList->ColData[mList->CurrentCount]=NULL;
		mList->CurrentCount = mList->CurrentCount + 1;
		return true;
		}

	//= Column Data being added
	char *newcoldata;
	newcoldata=new char[MStdStrLen(column)+1];
	if(newcoldata==NULL)
		{
		return false;
		}

	// copy string
	MStdStrCpy(newcoldata,column);

	// Add to the mList
	mList->ColData[mList->CurrentCount]=newcoldata;
	mList->CurrentCount = mList->CurrentCount + 1;
	
	return true;
	}


/////////////////////////////////////////////////////////
bool MSQLiteReader::ReadReset(void)
	{
	mReadPositionReset=true;
	return true;
	}


/////////////////////////////////////////////////////////
bool MSQLiteReader::Read(void)
	{
	if(mReadPositionReset==true)
		{
		mReadPositionReset=false;
		mReadPosition=mList;
		if(mReadPosition==NULL) {  return false;  }

		// Move to first of list
		while(mReadPosition->Next!=NULL)
			{  mReadPosition = mReadPosition->Next; }

		return true;
		}

	if(mReadPosition==NULL) { return false; }

	//=We are pointing to next node
	mReadPosition=mReadPosition->Last;

	if(mReadPosition!=NULL) { return true; }

	return false;
	}


/////////////////////////////////////////////////////////
const char *MSQLiteReader::GetColumn(int index)
	{
	if(index<0 || index>=mColumnCount)
		{
		return NULL;
		}

	if(mReadPosition==NULL) { return NULL; }
	return mReadPosition->ColData[index];
	}


/////////////////////////////////////////////////////////
int MSQLiteReader::GetColumnCount(void)
	{
	return mColumnCount;
	}


/////////////////////////////////////////////////////////
int MSQLiteReader::GetRowCount(void)
	{
	return mRowCount;
	}



