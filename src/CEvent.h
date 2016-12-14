#ifndef CEVENT_H
#define CEVENT_H

#include "CGenericType.h"
#include <queue>
using namespace std;

#define EVENT_SIZE 32


class CEventArguments
{
public:
	bool MatchesFormat(M_DataTypes args0)
	{
		if(m_argList.size() < 1)
			return false;
		if(m_argList[0].GetType() == args0)
			return true;
		else
			return false;
	}
	bool MatchesFormat(M_DataTypes args0, M_DataTypes args1)
	{
		if(m_argList.size() < 2)
			return false;
		if(m_argList[0].GetType() == args0)
			if(m_argList[1].GetType() == args1)
				return true;

		return false;
	}
	bool MatchesFormat(M_DataTypes args0, M_DataTypes args1, M_DataTypes args2)
	{
		if(m_argList.size() < 3)
			return false;
		if(m_argList[0].GetType() == args0)
			if(m_argList[1].GetType() == args1)
				if(m_argList[2].GetType() == args2)
					return true;
		return false;
	}

	vector<CGenericType> m_argList;
	//queue<CGenericType> m_list;	
};

class CEvent
{
public:
	CEvent(){m_data[0] = 0;}

	char GetChar()
	{
		return (char)m_data[0];
	}

	short GetShort()
	{
		return (short)m_data[0];
	}

	long GetLong()
	{
		return (long)m_data[0];
	}


	void Shift( unsigned int numWords )
	{
		for( unsigned int i = 0; i < EVENT_SIZE-numWords; i++ )
		{
			m_data[i] = m_data[i+numWords];
		}
	}

	int m_data[EVENT_SIZE]; // event data
	CEventArguments m_arguments;
};





#endif