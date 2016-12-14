
#ifndef CHSIPLANET_H
#define CHSIPLANET_H

#include "CModel.h"
#include "CSmartPointer.h"
#include "CVideoSubsystem.h"
#include "CHSIMainDisplayContext.h"
#include "CGuiSubsystem.h"
#include "colony.h" // prototype code



class CHSIPlanet : public ILookAtMe, ITrackInHud
{
public:
	CHSIPlanet();
	CHSIPlanet(const CHSIPlanet& rhs);
	~CHSIPlanet();

	void Initialize(string modelFilename);
	void Shutdown();

	void FindMaximumRadius();
	float GetRadius(){return m_radius;}
	string GetName(){return m_name;}
	void SetName(const string& name){ m_name = name; }

	void Draw(CCamera& camera, CHSIMainDisplayContext& context, bool systemSelected, bool planetSelected);
	void DrawSystemViewAccessories(CCamera& camera, CHSIMainDisplayContext& context, bool selected);

	/*void CameraFocusOn(CCamera& camera);*/
	matrix4x4 GetWorldMatrix(void);
	void SetWorldMatrix(const matrix4x4& matrix);

	v3d GetHudPosition();
	
	void Think();

	CHSIPlanet operator= (const CHSIPlanet& rhs);

	CSmartPointer<CModel> GetModel();
	bool m_lensflare;

	VideoResourceID m_lensFlareGraphic;


	struct{
		int systemNumber;
		int planetNumber;
	}	m_planetID;
	colony m_colony;
	float m_nextTurn;
	void SyncColonyData();

private:
	float m_radius;
	string m_name;

	matrix4x4 m_worldMatrix;

	GuiWindowSpecID m_systemViewWindowSpecID;
	GuiWindowSpecID m_colonyViewWindowSpecID;
	GuiWindowID m_systemViewWindowID;
	GuiWindowID m_colonyViewWindowID;
	CGuiWindowWidget* m_systemViewWindow;
	CGuiWindowWidget* m_colonyViewWindow;

	CSmartPointer<CModel> m_spModel;

	void SetupGuiWindows();
	void CopyFrom(const CHSIPlanet& rhs);
};

#endif