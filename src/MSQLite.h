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

