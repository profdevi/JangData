//v1.11 copyright Comine.com 20150813R1241
#ifndef MPathNameList_h
#define MPathNameList_h

//////////////////////////////////////////////////
#include "MBuffer.h"

//******************************************************
//**  MPathNameList class
//******************************************************
class MPathNameList
	{
	////////////////////////////////////////////////
	char mDriveLetter;
	char **mList;
	int mMaxLen;
	int mLen;
	
	////////////////////////////////////////////////
	void ClearObject(void);
	bool InitArray(const char *path);

	////////////////////////////////////////////////
	public:
	MPathNameList(bool create=false);
	~MPathNameList(void);
	bool Create(void);
	bool Create(const char *absdirpath);
	bool Destroy(void);
	const char *Get(int index);
	int GetElementCount(void);
	bool GetFullPath(MBuffer &buffer);	
	bool Push(const char *element);			// Push Element into list
	bool Push(MPathNameList &paths);		// Push More Elements at end
	bool Pop(void);							// Pop Element off list
	bool Pop(int count);					// Pop Till Count elements exist
	const char *GetTop(void);				// Get Last Element
	char GetDriveLetter(void);				// Get Drive Letter
	bool IsSamePath(MPathNameList &lst);	// =true if paths are same
	bool IsSamePath(const char *path);		// =true if paths are same
	};

#endif // MPathNameList_h

