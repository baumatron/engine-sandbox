#ifndef ITRACKINHUD_H
#define ITRACKINHUD_H

#include "math_main.h"
#include "CCamera.h"

class ITrackInHud
{
public:
	virtual v3d GetHudPosition() = 0;
};

#endif