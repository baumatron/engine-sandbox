/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.22
 *
 */

#include "CBaconDisplayGui.h"
#include "CGuiWindowWidget.h"



CBaconDisplayGui::CBaconDisplayGui()
{
}
CBaconDisplayGui::~CBaconDisplayGui()
{
}

void CBaconDisplayGui::Initialize()
{
	m_window.New(1);
	m_window->Load("data/gui/console.xml");
	m_window->GetWidget<CGuiFieldWidget>("consoletext").contents = "";
}
void CBaconDisplayGui::Shutdown()
{
	if(!m_window.Null())
		m_window->Close();
	m_window.Release();
}
void CBaconDisplayGui::Render()
{
}
void CBaconDisplayGui::ScrollText(int lines)
{
}
void CBaconDisplayGui::Message(string text)
{
	/*for(int i = 0; i < text.length(); i++)
	{
		if(text[i] == '\n')
			m_textBuffer.push_back(string());
		else
		{
			if(!m_textBuffer.size())
				m_textBuffer.push_back(string());
			m_textBuffer.back() += text[i];
		}
	}
	if(!m_window.Null())
	{*/
		/*string & contents =*/ m_window->GetWidget<CGuiFieldWidget>("consoletext").contents += text + '\n';
	/*	contents = "";
		for(deque<string>::reverse_iterator it = m_textBuffer.rbegin(); it != m_textBuffer.rend(); it++)
		{
			contents += *it;
			contents += '\n';
		}
	}*/
}