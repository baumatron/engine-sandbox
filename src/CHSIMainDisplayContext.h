#ifndef CHSIMAINDISPLAYCONTEXT_H
#define CHSIMAINDISPLAYCONTEXT_H

#include "math_main.h"
#include "CCamera.h"
#include "CGuiSubsystem.h"
#include "CCalModel.h"

class CHSIMainDisplayContext
{
public:
	CHSIMainDisplayContext();
	~CHSIMainDisplayContext();

	void Initialize();
	void SetupMainDisplayWindow();
	void Think(void);

	enum ViewTiers {galactic, system, colony, ship};
	void ZoomOut();
	void ZoomIn();
	void CycleForwardInTier();
	void CycleBackwardInTier();

	void SetViewTier(ViewTiers tier);
	void SetActivePlanet(int planet);
	void SetActiveSystem(int system);

	void UpdateCameraTracking();


//	GuiWindowID mainWindowID;
	CCamera camera;
	CCalModel* skybox;

	ViewTiers viewTier;
	int activePlanet;
	int activeSystem;
	int activeShip;
};

#endif