//v2.0 copyright Comine.com 20150810M0824
#ifndef MMemTrack_h
#define MMemTrack_h

/////////////////////////////////////
#include "MStdLib.h"

/////////////////////////////////////////////////////////
bool MMemTrackAdd(void *memory,int blocksize,bool isarray,const char *filename,int lineno);
bool MMemTrackDel(void *memory,bool isarray,const char *filename,int lineno);
void MMemTrackPrint(void);
bool MMemTrackPrintDebug(void);
bool MMemTrackPrint(void *block);
bool MMemTrackIsValid(void);
int MMemTrackGetBlockCount(void);
unsigned int MMemTrackGetMemorySize(void);

//////////////////////////////////////////////////////////
#ifndef MMemTrackNew
#define MMemTrackNew(PTR)					MMemTrackAdd(PTR,sizeof(*PTR),false,__FILE__,__LINE__)
#endif // MMemTrackNew

//////////////////////////////////////////////////////////
#ifndef MMemTrackNewArray
#define MMemTrackNewArray(PTR,N)			MMemTrackAdd(PTR,N*sizeof(*PTR),true,__FILE__,__LINE__)
#endif // MMemTrackNewArray

//////////////////////////////////////////////////////////
#ifndef MMemTrackFree
#define MMemTrackDelete(PTR)				MMemTrackDel(PTR,false,__FILE__,__LINE__)
#endif // MMemTrackFree

//////////////////////////////////////////////////////////
#ifndef MMemTrackFreeArray
#define MMemTrackDeleteArray(PTR)			MMemTrackDel(PTR,true,__FILE__,__LINE__)
#endif // MMemTrackFreeArray

#endif // MMemTrack_h

