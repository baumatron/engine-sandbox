#include "ship.h"
#include <stdlib.h>
#include "colony.h"
#include <math.h>

#include "math_main.h"
//#include "../CHSIGalaxy.h"
#include "CHSISubsystem.h"
#include <sstream>
using namespace std;
/*
extern int NUMBER_OF_SYSTEMS;
extern int NUMBER_OF_PLANETS;
extern colony* colonyList[3];
extern ship* shipList[100];*/
extern int IDticker;


ship::ship(int buildLocationSystemNumber, int buildLocationPlanetNumber, int typeIN, int buildLocationX, int buildLocationY, int buildLocationZ)
{
	id = IDticker; // brian changed, temporarily
	type = typeIN;
	locationX = buildLocationX;
	locationY = buildLocationY;
	locationZ = buildLocationZ;
	destLocationX = 0;
	destLocationY = 0;
	destLocationZ = 0;
	destETA = 0;
	locationID.planetNumber = buildLocationPlanetNumber;
	locationID.systemNumber = buildLocationSystemNumber;
	destinationID.planetNumber = 0;
	destinationID.systemNumber = 0;
	switch(type){
		case 1:
			shield = 1;
			hull = 2;
			speed = 2;
			gunDam = 1;
			gunAcc = 1;
			break;
		case 2:
			shield = 0;
			hull = 1;
			speed = 1;
			gunDam = 0;
			gunAcc = 0;
			break;
	}

}

void ship::changeDestination(int destinationSystemNumber, int destinationPlanetNumber)
{
	if ((destinationSystemNumber == locationID.systemNumber) && (destinationPlanetNumber == locationID.planetNumber))
		return;
	else
	{	
		locationID.planetNumber = 0;	//means in transit
		//locationID.systemNumber = 0;
		destinationID.planetNumber = destinationPlanetNumber;
		destinationID.systemNumber = destinationSystemNumber;

		//for (int i = 0; i < NUMBER_OF_PLANETS; i++)
		{ // brian changed this
			//if ((destinationSystemNumber == colonyList[i]->planetID.systemNumber) && (destinationPlanetNumber == colonyList[i]->planetID.planetNumber))
			v3d destLocation = Hsi.galaxy.GetPlanetLocation(destinationSystemNumber, destinationPlanetNumber).getTranslation();

			{
				destLocationX = destLocation.x; //colonyList[i]->locationX;
				destLocationY = destLocation.y; //colonyList[i]->locationY; // brian changed this
				destLocationZ = destLocation.z; //colonyList[i]->locationZ;
				int xDist = 0;
				int yDist = 0;
				int zDist = 0;
				double distVect = 0;
				xDist = locationX - destLocationX;
				yDist = locationY - destLocationY;
				zDist = locationZ - destLocationZ;
				distVect = sqrt(double(xDist * xDist) + double(yDist * yDist) + double(zDist* zDist));
				destETA = distVect / (speed * 50);
			}
		}

	}
}

ship ship::operator=(ship src)
{
	id = src.id;
	type = src.type;
	locationX = src.locationX;
	locationY = src.locationY;
	locationZ = src.locationZ;
	locationID = src.locationID;
	destinationID = src.destinationID;
	shield = src.shield;
	hull = src.hull;
	speed = src.speed;
	gunDam = src.gunDam;
	gunAcc = src.gunAcc;

	return *this;
}

void ship::display()
{
	cout<<"SHIP #"<<id<<endl;
	cout<<"Planet :"<<locationID.planetNumber<<endl;
	cout<<"System :"<<locationID.systemNumber<<endl;
	cout<<"Class  :"<<type<<endl;
	cout<<"Location: "<<locationID.planetNumber<<endl;
	cout<<"Destination "<<destinationID.planetNumber<<endl;
	cout<<"ETA "<<destETA<<endl<<endl;
}

string ship::getDisplayString() // brian added
{
	ostringstream oss;
	oss<<"SHIP #"<<id<<endl;
	oss<<"Planet :"<<locationID.planetNumber<<endl;
	oss<<"System :"<<locationID.systemNumber<<endl;
	oss<<"Class  :"<<type<<endl;
	oss<<"Location: "<<locationID.planetNumber<<endl;
	oss<<"Destination "<<destinationID.planetNumber<<endl;
	oss<<"ETA "<<destETA<<endl<<endl;
	return oss.str();
}


void ship::colonize()
{
	/*for(int i = 0; i < NUMBER_OF_PLANETS; i++)
	{
		if ((colonyList[i]->planetID.planetNumber == locationID.planetNumber) && (!colonyList[i]->flagOwnedColony))
		{
			colonyList[i]->colonize();
		}
	}
*/
	// brian changed
	colony& theColony = Hsi.galaxy.GetPlanetColony(locationID.systemNumber, locationID.planetNumber);
	theColony.colonize();
}


void ship::nextTurn()
{/*
	int xDist = 0;
	int yDist = 0;
	int zDist = 0;
	double ratioX = 0;
	double ratioY = 0;
	double ratioZ = 0;
	double distVect = 0;
	if (locationID.planetNumber == 0)
	{
		xDist = locationX - destLocationX;
		yDist = locationY - destLocationY;
		zDist = locationZ - destLocationZ;
		distVect = sqrt(double(xDist * xDist) + double(yDist * yDist) + double(zDist* zDist));

		ratioX = xDist/distVect;
		ratioY = yDist/distVect;
		ratioZ = zDist/distVect;

		distVect -= (speed * 50);

		locationX -= (locationX - (ratioX * distVect));
		locationY -= (locationY - (ratioY * distVect));
		locationZ -= (locationZ - (ratioZ * distVect));
		destETA = distVect / (speed * 50);
		if (distVect == 0)
		{
			for(int i = 0; i < NUMBER_OF_PLANETS; i++)
			{
				if((locationX == colonyList[i]->locationX) && (locationY == colonyList[i]->locationY) && (locationZ == colonyList[i]->locationZ))
					locationID.planetNumber = colonyList[i]->planetID.planetNumber;
			}
		}
	}*/
	if (destETA != 0)
	{
		destETA--;
		if (destETA == 0)
		{
			locationID.planetNumber = destinationID.planetNumber;
			destinationID.planetNumber = 0;
		}
	}

}