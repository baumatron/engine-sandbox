#ifndef M_MISC_H
#define M_MISC_H

#include <string>
using namespace std;

extern string newl;


// timing functions
float M_TimeFloat(); // get time passed in program in seconds

enum M_DataTypes { t_bool, t_char, t_uchar, t_short, t_ushort, t_long, t_ulong, t_float, t_double, t_string };

M_DataTypes M_DetermineType(string data);
 
string M_ftoa(double number); // M_ntoa functions return a string class object 
string M_itoa(long number); // with a character representation of its value
string M_btoa(bool number);
string M_itoah(unsigned short number, unsigned short placeCount, bool prefix = true); // hexadecimal output placecount = 4 for a word, 2 for a char
string M_Uppercase(string rhs);

bool M_atob(string rhs);
short M_atoh(string rhs);

bool M_FileExists(string filename);

class FunctionNode
{
public:
	FunctionNode():next(0), handler(0){}
	~FunctionNode(){}
	FunctionNode* next;
	void (* handler) (void);
};

#endif 