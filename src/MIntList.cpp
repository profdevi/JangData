//v2.8 copyright Comine.com 20150813R1159
#include "MStdLib.h"
#include "MIntList.h"


//********************************************************************
//** MIntList
//********************************************************************
void MIntList::ClearObject(void)
	{
	mFirstNode=NULL;
	mLastNode=NULL;
	mCurrent=NULL;
	mCount=0;
	}


//////////////////////////////////////////////////////////////
MIntList::MIntList(void)
	{  ClearObject();  }


//////////////////////////////////////////////////////////////
MIntList::MIntList(bool create)
	{
	ClearObject();
	if(create==true && Create()==false)
		{
		return;
		}
	}


//////////////////////////////////////////////////////////////
MIntList::~MIntList(void)
	{  Destroy();  }


//////////////////////////////////////////////////////////////
bool MIntList::Create(void)
	{
	Destroy();
	return true;
	}


/////////////////////////////////////////////////////////////
bool MIntList::Destroy(void)
	{
	// Traverse and release all the data
	GIntListNode *pnode;
	for(pnode=mFirstNode;pnode!=NULL; )
		{	
		GIntListNode *tmp=pnode->Next;  
		delete pnode;
		pnode=tmp;
		}

	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////////////
bool MIntList::Add(int value)			// Add to the end of the list
	{
	GIntListNode *newnode;
	newnode=new(std::nothrow) GIntListNode;
	if(newnode==NULL)
		{
		return false;
		}

	newnode->Data=value;
	newnode->Next=NULL;

	// Bump up count
	mCount=mCount+1;

	if(mFirstNode==NULL)
		{
		mCurrent=mFirstNode=mLastNode=newnode;
		return true;
		}

	mLastNode->Next=newnode;
	mLastNode=newnode;

	// Update mCurrent if at end to last node
	if(mCurrent==NULL)
		{
		mCurrent=mLastNode;
		}
		
	return true;
	}


//////////////////////////////////////////////////////////
bool MIntList::Add(MIntList &list)
	{
	int count=list.GetCount();

	if(list.ReadReset()==false)
		{
		return false;
		}

	int i;
	for(i=0;i<count;++i)
		{
		int val;
		if(list.Read(val)==false)
			{
			return false;
			}

		// Add list item
		if(Add(val)==false)
			{
			return false;
			}
		}
	
	return true;
	}

////////////////////////////////////////////////////////////
bool MIntList::ReadReset(void)  // Will Reset to the first item again
	{
	mCurrent=mFirstNode;
	return true;
	}


////////////////////////////////////////////////////////////
bool MIntList::Read(int &val)  
	{
	if(mCurrent==NULL) { return false; }
	val=mCurrent->Data;
	mCurrent=mCurrent->Next;
	return true;
	}


/////////////////////////////////////////////////////////////
bool MIntList::IsMember(int val)	// Check if val is member
	{
	// Search through all items
	GIntListNode *p;
	for(p=mFirstNode;p!=NULL;p=p->Next)
		{
		if(val==p->Data) { return true; }
		}

	return false;
	}


/////////////////////////////////////////////////////////
int MIntList::GetCount(void)
	{
	return mCount;
	}


////////////////////////////////////////////////////////
bool MIntList::Print(void)
	{
	MStdPrintf("MIntList : ");
	GIntListNode *p;
	for(p=mFirstNode;p!=NULL;p=p->Next)
		{
		MStdPrintf("%d ",p->Data);
		}

	MStdPrintf("\n");
	return true;
	}


