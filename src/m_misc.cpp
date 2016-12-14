#define SYS_DEFS_INCLUDE_WIN32 // for SYSW_TimeFloat
#include "m_misc.h"
#include "sys_win.h"
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;
string newl("\n");

// Return program time in seconds
float M_TimeFloat()
{
	// cascade to windows function for this, as it uses windows performance
	// counter and multimedia timer functions
	return((float)SYSW_TimeFloat());
}

template <class T>
std::string to_string(T t, std::ios_base & (*f)(std::ios_base&))
{
   std::ostringstream oss;
   oss << f << t;
   return oss.str();
}

M_DataTypes M_DetermineType(string data)
{
	bool d(true); // double
	bool l(true);  // long
	bool b(false); // bool
	for(int i = 0; i < data.length(); i++)
	{
		if((data[i] < '0') || (data[i] > '9'))
		{
			l = false;
			if(data[i] != '.')
				d = false;
		}
	}
	if(!d && !l) // must be a string or bool
	{
		string temp;
		temp = M_Uppercase(data);
		if( temp == "FALSE" || temp == "TRUE" )
			b = true;
	}
	if(l)
		return t_long;
	else if(d)
		return t_double;
	else if(b)
		return t_bool;
	else
		return t_string;
}

string M_ftoa(double number)
{
	return to_string(number, std::dec);
}

string M_itoa(long number)
{
	string product;
	char baseChar = '0';
	if(number < 0) // its negative
	{
		product += '-';
		number = -number; // avoid confusion when converting
	}
	bool haveFoundNonzeroDigit(false);
	double power(.0001);
	for(long divisor = 100000; divisor != 1; divisor/=10, power*=10)
	{
		if(!haveFoundNonzeroDigit)
		{
			if( ( (long) ((number % divisor) * power) ) != 0 ) // gets rid of numbers to left of current digit and allows truncation of digits to the right
				haveFoundNonzeroDigit = true;
		}
		if(haveFoundNonzeroDigit)
		{
			product += baseChar + (char)(long)((number % divisor) * power);
		}

	}
	if(product == "")
		product = "0";
	return product;
}

string M_btoa(bool number)
{
	string product;
	if(number)
		return string("true");
	else
		return string("false");
}

string M_itoah(unsigned short number, unsigned short placeCount, bool prefix) // hexadecimal output placecount = 4 for a word, 2 for a char
{
	string product;
	string backwardsProduct;
	char digit;
	char baseChar = '0';
	char hexBaseChar = 'A';

/*	if(number < 0) // its negative
	{
		product += '-';
		number = -number; // avoid confusion when converting
	}*/

	do
	{
		digit = number%16;
		backwardsProduct += digit < 10 ? baseChar + digit : hexBaseChar + (digit - 10);
		number>>=4;
	} while (number > 0);

	short length = backwardsProduct.length(); // length() changes during the for loop...
	for(int j = 0; j < (placeCount - length); j++)
	{
		backwardsProduct += '0';
	}

	if(prefix)
		product+= "0x";

	for(int i = backwardsProduct.length()-1; i >= 0; i--)
	{
		product += backwardsProduct[i];
	}

	return product;
}
string M_Uppercase(string rhs) // returns rhs converted to uppercase
{
	for(unsigned int i = 0; i < rhs.length(); i++)
	{
		if( (rhs[i] >= 'a') && (rhs[i] <= 'z') )
		{
			rhs[i] -=  32;
		}
	}
	return rhs;
}


bool M_atob(string rhs)
{
	rhs = M_Uppercase(rhs);
	if( (rhs[0] == 'T') || (rhs[0] == '1') )
		return true;
	else
		return false;
}
short M_atoh(string rhs)
{
	rhs = M_Uppercase(rhs);
	short totalValue(0);
	short digitValue(0);
	for(unsigned int i = 0; i < rhs.length(); i++) // convert it to base 10
	{
		if( (rhs[i] >= '0') && (rhs[i] <= '9') )
			digitValue = rhs[i] - '0';
		else
			digitValue = 10 + rhs[i] - 'A';
		totalValue += digitValue*(short)pow(16, (rhs.length()-1)-i);
	}
	return totalValue;
}

bool M_FileExists(string filename)
{
	ifstream fin;
	fin.open(filename.c_str(), ifstream::in);
	fin.close();
	if(fin.fail())
		return false;
	else
		return true;
}
