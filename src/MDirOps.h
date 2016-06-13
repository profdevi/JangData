//v1.20 copyright Comine.com 20150813R1238
#ifndef MDirOps_h
#define MDirOps_h

////////////////////////////////////////////////////
#include "MBuffer.h"

//******************************************************
//**  MDirOps class
//******************************************************
class MDirOps
	{
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	void ClearObject(void);
	
	////////////////////////////////////////////////
	public:
	MDirOps(bool create=false);
	~MDirOps(void);
	bool Create(void);
	bool Destroy(void);
	bool MakeDir(const char *dirname,bool erroronfail=false);	// Make a directory

	// Make all subdirectories.  For instance c:\work\tmp\x\y will create
	// subdirectories c:\work, c:\work\tmp, c:\work\tmp\x, c:\work\tmp\x\y
	bool MakeDirNested(const char *dirname);

	bool RemoveDir(const char *dirname,bool erroronfail=false);
	bool Exists(const char *dirname);
	bool IsSystem(const char *dirname);
	bool IsReadOnly(const char *dirname);
	bool IsHidden(const char *dirname);
	bool IsEncrypted(const char *dirname);
	bool IsCompressed(const char *dirname);						// Is Directory Compressed
	bool Move(const char *srcdir,const char *dstdir				// Move a directory
			,bool erroronfail=false);
	bool MoveOnReboot(const char *srcdir,const char *dstdir);	// Move dir after reboot
	bool GetCurrentDir(MBuffer &dir);							// Return Current Directory
	bool GetCurrentDir(char drive,MBuffer &dir);				// Return Current Directory of drive
	bool GetTempDir(MBuffer &buf);								// Get Temp Directory with \ end
	bool SetDir(const char *dir);								// Change Directory
	bool IsReadable(const char *dir);							// Is Directory Readable
	};

#endif // MDirOps_h

