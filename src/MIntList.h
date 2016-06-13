//v2.8 copyright Comine.com 20150813R1159
#ifndef MIntList_h
#define MIntList_h

///////////////////////////////////////////////////
class MIntList
	{
	struct GIntListNode
		{
		int Data;
		GIntListNode *Next;
		};

	/////////////////////////////////////////////
	GIntListNode *mFirstNode;
	GIntListNode *mLastNode;
	GIntListNode *mCurrent;				// Current Read Item
	int mCount;							// # of items in linked list

	//////////////////////////////////////////////
	void ClearObject(void);

	//////////////////////////////////////////////
	public:
	MIntList(void);
	MIntList(bool create);
	~MIntList(void);
	bool Create(void);
	bool Destroy(void);
	bool Add(int data);					// Add to the end of the list
	bool Add(MIntList &list);			// Add a fresh list(resets the reader position)
	bool ReadReset(void);				// Will Reset to the first item again
	bool Read(int &val);				// Read an item
	bool IsMember(int val);				// Check if val is a member
	int GetCount(void);					// Get Count of items
	bool Print(void);					// Print out the integers in list
	};

#endif // MIntList_h

