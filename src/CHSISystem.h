#ifndef CHSISYSTEM_H
#define CHSISYSTEM_H

#include "math_main.h"
#include "CCalModel.h"
#include "CVideoSubsystem.h"
#include "CHSIPlanet.h"
#include <vector>
using namespace std;

class CHSISystem
{
public:
	void Draw(CCamera& camera, CHSIMainDisplayContext& context, bool selected);

	void Think();

	vector<CHSIPlanet> planets;
	// about planets
	//	assume that planet 0 is the star, and the center of a system
	//  all other planets are normal planets, up to 6 planets in a system
};

#endif