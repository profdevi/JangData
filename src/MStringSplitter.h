//v2.1 copyright Comine.com 20150816U1925
#ifndef MStringSplitter_h
#define MStringSplitter_h

/////////////////////////////////////////////////////
#include "MStdLib.h"
#include "MBuffer.h"
#include "TVector.h"

//******************************************************
//**  MStringSplitter class
//******************************************************
class MStringSplitter
	{
	////////////////////////////////////////////////
	MBuffer mString;							// Modified String
	TVector<char *> mArray;						// Array of pointers
	int mCount;

	////////////////////////////////////////////////
	void ClearObject(void);
	
	////////////////////////////////////////////////
	public:
	MStringSplitter(void);
	MStringSplitter(const char *str,const char *splitchars="\r\n \t");
	~MStringSplitter(void);
	bool Create(const char *str,const char *splitchars="\r\n \t");
	bool Destroy(void);
	int GetCount(void);							// Get Count of split strings
	const char *Get(int index);					// Get Split String
	};

#endif // MStringSplitter_h

