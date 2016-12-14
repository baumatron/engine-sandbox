
#include "CHSIMainDisplayContext.h"
#include "CHSISubsystem.h"

#include "CGuiSubsystem.h"

#include "CCardinalSpline.h"
#include "sys_main.h"
#include "CShip.h"
//#include "CMilkShapeModel.h"

CCardinalSpline spline;
CCamera splinecam;
GuiWindowID splineWindowID;
CShip testships[4];
//CCalModel testmodel;

bool lookingAtTestShip(false);

//CSmartPointer<CModel> glodModel;
CShip glodShip;

VideoResourceID backgroundimage;

CSmartPointer<float> stars;
CSmartPointer<float> starColor;
CSmartPointer<float> nebula;
CSmartPointer<float> nebulaColor;

const int starCount = 3000;
const int nebulaVertices = 50;
void CreateStars()
{
	stars.New(3*starCount);// = CSmartPointer<float>(new float[3*starCount], true);
	starColor.New(3*starCount);// = CSmartPointer<float>(new float[4*starCount], true);
	nebula.New(3*nebulaVertices);// = CSmartPointer<float>(new float[3*nebulaVertices], true);
	nebulaColor.New(4*nebulaVertices);// = CSmartPointer<float>(new float[4*nebulaVertices], true);
	for(int i = 0; i < starCount; i++)
	{
		v3d starPosition(0,0,100);
		starPosition.rotate(0, v3d( 180*((double)rand()/(double)RAND_MAX), 180*((double)rand()/(double)RAND_MAX), 360*((double)rand()/(double)RAND_MAX) ));
		stars[i*3+0] = starPosition.x;
		stars[i*3+1] = starPosition.y;
		stars[i*3+2] = starPosition.z;
		float color = ((double)rand()/(double)RAND_MAX); //brightness
		starColor[i*3+0] = color;
		starColor[i*3+1] = color;
		starColor[i*3+2] = color;
		//starColor[i*4+3] = color;
	}
	for(int i = 0; i < nebulaVertices; i++)
	{
		v3d starPosition(0,0,100);
		starPosition.rotate(0, v3d( -15+30*((double)rand()/(double)RAND_MAX), i*2.5+1.25*((double)rand()/(double)RAND_MAX), 0 ));
		nebula[i*3+0] = starPosition.x;
		nebula[i*3+1] = starPosition.y;
		nebula[i*3+2] = starPosition.z;
		nebulaColor[i*4+0] = ((double)rand()/(double)RAND_MAX); //brightness
		nebulaColor[i*4+1] = nebulaColor[i*4+0]*.5;//((double)rand()/(double)RAND_MAX); //brightness
		nebulaColor[i*4+2] = 0;//((double)rand()/(double)RAND_MAX); //brightness
		nebulaColor[i*4+3] = ((double)rand()/(double)RAND_MAX); //brightness
	}
//	backgroundimage = Video.VideoResourceManager.LoadImageToVideoMemory("MarsMap_2500x1250.tga");
}

void RenderImage()
{
//	Video.BlitBitmap(backgroundimage, v3d(32,32), v3d(Video.settings.getSw()-100, Video.settings.getSh()-100));
}


void RenderStars()
{
	CCamera skyboxCam;
	skyboxCam.SetTransformationGoal(TranslationMatrix(Hsi.m_mainDisplayContext.camera.GetTransformation().getTranslation()).getInverse() * Hsi.m_mainDisplayContext.camera.GetTransformation());
	glDisable(GL_LIGHTING);
//	Hsi.m_mainDisplayContext.skybox->Render(skyboxCam, matrix4x4::mtfIdentity());
//	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(Video.settings.getSh())
		gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 10000);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLoadMatrixf((skyboxCam.GetModelViewMatrix()).matrix);

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND); // DISABLED BECAUSE OF BLENDING PROBLEMS /////////////////////////////////////////////////////////////////////
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable(GL_BLEND);
   // glBlendFunc( GL_ONE, GL_ONE );

	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable( GL_TEXTURE_2D );

//	glEnable(GL_BLEND);
//	glEnable(GL_POINT_SMOOTH);
	glPointSize(2.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, stars.GetPointer()); 
	glColorPointer(3, GL_FLOAT, 0, starColor.GetPointer());
	glDrawArrays( GL_POINTS, 0, starCount );		

//	glVertexPointer(3, GL_FLOAT, 0, nebula.GetPointer()); 
//	glColorPointer(4, GL_FLOAT, 0, nebulaColor.GetPointer());
//	glDrawArrays( GL_TRIANGLE_STRIP, 0, nebulaVertices );		// Draw All Of The Triangles At Once

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void CreateGlodModel()
{
	glodShip.Initialize("supercarrier.ms3d");
}

void RenderGlodModel()
{

	RenderStars();
	CModel::verticesRendered = 0;
	CModel::trianglesRendered = 0;

//	glodGroupParameteri(GLOD_GLOBAL_GROUP, GLOD_MAX_TRIANGLES, CModelRepository::Instance()->GetTriangleBudget());
	// set object xform
/*	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	if(Video.settings.getSh())
		gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLoadMatrixf((Hsi.m_mainDisplayContext.camera.GetModelViewMatrix()).matrix);
		glodBindObjectXform(0, GL_PROJECTION_MATRIX | GL_MODELVIEW_MATRIX);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();*/

	//glodAdaptGroup(0);

	CModelRepository::Instance()->UpdateGlod();

	glodShip.Draw(Hsi.m_mainDisplayContext.camera);
}

void RenderMainViewport()
{
	//testship.Think();
	//if(lookingAtTestShip)
	//{
	//	Hsi.m_mainDisplayContext.camera.SetTransformationGoal(TranslationMatrix(testship.m_model.worldMatrix.getTranslation() + v3d(80,20,-40)));
	//	Hsi.m_mainDisplayContext.camera.LookAt(testship.m_model.worldMatrix.getTranslation());
	//}
//	{// do glod stuff
		CModel::verticesRendered = 0;
		CModel::trianglesRendered = 0;

	/*	//glodGroupParameteri(GLOD_GLOBAL_GROUP, GLOD_MAX_TRIANGLES, 10000);
		// set object xform
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		if(Video.settings.getSh())
			gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glLoadMatrixf((Hsi.m_mainDisplayContext.camera.GetModelViewMatrix()).matrix);
			glodBindObjectXform(0, GL_PROJECTION_MATRIX | GL_MODELVIEW_MATRIX);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glodAdaptGroup(0);*/
	//}

	CModelRepository::Instance()->UpdateGlod();

	RenderStars();
	

/*	ccout << "Testship: " << testship.m_model.worldMatrix.getTranslation() << newl;
	ccout << "Camera: " << Hsi.m_mainDisplayContext.camera.GetTransformation().getTranslation() << newl;
*/
	Video.PushProjection3d();
/*	glLoadMatrixf((Hsi.m_mainDisplayContext.camera.GetTransformation().getInverse()).matrix);
	Video.vcout.setColor(CColor(0.0f, 0.8f, 1.0f));
	v3d origin = Video.ProjectPoint(v3d(0,0,0));
	v3d point = Video.ProjectPoint(v3d(50,0,0));
	Video.vcout.setPos(v3d(Video.TopViewportContext().area.getWidth()/2, Video.TopViewportContext().area.getHeight()/2) + origin);
	Video.vcout << "origin";
	Video.vcout.setPos(v3d(Video.TopViewportContext().area.getWidth()/2, Video.TopViewportContext().area.getHeight()/2) + point);
	Video.vcout << "point";*/


// set up the light for the star
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Create light components
	float ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.25f);

	v3d camposition = Hsi.m_mainDisplayContext.camera.GetTransformation().getTranslation();
	camposition.makeUnitVector();
	camposition *= 1000;
	float campositionfv[] = {camposition.x, camposition.y, camposition.z};
	glLightfv(GL_LIGHT0, GL_POSITION, campositionfv);

//	for(int i =0 ; i < 4; i++)
//	{
//		testships[i].Draw(Hsi.m_mainDisplayContext.camera);
//	}
//	testmodel.Render(Hsi.m_mainDisplayContext.camera);
//	Video.DrawModel(testmodel, Hsi.m_mainDisplayContext.camera);
	Hsi.galaxy.Draw(Hsi.m_mainDisplayContext.camera, Hsi.m_mainDisplayContext);
//	testship.Draw(Hsi.m_mainDisplayContext.camera);
	Video.PopProjection();
}

CHSIMainDisplayContext::CHSIMainDisplayContext()
{
	activePlanet = 0;
	activeSystem = 0;
	activeShip = 0;
	viewTier = colony;
	skybox = 0;
}
CHSIMainDisplayContext::~CHSIMainDisplayContext()
{
	if(skybox)
		CCalCoreModelRepository::GetInstance()->ReleaseModelInstance(skybox);
}

void CHSIMainDisplayContext::Initialize()
{
	CreateGlodModel();
	this->SetupMainDisplayWindow();
	
//	CMilkShapeModelLoader::GetInstance()->LoadModel(testmodel, "supercarrier.ms3d");//.ReadModelFromFile("supercarrier.ms3d");
	//CCalModel temp;
	//MilkshapeModelLoader.LoadModel(testmodel);
	//testship.SetModel(testmodel);
	//testship.m_modelLow = testmodel;

/*	for(int i = 0; i < 4; i++)
	{
		testships[i].Initialize("supercarrier.ms3d");
		testships[i].SetWorldMatrix(matrix4x4::mtfTranslate(v3d(0,0,0)));// * matrix4x4::mtfScale(v3d(0.002,0.002,0.002));
		testships[i].SetPath(spline);
	}
	testships[0].SetWorldMatrix(matrix4x4::mtfTranslate(v3d(0,0,0)));// * matrix4x4::mtfScale(v3d(0.002,0.002,0.002));
	testships[1].SetWorldMatrix(matrix4x4::mtfTranslate(v3d(500,0,0)));// * matrix4x4::mtfScale(v3d(0.002,0.002,0.002));
	testships[2].SetWorldMatrix(matrix4x4::mtfTranslate(v3d(-500,0,0)));// * matrix4x4::mtfScale(v3d(0.002,0.002,0.002));
	testships[3].SetWorldMatrix(matrix4x4::mtfTranslate(v3d(0,-500,0)));// * matrix4x4::mtfScale(v3d(0.002,0.002,0.002));
*/
	this->camera.SetTransitionType(CCamera::average);
	this->camera.SetTransitionSpeed(14);
	//this->camera.StartTracking(testships[0], 1250);
	if(!Hsi.galaxy.systems.empty())
		this->camera.StartTracking(Hsi.galaxy.systems[this->activeSystem].planets[this->activePlanet], Hsi.galaxy.systems[this->activeSystem].planets[this->activePlanet].GetRadius() * 4);
	else
		this->camera.StartTracking(glodShip, (*glodShip.m_model).FindMaximumRadius() * 2.5);
	skybox = CCalCoreModelRepository::GetInstance()->GetModelInstance("stars.ms3d");
}

void CHSIMainDisplayContext::SetupMainDisplayWindow()
{
	CreateStars();

	if(true)
		Video.SetBackgroundRenderer(RenderMainViewport);
	//else
	//	Video.SetBackgroundRenderer(RenderGlodModel);
	//Video.SetBackgroundRenderer(RenderImage);
	

	//SetupSpline();

	CCardinalSpline spline;
	spline.SetAlpha(5);
/*	spline.AddVertex(v3d(1000,0,0));
	spline.AddVertex(v3d(0,1000,0));
	spline.AddVertex(v3d(0,1000,1000));
	spline.AddVertex(v3d(0,850,850));
	spline.AddVertex(v3d(-1000,0,0));
	spline.AddVertex(v3d(0,-1000,0));
	spline.AddVertex(v3d(100,0,0));
	spline.SetEntryVector(v3d(0,500,0));
	spline.SetExitVector(v3d(0,500,0)*-1);*/

	spline.AddVertex(v3d(10000,0,0) + v3d(0,0,0));
	spline.AddVertex(v3d(10000,0,0) + v3d(-1000,0,-1000));
	spline.AddVertex(v3d(10000,0,0) + v3d(-2000,1000,0));
	spline.AddVertex(v3d(10000,0,0) + v3d(-1000,0,1000));
	spline.AddVertex(v3d(10000,0,0) + v3d(0,0,0));
	spline.AddVertex(v3d(10000,0,0) + v3d(1000,0,-1000));
	spline.AddVertex(v3d(10000,0,0) + v3d(2000,-1000,0));
	spline.AddVertex(v3d(10000,0,0) + v3d(1000,0,1000));
	spline.AddVertex(v3d(10000,0,0) + v3d(0,0,0));
	spline.SetEntryVector(v3d(-200,0,-200));
	spline.SetExitVector(v3d(-200,0,-200)*-1);




	/*spline.AddVertex(v3d(1500,0,-1000));
	spline.AddVertex(v3d(1450,50,-1000));
	spline.AddVertex(v3d(1500,100,-950));
	spline.AddVertex(v3d(1400,200,-800));
	spline.AddVertex(v3d(1500,0,-1000));
	spline.SetEntryVector(v3d(-100,0,100));
	spline.SetExitVector(v3d(-100,0,100)*-1);*/
	//testship.SetPath(spline);
}

void CHSIMainDisplayContext::Think(void)
{
	this->camera.Think();
}

void CHSIMainDisplayContext::ZoomOut()
{
	if(viewTier != galactic)
		SetViewTier((ViewTiers)(viewTier-1));
}
void CHSIMainDisplayContext::ZoomIn()
{
	if(viewTier != ship)
		SetViewTier((ViewTiers)(viewTier+1));
}
void CHSIMainDisplayContext::CycleForwardInTier()
{
	switch(viewTier)
	{
	case ship:
		{
			if(Hsi.galaxy.ships.empty())
				break;
			activeShip++;
			if(activeShip >= Hsi.galaxy.ships.size())
				activeShip = 0;
			UpdateCameraTracking();
		}
		break;
	case colony:
		{
			if(Hsi.galaxy.systems.empty())
				break;
			CHSISystem& system = Hsi.galaxy.systems[activeSystem];
			activePlanet++;
			if(activePlanet >= system.planets.size())
				activePlanet = 0;
			SetActivePlanet(activePlanet);
		}
		break;
	case system:
	case galactic:
		{
			if(Hsi.galaxy.systems.empty())
				break;
			activeSystem++;
			if(activeSystem >= Hsi.galaxy.systems.size())
				activeSystem = 0;
			SetActiveSystem(activeSystem);
		}
		break;	
	}
}
void CHSIMainDisplayContext::CycleBackwardInTier()
{
	switch(viewTier)
	{
	case ship:
		{
			if(Hsi.galaxy.ships.empty())
				break;
			activeShip--;
			if(activeShip < 0)
				activeShip = Hsi.galaxy.ships.size()-1;
			UpdateCameraTracking();
		}
		break;
	case colony:
		{
			if(Hsi.galaxy.systems.empty())
				break;
			CHSISystem& system = Hsi.galaxy.systems[activeSystem];
			activePlanet--;
			if(activePlanet < 0)
				activePlanet = system.planets.size()-1;
			SetActivePlanet(activePlanet);
		}
		break;
	case system:
	case galactic:
		{
			if(Hsi.galaxy.systems.empty())
				break;
			activeSystem--;
			if(activeSystem < 0)
				activeSystem = Hsi.galaxy.systems.size()-1;
			SetActiveSystem(activeSystem);
		}
		break;	
	}
}




void CHSIMainDisplayContext::UpdateCameraTracking()
{
	float viewDistance(0);
	switch(viewTier)
	{
	case ship:
		{
			if(Hsi.galaxy.ships.empty())
				break;
			viewDistance = 2.5;
			camera.StartTracking(Hsi.galaxy.ships[activeShip], viewDistance);
		}
		break;
	case colony:
		if(Hsi.galaxy.systems.empty())
			break;
		viewDistance = 120;
		camera.StartTracking(Hsi.galaxy.systems[activeSystem].planets[activePlanet], Hsi.galaxy.systems[activeSystem].planets[activePlanet].GetRadius() * 4);
	//	camera.SetScaleMatrix(matrix4x4::mtfScale(v3d(1,1,1)));
		break;
	case system:
		if(Hsi.galaxy.systems.empty())
			break;
		viewDistance = 500000;
		camera.StartTracking(Hsi.galaxy.systems[activeSystem].planets[0], viewDistance);
	//	camera.SetScaleMatrix(matrix4x4::mtfScale(v3d(1,1,1)));
		break;
	case galactic:
		if(Hsi.galaxy.systems.empty())
			break;
		viewDistance = 2000000;
		camera.StartTracking(Hsi.galaxy.systems[activeSystem].planets[0], viewDistance);
	//	camera.SetScaleMatrix(matrix4x4::mtfScale(v3d(10,10,10)));
		break;
	}
}

void CHSIMainDisplayContext::SetViewTier(ViewTiers tier)
{
	viewTier = tier;
	UpdateCameraTracking();
}
void CHSIMainDisplayContext::SetActivePlanet(int planet)
{
	activePlanet = planet;
	UpdateCameraTracking();
}
void CHSIMainDisplayContext::SetActiveSystem(int system)
{
	activePlanet = 0;
	activeSystem = system;
	UpdateCameraTracking();
}
