/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#ifndef CBACON_H
#define CBACON_H

#include <string>
#include <deque>
#include <vector>
using namespace std;

class IBaconDisplay
{
public:
	virtual ~IBaconDisplay(){}
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void Render() = 0; // if this uses the gui, this function doesn't really do anything i guess
	virtual void ScrollText(int lines) = 0; // lines = positive or negative value
	virtual void Message(string text) = 0; // called every time a new line is written to the console, allowing for messages to be displayed in a timed out overlay or something
};

#define NUM_TEXT_BUFFER_LINES 256


class CBacon
{
public:
	CBacon();
	~CBacon();

	void Initialize();
	void AddDisplay(IBaconDisplay* display);
	void Shutdown();

	void ProcessInput(string command);

	deque<string>& GetText();

	// basic types
	CBacon& operator<<(string rhs);
	CBacon& operator<<(const char* rhs);	
	CBacon& operator<<(char rhs);
	CBacon& operator<<(unsigned char rhs);
	CBacon& operator<<(float rhs);
	CBacon& operator<<(int rhs);
	CBacon& operator<<(long int rhs);
	CBacon& operator<<(unsigned long int rhs);
	CBacon& operator<<(short int rhs);
	CBacon& operator<<(unsigned short int rhs);
	CBacon& operator<<(bool rhs);
private:

	bool m_initialized;
	void AppendText(string text);

	vector<IBaconDisplay*> m_pDisplays;
	deque<string> m_text; // each element is a line
};

namespace bacon
{
	extern CBacon cout;
}

#endif