/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#pragma once

#include "CBacon.h"
#include "CGuiWindowWidget.h"
#include <deque>
using namespace std;
class CBaconDisplayGui: public IBaconDisplay
{
public:
	CBaconDisplayGui();
	~CBaconDisplayGui();

	virtual void Initialize();
	virtual void Shutdown();
	virtual void Render(); // if this uses the gui, this function doesn't really do anything i guess
	virtual void ScrollText(int lines); // lines = positive or negative value
	virtual void Message(string text); // called every time a new line is written to the console, allowing for messages to be displayed in a timed out overlay or something

	deque<string> m_textBuffer;
	CSmartPointer<CGuiWindowWidget> m_window;
};

