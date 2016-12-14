#ifndef CTOKEN_H
#define CTOKEN_H

#include "m_misc.h"
#include "CRouterEvent.h"
#include <string>
#include <vector>
using namespace std;

class CToken 
{
public:
	CToken():id(0), section_id(0), p_data(0), type(section), label("unnamed"){}

	enum TokenTypes {section, function, data};

	unsigned int id;
	unsigned int section_id;
	TokenTypes type;
	string label;
	M_DataTypes dataType;
	void* p_data; // pointer to the data referenced only by a data token type
};


#endif
