
#include "CSmartPointer.h"

#include <iostream>
using namespace std;

// TODO: FIX PROBLEM THAT WOULD OCCUR IF SMART POINTER POINTS TO AN ARRAY,
//       RIGHT NOW, ONLY delete IS CALLED, NOT delete []

CSmartPointerDatabase* CSmartPointerDatabase::instance = 0;


CSmartPointerDatabase::CSmartPointerDatabase()
{
}

CSmartPointerDatabase::~CSmartPointerDatabase()
{
	if(instance)
	{
		delete instance;
		instance = 0;
	}
}

void CSmartPointerDatabase::IncreaseReferenceCount(void* pointer)
{
	if(pointer == 0)
		return;

	map<void*, int>::iterator referenceIt = m_referenceMap.find(pointer);

	if(referenceIt == m_referenceMap.end())
	{// no reference exists, add it
		m_referenceMap[pointer] = 1;
	}
	else
	{
		m_referenceMap[pointer]++;
	}
}
void CSmartPointerDatabase::DecreaseReferenceCount(void* pointer, bool pointsToArray)
{
	if(pointer == 0)
		return;

	map<void*, int>::iterator referenceIt = m_referenceMap.find(pointer);

	if(referenceIt == m_referenceMap.end())
	{// no reference exists, add it
		cout << "Error! CSmartPointerDatabase::DecreaseReferenceCount tried to remove a reference to a non-existent resource!\n";
	}
	else
	{
		if(--m_referenceMap[pointer] == 0)
		{ // no references are remaining, so free the memory
			
			if(pointsToArray)
			{
				ccout << "Deleting memory(array)...\n";
				delete [] pointer;
			}
			else
			{
				ccout << "Deleting memory...\n";
				delete pointer;
			}
			m_referenceMap.erase(referenceIt);
		}
	}
}

int CSmartPointerDatabase::GetReferenceCount(void* pointer)
{
	if(pointer == 0)
		return 0;

	map<void*, int>::iterator referenceIt = m_referenceMap.find(pointer);

	if(referenceIt == m_referenceMap.end())
	{// no reference exists, there are none
		return 0;		
	}
	else
	{
		return referenceIt->second;
	}
}


CSmartPointerDatabase* CSmartPointerDatabase::Instance()
{
	if(!instance)
	{
		instance = new CSmartPointerDatabase;
	}
	return instance;
}

