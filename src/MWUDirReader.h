//v1.2 copyright Comine.com 20150816U1931
#ifndef MWUDirReader_h
#define MWUDirReader_h

/////////////////////////////////////////////////////
#include "MStdLib.h"

////////////////////////////////////////////////
#if (defined(MSTDLIB_API_WINDOWS) )
#include <windows.h>
#include "MFilePathBuilder.h"
#include "MBuffer.h"
#include "TAutoPtr.h"

///////////////////////////////////////////////
#elif (defined(MSTDLIB_API_LINUX) )
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "MFilePathBuilder.h"
#include "MBuffer.h"
#include "TAutoPtr.h"

#endif // MSTDLIB_API_WINDOWS

//******************************************************
//**  MWUDirReader class
//******************************************************
class MWUDirReader
	{
	////////////////////////////////////////////////
	#if (defined(MSTDLIB_API_WINDOWS) )
	HANDLE mDirHandle;
	TAutoPtr<WIN32_FIND_DATAA> mData;
	bool mDataRead;
	MFilePathBuilder mPathList;
	MBuffer mFullFilePath;
	bool mNoFiles;								//=true if no more files

	///////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	DIR *mDirHandle;
	struct dirent *mData;
	TAutoPtr<struct stat> mStat;
	bool mDataRead;
	MFilePathBuilder mPathList;
	MString mFileName;
	MBuffer mFullFilePath;
	bool mNoFiles;								//=true if no more files

	#endif // MSTDLIB_API_WINDOWS
	
	////////////////////////////////////////////////
	void ClearObject(void);
	
	////////////////////////////////////////////////
	public:
	MWUDirReader(void);
	~MWUDirReader(void);
	bool Create(const char *dirname);
	bool Destroy(void);
	bool Read(void);							// Returns True for each new entry
	const char *GetFileName(void);				// Get a file name
	const char *GetFullFilePath(void);			// Get Full File Path
	bool IsDirectory(void);	
	unsigned long GetFileSizeLow(void);
	unsigned long GetFileSizeHigh(void);
	bool GetCreateTime(time_t &tim);
	bool GetLastAccessTime(time_t &tim);
	bool GetLastWriteTime(time_t &tim);		
	};

#endif // MWUDirReader_h

