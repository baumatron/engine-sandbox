/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#ifndef BANESTRINGS_H
#define BANESTRINGS_H

#include <string>
using namespace std;

string M_ftoa(float number); // M_ntoa functions return a string class object 
string M_itoa(long number); // with a character representation of its value
string M_btoa(bool number);
string M_itoah(unsigned short number, unsigned short placeCount, bool prefix = true); // hexadecimal output placecount = 4 for a word, 2 for a char
string M_Uppercase(string rhs);

bool M_atob(string rhs);
short M_atoh(string rhs);

#endif