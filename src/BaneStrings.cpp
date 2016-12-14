/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#include "BaneStrings.h"

#include <math.h>
#include <iostream>
#include <sstream>
using namespace std;

template <class T>
std::string to_string(T t, std::ios_base & (*f)(std::ios_base&))
{
   std::ostringstream oss;
   oss << f << t;
   return oss.str();
}

string M_ftoa(float number)
{
	return to_string(number, std::dec);
}

string M_itoa(long number)
{
	return to_string(number, std::dec);
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
	return to_string(number, std::hex); // does this work?
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