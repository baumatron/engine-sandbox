/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#ifndef CBACONDISPLAYCONSOLE_H
#define CBACONDISPLAYCONSOLE_H

#include "CBacon.h"

class CBaconDisplayConsole: public IBaconDisplay
{
public:
	CBaconDisplayConsole();
	~CBaconDisplayConsole();

	virtual void Initialize();
	virtual void Shutdown();
	virtual void Render(); // if this uses the gui, this function doesn't really do anything i guess
	virtual void ScrollText(int lines); // lines = positive or negative value
	virtual void Message(string text); // called every time a new line is written to the console, allowing for messages to be displayed in a timed out overlay or something
};


#endif