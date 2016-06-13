//v1.2 copyright Comine.com 20150816U1930
#ifndef MFilePathBuilder_h
#define MFilePathBuilder_h

//////////////////////////////////////////////////
#include "MStdLib.h"
#include "MBuffer.h"
#include "MString.h"
#include "TQueueArray.h"

/*
	Class can be used to build paths
	Output Path will have generally /a/b/c format accept in windows
	where no leading / is placed for drives.

	Cannot Push . or .. as path elements

*/

//******************************************************
//**  MFilePathBuilder class
//******************************************************
class MFilePathBuilder
	{
	////////////////////////////////////////////////
	TQueueArray<MString> mList;
	
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MFilePathBuilder(bool create=false);
	MFilePathBuilder(const char *dirpath);
	~MFilePathBuilder(void);
	bool Create(void);
	bool Create(const char *absdirpath);
	bool Destroy(void);
	const char *Get(int index);
	int GetElementCount(void);
	bool GetFullPath(MBuffer &buffer);	
	bool Push(const char *element);			// Push Element into list
	bool Push(MFilePathBuilder &paths);		// Push More Elements at end
	bool Pop(void);							// Pop Element off list
	bool Pop(int count);					// Pop Till Count elements exist
	const char *GetTop(void);				// Get Last Element
	bool IsSamePath(MFilePathBuilder &lst);	// =true if paths are same
	bool IsSamePath(const char *path);		// =true if paths are same
	};

#endif // MFilePathBuilder_h

