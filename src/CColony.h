#pragma once

#include "CRegion.h"

class CColony
{
public:
	CColony(){}
	virtual ~CColony(){}

	void NextTurn(){}	//pass the turn
	//post: all turn sensitive variables are iterated to next turn

	void Display(){} //display all colony info

	CRegion *m_regions[12];
	int m_mineralProduction;
	int m_energyProduction;
	int m_energyUsage;
	int m_foodProduction;
	int m_foodUsage;
	int m_researchProduction;
	int m_stardockProduction;
	int m_orbitalProduction;
	int m_regionNumber;

	struct {
		float percent;
		bool locked;
	} m_infrastructure;

	struct {
		float percent;
		bool locked;
	} m_research;

	struct {
		float percent;
		bool locked;
	} m_stardock;

	struct {
		float percent;
		bool locked;
	} m_orbital;
	
	struct {
		int system;
		int planet;
	} m_locationID;

};