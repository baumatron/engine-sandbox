/* AUTHOR: Brian Baumhover
 *   DATE: 2006.01.15
 *
 */

#include "CBaconDisplayConsole.h"

#include <iostream>
using namespace std;



CBaconDisplayConsole::CBaconDisplayConsole()
{
}
CBaconDisplayConsole::~CBaconDisplayConsole()
{
}

void CBaconDisplayConsole::Initialize()
{
}
void CBaconDisplayConsole::Shutdown()
{
}
void CBaconDisplayConsole::Render()
{
}
void CBaconDisplayConsole::ScrollText(int lines)
{
}
void CBaconDisplayConsole::Message(string text)
{
	cout << text << '\n';
}