#ifndef CHSIGALAXY_H
#define CHSIGALAXY_H

#include "CCalModel.h"
#include "CVideoSubsystem.h"
#include "CShip.h"
#include "CHSISystem.h"
#include "CHSIMainDisplayContext.h"
#include "colony.h"
#include <vector>
#include <string>
using namespace std;

class CHSIGalaxy
{
public:
	void Draw(CCamera& camera, CHSIMainDisplayContext& context);

	void Think();

	void ReadFromFile(string filename);
	void GenerateRandomly();

	matrix4x4 GetPlanetLocation(int system, int planet);
	colony& GetPlanetColony(int system, int planet);
	CHSIPlanet& GetPlanet(int system, int planet);

	vector<CHSISystem> systems;

	vector<CShip> ships;
};

#endif