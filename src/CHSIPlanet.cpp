
#include "CHSISubsystem.h"
#include "CHSIPlanet.h"
//#include "CMilkShapeModel.h"
#include "sys_main.h"
#include "CFontManager.h"
#include "CCardinalSpline.h"
#include "CVideoSubsystem.h"

CCardinalSpline path;

CHSIPlanet::CHSIPlanet()
{
	m_name = "Unnamed Planet";
	m_lensflare = false;

	m_systemViewWindowSpecID = -1;
	m_colonyViewWindowSpecID = -1;
	m_systemViewWindowID = -1;
	m_colonyViewWindowID = -1;
	m_systemViewWindow = 0;
	m_colonyViewWindow = 0;
	m_nextTurn = 0;
}
CHSIPlanet::CHSIPlanet(const CHSIPlanet& rhs)
{
	m_systemViewWindowSpecID = -1;
	m_colonyViewWindowSpecID = -1;
	m_systemViewWindowID = -1;
	m_colonyViewWindowID = -1;
	m_systemViewWindow = 0;
	m_colonyViewWindow = 0;
	CopyFrom(rhs);
	m_nextTurn = 0;
}
CHSIPlanet::~CHSIPlanet()
{
	if(m_systemViewWindowID != -1)
	{
		Gui.WM.DeleteWindowInstance(m_systemViewWindowID);
	}
	if(m_colonyViewWindowID != -1)
	{
		Gui.WM.DeleteWindowInstance(m_colonyViewWindowID);
	}
//	CCalCoreModelRepository::GetInstance()->ReleaseModelInstance(m_model);
}

void CHSIPlanet::Initialize(string modelFilename)
{
	m_spModel = CModelRepository::Instance()->GetModelInstance(modelFilename);
	FindMaximumRadius();
	SetupGuiWindows();
	SyncColonyData();
}


void CHSIPlanet::FindMaximumRadius()
{
	if(!m_spModel.Null())
		m_radius = (*m_spModel).FindMaximumRadius();
}

void CHSIPlanet::SetupGuiWindows()
{
	m_systemViewWindowSpecID = Gui.WM.ReadWindowSpec("data/gui/templates/colonyoverview.xml");
	m_systemViewWindowID = Gui.WM.MakeWindowInstance( m_systemViewWindowSpecID );
	if(Gui.WM.WindowIDIsValid(m_systemViewWindowID))
	{
		m_systemViewWindow = Gui.WM.GetWindowInstance(m_systemViewWindowID);
	}

	m_colonyViewWindowSpecID = Gui.WM.ReadWindowSpec("data/gui/templates/colony.xml");
	m_colonyViewWindowID = Gui.WM.MakeWindowInstance( m_colonyViewWindowSpecID );
	if(Gui.WM.WindowIDIsValid(m_colonyViewWindowID))
	{
		m_colonyViewWindow = Gui.WM.GetWindowInstance(m_colonyViewWindowID);
	}

	if(m_systemViewWindow)
	{
		m_systemViewWindow->visible = false;
		m_systemViewWindow->StartTracking(*this);
	}
	if(m_colonyViewWindow)
		m_colonyViewWindow->visible = false;
}


void CHSIPlanet::Draw(CCamera& camera, CHSIMainDisplayContext& context, bool systemSelected, bool planetSelected)
{
	if(systemSelected)
		if(m_lensflare)
		{
			float flareWidth = m_radius*2 * 38.0f/5.0f;
			Video.DrawLensFlare(m_lensFlareGraphic, flareWidth, m_worldMatrix.getTranslation(), camera);
		}

	switch(context.viewTier)
	{
	case CHSIMainDisplayContext::ship:
	case CHSIMainDisplayContext::colony:
		{
			if(systemSelected)
			{			
				if(planetSelected || this->m_planetID.planetNumber == 0)
				{
					if(m_lensflare)
					{
						glPushAttrib(GL_ENABLE_BIT);
						glDisable(GL_LIGHTING);
						(*m_spModel).Render(camera, m_worldMatrix);
						glPopAttrib();
					}
					else
					{
						(*m_spModel).Render(camera, m_worldMatrix);
					}
				}
			}

			if(planetSelected)
			{
				Video.vcout.setSize(16);
				Video.vcout.setColor(CColor(0.8f, 0.6f, 0.0f));
				Video.vcout.setPos(v3d((float)Video.settings.getSw()/2, (float)Video.settings.getSh()-50));
				Video.vcout << m_name;
				Video.vcout.setSize(12);
				if(m_systemViewWindow)
					m_systemViewWindow->visible = false;
				if(m_colonyViewWindow)
					if(this->m_colony.flagOwnedColony)
						m_colonyViewWindow->visible = true;
			}
			else
			{
				if(m_systemViewWindow)
					m_systemViewWindow->visible = false;
				if(m_colonyViewWindow)
					m_colonyViewWindow->visible = false;
			}
		}
		break;
	case CHSIMainDisplayContext::system:
		{
			if(systemSelected)
			{
				if(this->m_planetID.planetNumber == 0)
				{
					if(m_lensflare)
					{
						glPushAttrib(GL_ENABLE_BIT);
						glDisable(GL_LIGHTING);
						(*m_spModel).Render(camera, m_worldMatrix);
						glPopAttrib();
					}
					else
					{
						(*m_spModel).Render(camera, m_worldMatrix);
					}
				}
				DrawSystemViewAccessories(camera, context, planetSelected);
				if(m_systemViewWindow)
					if(this->m_colony.flagOwnedColony)
						m_systemViewWindow->visible = true;
				if(m_colonyViewWindow)
					m_colonyViewWindow->visible = false;
			}
			else
			{
				if(m_systemViewWindow)
					m_systemViewWindow->visible = false;
				if(m_colonyViewWindow)
					m_colonyViewWindow->visible = false;
			}
		}
		break;
	case CHSIMainDisplayContext::galactic:
		{
			if(m_systemViewWindow)
				m_systemViewWindow->visible = false;
			if(m_colonyViewWindow)
				m_colonyViewWindow->visible = false;

				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();

				if(Video.settings.getSh())
					gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 10000);

				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				glLoadMatrixf((camera.GetModelViewMatrix() * this->m_worldMatrix).matrix);

				glPushAttrib(GL_ENABLE_BIT);
				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
				//glEnable(GL_BLEND); // DISABLED BECAUSE OF BLENDING PROBLEMS /////////////////////////////////////////////////////////////////////
				//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glDisable(GL_BLEND);
			// glBlendFunc( GL_ONE, GL_ONE );

				glColor3f(1.0f, 0.0f, 0.0f);
				glDisable( GL_TEXTURE_2D );

			//	glEnable(GL_BLEND);
			//	glEnable(GL_POINT_SMOOTH);
				glPointSize(2.0f);

				//v3d location = this->m_worldMatrix.getTranslation();
				glBegin(GL_POINTS);
					//glVertex3f(location.x, location.y, location.z);
					glVertex3f(0.0f, 0.0f, 0.0f);
				glEnd();



				glPopAttrib();

				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
		}
		break;
	}

	if(m_colonyViewWindow)
		m_colonyViewWindow->SetUpperLeftCorner(v3d(10.0f, (float)Video.settings.getSh()-20));
}

// From ILookAtMe
matrix4x4 CHSIPlanet::GetWorldMatrix(void)
{
	return this->m_worldMatrix;
}
void CHSIPlanet::SetWorldMatrix(const matrix4x4& matrix)
{
	this->m_worldMatrix = matrix;
}
v3d CHSIPlanet::GetHudPosition()
{
	Video.PushProjection3d();
	glLoadMatrixf(Hsi.m_mainDisplayContext.camera.GetModelViewMatrix().matrix);
	v3d planetPointProjected = Video.ProjectPoint(this->GetWorldMatrix().getTranslation());
	v3d windowOffset = v3d(Video.TopViewportContext().area.getWidth()/2, Video.TopViewportContext().area.getHeight()/2) - v3d(10, 10);
	Video.PopProjection();
	return planetPointProjected + windowOffset;
}


void CHSIPlanet::DrawSystemViewAccessories(CCamera& camera, CHSIMainDisplayContext& context, bool selected)
{
	v3d planetPointProjected;
	float nameLength;
	float nameHeight;
	float nameDescender;
	CRectangle nameBox;

	// label planet
	Video.PushProjection3d();
	glLoadMatrixf((camera.GetTransformation().getInverse()).matrix);
	Video.vcout.setColor(CColor(0.0f, 0.8f, 1.0f));
	planetPointProjected = Video.ProjectPoint(GetWorldMatrix().getTranslation());
	planetPointProjected += v3d(Video.TopViewportContext().area.getWidth()/2, Video.TopViewportContext().area.getHeight()/2);
	v3d textPosition = planetPointProjected + v3d(20,20);
	Video.vcout.setPos(textPosition);
	Video.vcout << this->m_name;
	// now put a box around the label
	nameLength = Video.vcout.getAdvance(m_name);
	nameHeight = Video.vcout.getHeight();
	nameDescender = Video.vcout.getDescender();
	nameBox.leftx = textPosition.x-5;
	nameBox.rightx = textPosition.x+5 + nameLength; //Video.vcout(50,0,-200)
	nameBox.bottomy = textPosition.y + nameDescender;
	nameBox.topy = textPosition.y + nameDescender + nameHeight;
	Video.PopProjection();
	Video.PushProjection2d();
	
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);

	glBegin(GL_LINE_LOOP);
		glColor3f(0.8f, 0.6f, 0.0f);
		glVertex3f(nameBox.getUpperLeft().x, nameBox.getUpperLeft().y, 0.0);
		glVertex3f(nameBox.getUpperRight().x, nameBox.getUpperRight().y, 0.0);
		glVertex3f(nameBox.getLowerRight().x, nameBox.getLowerRight().y, 0.0);
		glVertex3f(nameBox.getLowerLeft().x, nameBox.getLowerLeft().y, 0.0);
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(planetPointProjected.x, planetPointProjected.y, planetPointProjected.z);
		glVertex3f(nameBox.getLowerLeft().x, nameBox.getLowerLeft().y, nameBox.getLowerLeft().z);
	glEnd();
	glPopAttrib();
	Video.PopProjection();

	Video.PushProjection3d();
	glLoadMatrixf((camera.GetTransformation().getInverse() * TranslationMatrix(m_worldMatrix.getTranslation()) * camera.GetTransformation().getTransform()).matrix);
	CRectangle planetBox;
	planetBox.leftx = -m_radius*2;//5000*2;
	planetBox.rightx = m_radius*2;//5000*2;
	planetBox.bottomy = -m_radius*2;//-5000*2;
	planetBox.topy = m_radius*2;//5000*2;


	glDisable(GL_LIGHTING);

	/*glBegin(GL_LINE_LOOP);
		glColor3f(0.8f, 0.6f, 0.0f);
		glVertex3f(planetBox.getUpperLeft().x, planetBox.getUpperLeft().y, planetBox.getUpperLeft().z);
		glVertex3f(planetBox.getUpperRight().x, planetBox.getUpperRight().y, planetBox.getUpperRight().z);
		glVertex3f(planetBox.getLowerRight().x, planetBox.getLowerRight().y, planetBox.getLowerRight().z);
		glVertex3f(planetBox.getLowerLeft().x, planetBox.getLowerLeft().y, planetBox.getLowerLeft().z);
	glEnd();*/
	/*glBegin(GL_LINE_LOOP);
		glColor3f(0.8f, 0.6f, 0.0f);
		glVertex3f(nameBox.getUpperLeft().x, nameBox.getUpperLeft().y, nameBox.getUpperLeft().z);
		glVertex3f(nameBox.getUpperRight().x, nameBox.getUpperRight().y, nameBox.getUpperRight().z);
		glVertex3f(nameBox.getLowerRight().x, nameBox.getLowerRight().y, nameBox.getLowerRight().z);
		glVertex3f(nameBox.getLowerLeft().x, nameBox.getLowerLeft().y, nameBox.getLowerLeft().z);
	glEnd();*/
/*	glBegin(GL_LINES);
		glColor3f(0.8f, 0.6f, 0.0f);
		glVertex3f(nameBox.getLowerLeft().x, nameBox.getLowerLeft().y, nameBox.getLowerLeft().z);
		glVertex3f(planetBox.getUpperLeft().x, planetBox.getUpperLeft().y, planetBox.getUpperLeft().z);
	glEnd();*/
	
	Video.PopProjection();

	float radius = Hsi.galaxy.systems[Hsi.m_mainDisplayContext.activeSystem].planets[0].GetWorldMatrix().getTranslation().distance(GetWorldMatrix().getTranslation());
	CColor orbitColor(201, 207, 218, 75);
	Video.DrawOrbit(Hsi.galaxy.systems[Hsi.m_mainDisplayContext.activeSystem].planets[0].GetWorldMatrix().getTranslation(), radius, orbitColor, camera);

}
CSmartPointer<CModel> CHSIPlanet::GetModel()
{
	return this->m_spModel;
}

//void CHSIPlanet::SetModel(const CCalModel& model)
//{
	//this->m_modelHighRes = model;
	//this->FindMaximumRadius();
//	this->m_modelDisplayList = Video.GenerateDisplayListFromModel(this->m_modelHighRes);
//}

void CHSIPlanet::SyncColonyData()
{
	v3d location = this->GetWorldMatrix().getTranslation();
	m_colony.locationX = (int)location.x;
	m_colony.locationY = (int)location.y;
	m_colony.locationZ = (int)location.z;

	m_colony.planetID.planetNumber = this->m_planetID.planetNumber;
	m_colony.planetID.systemNumber = this->m_planetID.systemNumber;
}

void CHSIPlanet::Think()
{
	m_worldMatrix = /*RotationMatrix(v3d(0,-.5*sys_frameTime)) **/ m_worldMatrix * RotationMatrix(v3d(0, 5*sys_frameTime, 0)) ;
	
	// sync m_colony with CHSIPlanet
	SyncColonyData();

	if(m_nextTurn <= sys_curTime)
	{
		m_nextTurn = sys_curTime + 10.0f;
		this->m_colony.nextTurn();
	}

	// update the contents of the system and colony windows
	if(m_colonyViewWindow)
	{
		IGuiWidget* widget = m_colonyViewWindow->widgetPanel.GetWidgetByName("colonyinfo");
		CGuiFieldWidget* field = dynamic_cast<CGuiFieldWidget*>(widget);
		field->contents = m_colony.getDisplayString();
	}
	if(m_systemViewWindow)
	{
		IGuiWidget* widget = m_systemViewWindow->widgetPanel.GetWidgetByName("overview");
		CGuiFieldWidget* field = dynamic_cast<CGuiFieldWidget*>(widget);
		field->contents = m_colony.getOverviewDisplayString();
		if(field->contents == "")
			m_systemViewWindow->visible = false;
	}
}

CHSIPlanet CHSIPlanet::operator=(const CHSIPlanet& rhs)
{
	CopyFrom(rhs);
	return *this;
}
void CHSIPlanet::CopyFrom(const CHSIPlanet& rhs)
{
	m_systemViewWindowSpecID = rhs.m_systemViewWindowSpecID;
	m_systemViewWindowID = Gui.WM.MakeWindowInstance( m_systemViewWindowSpecID );
	if(Gui.WM.WindowIDIsValid(m_systemViewWindowID))
	{
		m_systemViewWindow = Gui.WM.GetWindowInstance(m_systemViewWindowID);
	}

	m_colonyViewWindowSpecID = rhs.m_colonyViewWindowSpecID;
	m_colonyViewWindowID = Gui.WM.MakeWindowInstance( m_colonyViewWindowSpecID );
	if(Gui.WM.WindowIDIsValid(m_colonyViewWindowID))
	{
		m_colonyViewWindow = Gui.WM.GetWindowInstance(m_colonyViewWindowID);
	}

	if(m_systemViewWindow)
	{
		if(rhs.m_systemViewWindow)
			m_systemViewWindow->visible = rhs.m_systemViewWindow->visible;
		m_systemViewWindow->StartTracking(*this);
	}
	if(m_colonyViewWindow)
		if(rhs.m_colonyViewWindow)
			m_colonyViewWindow->visible = rhs.m_colonyViewWindow->visible;
	this->m_lensflare = rhs.m_lensflare;
	this->m_lensFlareGraphic = rhs.m_lensFlareGraphic;
	//if(this->m_model)
	//	CCalCoreModelRepository::GetInstance()->ReleaseModelInstance(m_model);
	this->m_spModel = CSmartPointer<CModel>(new CModel(*rhs.m_spModel), false); 
	this->m_name = rhs.m_name;
	this->m_radius = rhs.m_radius;
	this->m_colony = rhs.m_colony;
	this->m_planetID = rhs.m_planetID;
	this->m_nextTurn = rhs.m_nextTurn;
	this->m_worldMatrix = rhs.m_worldMatrix;
}

