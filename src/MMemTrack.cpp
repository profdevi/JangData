//v2.0 copyright Comine.com 20150810M0824
#include "MStdLib.h"
#include "MMemTrack.h"


////////////////////////////////////////////////////////////
#if (defined(MSTDLIB_API_WINDOWS) )
#include <windows.h>

////////////////////////////////////////////////////////////
#elif ( defined(MSTDLIB_API_LINUX) )
#include <new>
#include <pthread.h>

////////////////////////////////////////////////////////////
#elif defined(MSTDLIB_OS_MOSYNC)

#endif // MOSWINDOWS

///////////////////////////////////////////////////////
// Critical Section Variables
#if ( defined(MSTDLIB_API_WINDOWS) )
static volatile LONG GCriticalSectionCounter=0;

//////////////////////////////////////////////////////
#elif (defined(MSTDLIB_API_LINUX) )
static pthread_mutex_t  GCriticalSectionMutex=PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////////////////
#elif defined(MSTDLIB_OS_MOSYNC)
static volatile int GCriticalSectionCounter=0;

#endif // MSTDLIB_API_WINDOWS


///////////////////////////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
static void GCriticalSectionEnter(void)
	{
	for(;;)
		{
		long newvalue;
		newvalue=InterlockedIncrement((LPLONG)&GCriticalSectionCounter);
		if(newvalue==1) { return; }
		InterlockedDecrement((LPLONG)&GCriticalSectionCounter);
		}
	}


///////////////////////////////////////
static void GCriticalSectionLeave(void)
	{
	InterlockedDecrement((LPLONG)&GCriticalSectionCounter);
	}

////////////////////////////////////////////////////////////
#elif (defined(MSTDLIB_API_LINUX) )
static void GCriticalSectionEnter(void)
	{
	if(pthread_mutex_lock(&GCriticalSectionMutex)!=0)
		{
		return;
		}
	}


///////////////////////////////////////
static void GCriticalSectionLeave(void)
	{
	if(pthread_mutex_unlock(&GCriticalSectionMutex)!=0)
		{
		return;
		}
	}


////////////////////////////////////////////////////////////
#elif defined(MSTDLIB_OS_MOSYNC)
static void GCriticalSectionEnter(void)
	{
	for(;;)
		{
		GCriticalSectionCounter = GCriticalSectionCounter + 1;
		
		if(GCriticalSectionCounter==1) { return; }

		GCriticalSectionCounter = GCriticalSectionCounter - 1;
		}
	}


///////////////////////////////////////
static void GCriticalSectionLeave(void)
	{
	GCriticalSectionCounter=0;
	}

#endif // MSTDLIB_API_WINDOWS

//*******************************************************
//** Node Tracking Infor
//*******************************************************
struct GMemTrackNodeInfo
	{
	const char *FileName;
	int LineNo;
	bool IsArray;
	int BlockSize;
	void *MemBlock;
	GMemTrackNodeInfo * Next;
	};


/////////////////////////////////////////////////////////
static GMemTrackNodeInfo *GMemTrackList=NULL;

/////////////////////////////////////////
bool MMemTrackAdd(void *memory,int blocksize,bool isarray,const char *filename,int lineno)
	{
	if(memory==NULL) { return true; }

	GMemTrackNodeInfo *newnode=new(std::nothrow) GMemTrackNodeInfo;
	if(newnode==NULL)
		{
		return false;
		}

	newnode->BlockSize = blocksize;
	newnode->FileName = filename;
	newnode->LineNo = lineno;
	newnode->IsArray = isarray;
	newnode->MemBlock = memory;
	
	GCriticalSectionEnter();
	newnode->Next = GMemTrackList;
	GMemTrackList=newnode;
	GCriticalSectionLeave();
	
	return true;
	}


/////////////////////////////////////////
bool MMemTrackDel(void *memory,bool isarray,const char *filename,int lineno)
	{
	if(memory==NULL)
		{
		return false;
		}

	// Enter Critical Section
	GCriticalSectionEnter();

	GMemTrackNodeInfo *ptr=GMemTrackList;
	if(ptr==NULL)
		{
		GCriticalSectionLeave();
		return false;
		}

	// Check in first node
	if(ptr->MemBlock == memory)
		{
		// check if array deallaction
		if(ptr->IsArray != isarray)
			{
			//=Continue Anyway
			}

		GMemTrackList=GMemTrackList -> Next;

		delete ptr;
		GCriticalSectionLeave();
		return true;
		}
	
	for(ptr=GMemTrackList;ptr->Next!=NULL;ptr = ptr->Next )
		{
		GMemTrackNodeInfo *nextnode=ptr->Next;
		if(nextnode->MemBlock != memory) { continue; }
		
		if(nextnode->IsArray != isarray)
			{
			//=Continue anyway
			}

		ptr->Next = nextnode->Next;
		delete nextnode;
		GCriticalSectionLeave();
		return true;
		}
	
	GCriticalSectionLeave();
	return false;
	}


///////////////////////////////////////////
void MMemTrackPrint(void)
	{
	GCriticalSectionEnter();

	MStdPrintf("  **   Dynamic Memory List        ** \n");
	MStdPrintf(" ------------------------------------\n");

	int blockcount=0;
	unsigned int memcount=0;
	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		const char *arraydata="     ";
		if(p->IsArray==true)
			{  arraydata = "array";  }

		MStdPrintf("%-30s(%4d) %s : %d bytes\n",p->FileName,p->LineNo,arraydata,p->BlockSize);

		blockcount = blockcount + 1;
		memcount = memcount + p->BlockSize;
		}

	MStdPrintf("  %d Blocks    %u bytes\n",blockcount,memcount);

	GCriticalSectionLeave();
	}


///////////////////////////////////////////
bool MMemTrackPrintDebug(void)
	{
	MMemTrackPrint();
	return true;
	}


//////////////////////////////////////////
bool MMemTrackPrint(void *block)
	{
	GCriticalSectionEnter();

	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		if(p->MemBlock!=block) { continue; }

		const char *arraydata="     ";
		if(p->IsArray==true)
			{  arraydata = "array";  }
		
		MStdPrintf("Memory: %s(%d) %s %d bytes\n",p->FileName,p->LineNo
				,arraydata,p->BlockSize);
		}
	
	GCriticalSectionLeave();

	MStdPrintf("Memory Block: Unknown \n");
	return false;
	}


//////////////////////////////////////////
bool MMemTrackIsValid(void)
	{
	GCriticalSectionEnter();

	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		if(p->FileName==NULL) { return false; }
		if(p->LineNo<=0) { return false; }
		if(p->MemBlock==NULL) { return false; }
		if(p->BlockSize<=0) { return false; }
		}

	GCriticalSectionLeave();
	return true;
	}


//////////////////////////////////////////
int MMemTrackGetBlockCount(void)
	{
	GCriticalSectionEnter();
	int blockcount=0;
	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		blockcount = blockcount + 1;
		}	
	
	GCriticalSectionLeave();
	return blockcount;
	}


//////////////////////////////////////////
unsigned int MMemTrackGetMemorySize(void)
	{
	GCriticalSectionEnter();
	unsigned int memcount=0;
	volatile GMemTrackNodeInfo *p;
	for(p=GMemTrackList;p!=NULL;p=p->Next)
		{
		memcount = memcount + p->BlockSize;
		}

	GCriticalSectionLeave();
	return memcount;
	}


