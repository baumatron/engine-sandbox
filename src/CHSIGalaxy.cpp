
#include "CShip.h"
#include "CHSIGalaxy.h"
#include "CXMLParser.h"
//#include "CMilkShapeModel.h"
#include "CPythonSubsystem.h"
#include <boost/python.hpp>
using namespace boost::python;
#include <fstream>
using namespace std;

void CHSIGalaxy::Draw(CCamera& camera, CHSIMainDisplayContext& context)
{
	for(int i = 0; i < systems.size(); i++)
	{
		systems[i].Draw(camera, context, (context.activeSystem == i));
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(Video.settings.getSh())
		gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	for(vector<CShip>::iterator it = ships.begin(); it != ships.end(); it++)
	{
		it->Draw(camera);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


}

void CHSIGalaxy::Think()
{
	for(vector<CHSISystem>::iterator it = systems.begin(); it != systems.end(); it++)
	{
		it->Think();
	}
}

void CHSIGalaxy::ReadFromFile(string filename)
{
	// todo: check this function out, it is fucking up when not running in the ide
	ifstream fin(filename.c_str());

	string galaxy = CXMLParser::GetInstance()->GetTagData(fin, "galaxy");

	int systemNumber(0);
	string system = CXMLParser::GetInstance()->GetTagData(galaxy, "system", systemNumber);
	while (system != "")
	{
		CHSISystem systemObject;

		int planetNumber(0);
		string planet = CXMLParser::GetInstance()->GetTagData(system, "planet", planetNumber);
		while (planet != "")
		{
			CHSIPlanet planetObject;

			//planetObject.Initialize(model);

//			CCalModel theModel;
//			planetObject.m_model = CCalCoreModelRepository::GetInstance()->GetModelInstance(model);
			//CMilkShapeModelLoader::GetInstance()->LoadModel(theModel, model);
//			planetObject.SetModel(theModel);

			string lensflare = CXMLParser::GetInstance()->GetTagData(planet, "lensflare");
			planetObject.m_lensflare = M_atob(lensflare);
			if(planetObject.m_lensflare)
				planetObject.m_lensFlareGraphic = Video.VideoResourceManager.LoadImageToVideoMemory("flare.tga");

			string name = CXMLParser::GetInstance()->GetTagData(planet, "name");
			planetObject.SetName(name);

			string transform = CXMLParser::GetInstance()->GetTagData(planet, "transform");
			v3d translation, rotation;
			translation.x = atof(CXMLParser::GetInstance()->GetTagData(transform, "x").c_str());
			translation.y = atof(CXMLParser::GetInstance()->GetTagData(transform, "y").c_str());
			translation.z = atof(CXMLParser::GetInstance()->GetTagData(transform, "z").c_str());
			rotation.x = atof(CXMLParser::GetInstance()->GetTagData(transform, "xr").c_str());
			rotation.y = atof(CXMLParser::GetInstance()->GetTagData(transform, "yr").c_str());
			rotation.z = atof(CXMLParser::GetInstance()->GetTagData(transform, "zr").c_str());

			planetObject.SetWorldMatrix(TranslationMatrix(translation) * RotationMatrix(rotation));
			//planetObject.modelHighRes.worldMatrix = TranslationMatrix(translation) * RotationMatrix(rotation);

			planetObject.m_planetID.planetNumber = planetNumber;
			planetObject.m_planetID.systemNumber = systemNumber;
			if(planetObject.GetName() == "Earth")
			{
				planetObject.m_colony = colony(planetNumber, systemNumber, translation.x, translation.y, translation.z, 1);
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(3);
				planetObject.m_colony.queueShip(1);
			}
			if(planetObject.GetName() == "Mars")
			{
				planetObject.m_colony = colony(planetNumber, systemNumber, translation.x, translation.y, translation.z);
				planetObject.m_colony.colonize();
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(3);
				planetObject.m_colony.buildBasicBuilding(2);
				planetObject.m_colony.queueShip(2);
			}
			string model = CXMLParser::GetInstance()->GetTagData(planet, "model");
			planetObject.Initialize(model);

			systemObject.planets.push_back(planetObject);


			planet = CXMLParser::GetInstance()->GetTagData(system, "planet", ++planetNumber);
		} 
		systems.push_back(systemObject);

		system = CXMLParser::GetInstance()->GetTagData(galaxy, "system", ++systemNumber);
	}
	fin.close();
}

void CHSIGalaxy::GenerateRandomly()
{
	Python.ExecuteString("textures = []");
	Python.ExecuteFile("data\\images\\planets\\planettextures.py");

	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	object main_namespace = main_module.attr("__dict__");
	object py_textures = main_namespace["textures"];
	vector<string> planetTextures;

	for(int i = 0; i < PyList_Size(py_textures.ptr()); i++)
	{
		planetTextures.push_back( extract<string>(py_textures[i]) );
	}

	for(int systemNumber = 0; systemNumber < 10; systemNumber++)
	{
		CHSISystem systemObject;
		v3d systemPosition;
		systemPosition.x = -500000 + 1000000 * (double)rand()/(double)RAND_MAX;
		systemPosition.y = -500000 + 1000000 * (double)rand()/(double)RAND_MAX;
		systemPosition.z = -500000 + 1000000 * (double)rand()/(double)RAND_MAX;

		int numPlanets = 6 * (double)rand()/(double)RAND_MAX;
		if(numPlanets < 2)
			numPlanets = 2;
		for(int planetNumber = 0; planetNumber < numPlanets; planetNumber++)
		{
			CHSIPlanet planetObject;
			v3d planetPosition;
			planetPosition.x = -100000 + 200000 * (double)rand()/(double)RAND_MAX;
			planetPosition.y = 0;//-50000 + 100000 * (double)rand()/(double)RAND_MAX;
			planetPosition.z = -100000 + 200000 * (double)rand()/(double)RAND_MAX;
			planetPosition += systemPosition;

			string model = (planetNumber == 0) ? "sun.ms3d" : "mars.ms3d";//CXMLParser::GetInstance()->GetTagData(planet, "model");
			//CCalModel theModel;
			//CMilkShapeModelLoader::GetInstance()->LoadModel(theModel, model);
			//planetObject.SetModel(theModel);
	//		planetObject.Initialize(model);


			planetObject.m_lensflare = planetNumber == 0;//M_atob(lensflare);
			if(planetObject.m_lensflare)
				planetObject.m_lensFlareGraphic = Video.VideoResourceManager.LoadImageToVideoMemory("flare.tga");

			string name = (planetNumber == 0) ? ("Star " + M_itoa(systemNumber)): ("Planet " + M_itoa(planetNumber));//CXMLParser::GetInstance()->GetTagData(planet, "name");
			planetObject.SetName(name);
			
			v3d translation, rotation;
			translation = planetPosition;
			rotation = 0;

			planetObject.SetWorldMatrix(TranslationMatrix(translation) * RotationMatrix(rotation));

			planetObject.m_planetID.planetNumber = planetNumber;
			planetObject.m_planetID.systemNumber = systemNumber;
			if(planetObject.GetName() == "Earth")
			{
				planetObject.m_colony = colony(planetNumber, systemNumber, translation.x, translation.y, translation.z, 1);
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(3);
				planetObject.m_colony.queueShip(1);
			}
			if(planetObject.GetName() == "Mars")
			{
				planetObject.m_colony = colony(planetNumber, systemNumber, translation.x, translation.y, translation.z);
				planetObject.m_colony.colonize();
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(1);
				planetObject.m_colony.buildBasicBuilding(3);
				planetObject.m_colony.buildBasicBuilding(2);
				planetObject.m_colony.queueShip(2);
			}
			planetObject.Initialize(model);

			CSmartPointer<CCoreModel> planetModel = planetObject.GetModel()->GetCoreModel();
			if(model == "mars.ms3d")
			{
				//CMaterial planetMaterial;

				for(int i = 0; i < planetModel->m_materialCount; i++) // this seems ugly... oh well
					planetModel->m_pMaterials[i].SetTexture("data/images/planets/1.tga");
					//planetModel.m_pMaterials[i].m_texture = Video.VideoResourceManager.LoadImageToVideoMemory("data/images/planets/gas1.tga");

			}

			systemObject.planets.push_back(planetObject);
		} 
		systems.push_back(systemObject);
	}

	CShip aship;
	CCardinalSpline spline;
	spline.SetAlpha(5);
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

// generate a random shit ton of ships

	Python.ExecuteString("shipmodels = []");
	Python.ExecuteFile("data/shipmodels.py");

	//main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));
	//main_namespace = main_module.attr("__dict__");
	object py_shipmodels = main_namespace["shipmodels"];
	vector<string> shipModels;
	string modelDirectory = "data/models/";
	for(int i = 0; i < PyList_Size(py_shipmodels.ptr()); i++)
	{
		shipModels.push_back( modelDirectory + (string)extract<string>(py_shipmodels[i]) );
	}

	matrix4x4 fleetCenter =  matrix4x4::mtfTranslate(this->GetPlanetLocation(0, 1).getTranslation() + v3d(this->GetPlanet(0,1).GetRadius()*4/3) );


	srand(102);
	if(shipModels.size())
	for(int i = 0; i < 5; i++)
	{
		v3d shipPosition;
		shipPosition.x = -10 + 20 * (double)rand()/(double)RAND_MAX;
		shipPosition.y = -10 + 20 * (double)rand()/(double)RAND_MAX;
		shipPosition.z = -10 + 20 * (double)rand()/(double)RAND_MAX;
		volatile double fraction = (double)rand()/(double)RAND_MAX;
		volatile double size = (double)(shipModels.size()) ;
		volatile int modelIndex = size * fraction;
		if(modelIndex > shipModels.size())
			ccout << "Error!!!!\n";
		string mf = shipModels[modelIndex];
		string mfl = mf.substr(0, mf.size()-5) + 'l' + mf.substr(mf.size()-5, 5);


		aship.Initialize(mf);

//		CSmartPointer<CCoreModel> shipModel = aship.m_model->GetCoreModel();
	//	for(int i = 0; i < shipModel->m_materialCount; i++) // this seems ugly... oh well
	//		shipModel->m_pMaterials[i].SetTexture("chaimail.tga");

	//	CCalModel theModel;

	//	if(CMilkShapeModelLoader::GetInstance()->LoadModel(theModel, mf))
	//	{
	//		aship.SetModel(theModel);
	//		aship.m_modelLow = aship.m_model;		
	//	}

		/*if(MilkshapeModelLoader.ReadModelFromFile(mf))
		{
			MilkshapeModelLoader.LoadModel(theModel);
			aship.SetModel(theModel);
			aship.m_modelLow = aship.m_model;
		}*/
		//if(MilkshapeModelLoader.ReadModelFromFile(mfl))
		//	MilkshapeModelLoader.LoadModel(aship.m_modelLow);
		//else
		//	aship.m_modelLow = aship.m_model;

		aship.SetWorldMatrix(matrix4x4::mtfTranslate(shipPosition) * fleetCenter * matrix4x4::mtfScale(v3d(0.002,0.002,0.002)));
		aship.SetPath(spline);
		ships.push_back(aship);
	}
//	aship.m_model.worldMatrix = matrix4x4::mtfTranslate();


}

matrix4x4 CHSIGalaxy::GetPlanetLocation(int system, int planet)
{
	return systems[system].planets[planet].GetWorldMatrix();	
}

colony& CHSIGalaxy::GetPlanetColony(int system, int planet)
{
	return systems[system].planets[planet].m_colony;
}
CHSIPlanet& CHSIGalaxy::GetPlanet(int system, int planet)
{
	return systems[system].planets[planet];
}


