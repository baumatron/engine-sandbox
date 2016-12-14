#ifndef CSMARTPOINTER_H
#define CSMARTPOINTER_H

#include "con_main.h"

#include <map>
#include <string>
using namespace std;

template <class T>
class CSmartPointer
{
public:
	CSmartPointer():
			m_pointer(0),
			m_array(false),
			m_elementCount(0)
		{
		}
	CSmartPointer(const CSmartPointer& rhs):
			m_pointer(rhs.m_pointer),
			m_array(rhs.m_array),
			m_elementCount(rhs.m_elementCount)
		{
			CSmartPointerDatabase::Instance()->IncreaseReferenceCount(m_pointer);			
		}
	CSmartPointer(T* const pointer, bool pointsToArray):
			m_pointer(pointer),
			m_array(pointsToArray),
			m_elementCount(0) // UNKNOWN!!!!!!!!!!
		{
			CSmartPointerDatabase::Instance()->IncreaseReferenceCount(m_pointer);			
		}
	~CSmartPointer()
		{
			CSmartPointerDatabase::Instance()->DecreaseReferenceCount(m_pointer, m_array);
		}

/*	CSmartPointer operator=(T* const pointer)
		{
			//ccout << "CSmartPointer operator=(T* const pointer): Don't use this shit, silly!\n";
			CSmartPointerDatabase::Instance()->DecreaseReferenceCount(m_pointer);
			m_pointer = pointer;
			CSmartPointerDatabase::Instance()->IncreaseReferenceCount(m_pointer);
			return *this;
		}*/
	CSmartPointer operator=(const CSmartPointer& rhs)
		{
			CSmartPointerDatabase::Instance()->DecreaseReferenceCount(m_pointer, m_array);
			m_pointer = rhs.m_pointer;
			m_array = rhs.m_array;
			m_elementCount = rhs.m_elementCount;
			CSmartPointerDatabase::Instance()->IncreaseReferenceCount(m_pointer);
			return *this;
		}

	T& operator [] (int index)
		{
			return m_pointer[index];
		}
	const T& operator [] (int index) const
		{
			return m_pointer[index];
		}	

	T* GetPointer() const
		{
			return m_pointer;
		}

	int GetReferenceCount() const
		{
			return CSmartPointerDatabase::Instance()->GetReferenceCount(m_pointer);
		}
	bool Null() const
		{
			return (m_pointer == 0);
		}
	void Release()
		{
			CSmartPointerDatabase::Instance()->DecreaseReferenceCount(m_pointer, m_array);
			m_pointer = 0;
			m_array = false;
			m_elementCount = 0;
		}
	CSmartPointer<T> New(const unsigned int elementCount)
	{
		if(m_pointer)
			Release();

		m_elementCount = elementCount;
		m_array = true;
		m_pointer = new T[elementCount];
		CSmartPointerDatabase::Instance()->IncreaseReferenceCount(m_pointer);			
		return *this;
	}
	void MemCopy(const CSmartPointer<T> rhs)
	{
		if(m_elementCount != rhs.m_elementCount)
		{
			// sizes don't match!!!
			ccout << "MemCopy(...) error, size mismatch\n";
			return;
		}

		memcpy(m_pointer, rhs.m_pointer, m_elementCount*sizeof(T));
	}
	unsigned int GetSize() const
	{
		return m_elementCount*sizeof(T);
	}

	bool operator==(const CSmartPointer& rhs) const
		{
			return (m_pointer == rhs.m_pointer) && (m_elementCount == rhs.m_elementCount);
		}
	T& operator * ()
		{
			return *m_pointer;
		}
	const T& operator * () const
		{
			return *m_pointer;
		}
	T* operator->()
		{
			return m_pointer;
		}
public:
	T* m_pointer;
	bool m_array;
	unsigned int m_elementCount;
};

class CSmartPointerDatabase
{
public:
	CSmartPointerDatabase();
	~CSmartPointerDatabase();

	void IncreaseReferenceCount(void* pointer);
	void DecreaseReferenceCount(void* pointer, bool pointsToArray);

	int GetReferenceCount(void * pointer);

	static CSmartPointerDatabase* Instance();
private:
	map<void*, int> m_referenceMap;

	static CSmartPointerDatabase* instance;
};

template <class T>
class CSharedResource
{
public:
	CSharedResource();
	~CSharedResource();


private:
	int m_referenceCount;
	T* data;
};

template <class T>
class CSharedResourcePool
{
public:
	CSharedResourcePool()
	{
	}
	~CSharedResourcePool()
	{
		Purge();
	}

	void AddNewResource(string name, CSmartPointer<T> data)
	{
		m_resourceMap[name] = data;
		ccout << "adding new resource: " << name << newl;
	}
	CSmartPointer<T> GetSharedResource(string name)
	{
		return m_resourceMap[name];
	}
	
	void Purge() // frees all memory with only one reference
	{
		for(map<string, CSmartPointer<T> >::iterator it = m_resourceMap.begin(); it != m_resourceMap.end(); it++)
		{
			if(it->second.GetReferenceCount() == 1)
			{
				// this is the only reference, so kill that shit off
				ccout << "Deleting resource: " << it->first << newl;
				m_resourceMap.erase(it); // the memory is automatically deleted by the smart pointer
				it = m_resourceMap.begin(); // restart the loop to be safe
			}
			else
			{
				ccout << "Warning! Purge() called but the resource, " << it->first << ", is still in use!\n";
			}
		}
	}

	void PrintMap()
	{
		ccout << "\nCSharedResourcePool: resource map:\n";
		for(map<string, CSmartPointer<T> >::iterator it = m_resourceMap.begin(); it != m_resourceMap.end(); it++)
		{
			ccout << "resource: " << it->first << " at address " << M_itoah((unsigned short)it->second.GetPointer(), 8, true) << "\n";
		}
	}
private:
	map<string, CSmartPointer<T> > m_resourceMap; // resource data
};

#endif