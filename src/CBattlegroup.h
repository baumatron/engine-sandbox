#pragma once

#include "CShip.h"

class CBattlegroup
{
public:
	CBattlegroup() {}

	CBattlegroup(int systemID, int composition[32]);

	 

	CShip *m_ships[32];
/*	class 1: 1 point
	class 2: 2 points
	class 3: 4 points
	class 4: 8 points
	class 5: 16 points
	32 points to spend*/

	//Cleader *captain;

	int m_speed;	//speed of the slowest ship
	bool m_reserve;
	
	struct {
		int systemID;
		int planetID;
	} m_locationID;

private:
};