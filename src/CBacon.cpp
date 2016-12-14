/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#include "CBacon.h"
#include "CPythonSubsystem.h"
#include "BaneStrings.h"

namespace bacon
{
	CBacon cout;
}

CBacon::CBacon():
	m_initialized(false)
{
	m_text.push_front(string());
}
CBacon::~CBacon()
{
}

void CBacon::Initialize()
{
	m_initialized = true;
	for(vector<IBaconDisplay*>::iterator display = m_pDisplays.begin(); display != m_pDisplays.end(); display++)
	{
		(*display)->Initialize();
	}
}
void CBacon::AddDisplay(IBaconDisplay* display)
{
	m_pDisplays.push_back(display);
	if(m_initialized)
		display->Initialize();
}
void CBacon::Shutdown()
{
	m_initialized = false;
	for(vector<IBaconDisplay*>::iterator display = m_pDisplays.begin(); display != m_pDisplays.end(); display++)
	{
		(*display)->Shutdown();
	}
}

void CBacon::ProcessInput(string command)
{
	CPythonSubsystem::Instance()->ExecuteString(command);
}

deque<string>& CBacon::GetText()
{
	return m_text;
}

// basic types
CBacon& CBacon::operator << (string rhs)
{
	AppendText(rhs);
	return *this;
}

CBacon& CBacon::operator << (const char* rhs)
{
	AppendText(string(rhs));
	return *this;
}	
CBacon& CBacon::operator << (char rhs)
{
	char cstring[2] = {rhs, '\0'};
	AppendText(cstring);
	return *this;
}
CBacon& CBacon::operator << (unsigned char rhs)
{
	char cstring[2] = {rhs, '\0'};
	AppendText(cstring);
	return *this;
}
CBacon& CBacon::operator << (float rhs)
{
	AppendText(M_ftoa(rhs));
	return *this;
}
CBacon& CBacon::operator << (int rhs)
{
	AppendText(M_itoa(rhs));
	return *this;
}
CBacon& CBacon::operator << (long int rhs)
{
	AppendText(M_itoa(rhs));
	return *this;
}
CBacon& CBacon::operator << (unsigned long int rhs)
{
	AppendText(M_itoa(rhs));
	return *this;
}
CBacon& CBacon::operator << (short int rhs)
{
	AppendText(M_itoa(rhs));
	return *this;
}
CBacon& CBacon::operator << (unsigned short int rhs)
{
	AppendText(M_itoa(rhs));
	return *this;
}
CBacon& CBacon::operator << (bool rhs)
{
	AppendText(M_btoa(rhs));
	return *this;
}

void CBacon::AppendText(string text)
{
	for(int i = 0; i < text.size(); i++)
	{
		if(text[i] == '\n')
		{
			for(vector<IBaconDisplay*>::iterator display = m_pDisplays.begin(); display != m_pDisplays.end(); display++)
			{
				// let the display know that we just printed a line
				(*display)->Message(m_text[0]);
			}

			// push and pop the text buffer
			m_text.push_front(string());
            if(m_text.size() > NUM_TEXT_BUFFER_LINES)
				m_text.pop_back();
		}
		else
		{
			// add the character to the buffer and line that is used for the display
			m_text[0] += text[i];
		}
	}
}