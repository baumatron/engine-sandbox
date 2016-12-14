#ifndef CTYPEABSTRACTIONLAYER_H
#define CTYPEABSTRACTIONLAYER_H

#include <string>
using namespace std;
#include "m_misc.h"

class CGenericType
{
public:
	CGenericType(M_DataTypes type):m_type(type){}

	M_DataTypes GetType() const;

	void SetValue(const bool& value);
	void SetValue(const char& value);
	void SetValue(const unsigned char& value);
	void SetValue(const short& value);
	void SetValue(const unsigned short& value);
	void SetValue(const long& value);
	void SetValue(const unsigned long& value);
	void SetValue(const float& value);
	void SetValue(const double& value);
	void SetValue(const string& value);
	
	void GetValue(bool& result) const;
	void GetValue(char& result) const;
	void GetValue(unsigned char& result) const;
	void GetValue(short& result) const;
	void GetValue(unsigned short& result) const;
	void GetValue(long& result) const;
	void GetValue(unsigned long& result) const;
	void GetValue(float& result) const;
	void GetValue(double& result) const;
	void GetValue(string& result) const;

	static CGenericType FromString(string data);

private:
	M_DataTypes m_type;
	
	union
	{
		bool m_bool;
		char m_char;
		unsigned char m_uchar;
		short m_short;
		unsigned short m_ushort;
		long m_long;
		unsigned long m_ulong;
		float m_float;
		double m_double;
	} m_data;
	string m_string;
};



#endif