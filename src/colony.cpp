#include "colony.h"
#include "ship.h"
#include <stdlib.h>
#include <sstream>
using namespace std;
/*
extern int NUMBER_OF_SYSTEMS;
extern int NUMBER_OF_PLANETS;
extern colony* colonyList[3];*/
ship* shipList[100]; // this is an array of pointers, should all be initialized to zeros, and memory should be freed when done.. leaving for now
/*extern int IDticker;*/
int IDticker(0);

	colony::colony()
	{
		population = 0;
		populationGrowthRate = 0;
		productionPts = (population / 1000);
		basicBuildings = 0;
		farms = 0;
		mines = 0;
		generators = 0;
		buildingsInQueue = 0;
		shipsInQueue = 0;
		newShipInQueue = 1;
		shipQueueNotEnoughMetal = 0;
		metal = 0;
		metalProduction = 0;
		energy = 0;
		energyProduction = 0;
		energyUsage = 0;
		food = 0;
		foodProduction = 0;
		foodUsage = (population / 1000000);	
		locationX = 0;
		locationY = 0;
		locationZ = 0;
		planetID.systemNumber = 0;
		planetID.planetNumber = 0;
		flagOwnedColony = 0;
		for(int i=0; i<4; i++)
		{
			buildingQueueType[i] = 0;
			buildingQueueCost[i] = 0;
			shipQueueType[i] = 0;
			shipQueueCost[i] = 0;
			shipsInOrbit[i] = 0;
		}	
	}

	colony::colony(int planetNumber, int systemNumber, int locationXin, int locationYin, int locationZin, int flagHomeworld)	//construct new colony
	//pre: colony isnt made
	//post: colony is made with 20,000 people
	{
		population = 20000;
		populationGrowthRate = 100;
		productionPts = (population / 1000);
		basicBuildings = 0;
		farms = 0;
		mines = 0;
		generators = 0;
		buildingsInQueue = 0;
		shipsInQueue = 0;
		newShipInQueue = 1;
		shipQueueNotEnoughMetal = 0;
		metal = 400;
		metalProduction = 0;
		energy = 50;
		energyProduction = 0;
		energyUsage = 2;
		food = 100;
		foodProduction = 0;
		foodUsage = (population / 1000000);	
		locationX = locationXin;
		locationY = locationYin;
		locationZ = locationZin;
		planetID.systemNumber = systemNumber;
		planetID.planetNumber = planetNumber;
		flagOwnedColony = 1;
		for(int i=0; i<4; i++)
		{
			buildingQueueType[i] = 0;
			buildingQueueCost[i] = 0;
			shipQueueType[i] = 0;
			shipQueueCost[i] = 0;
			shipsInOrbit[i] = 0;
		}
	}

	void colony::queueShip(int shipType)	//new ship queued
	//post: shipType has been entered into queue if queue isnt full
	{
		if (shipsInQueue < 4)
		{
			if (shipType == 1)	//destroyer, cost 100 production pts
			{
				shipQueueType[shipsInQueue] = 1;
				shipQueueCost[shipsInQueue] = 100;
			}
			else if (shipType == 2)	//colony ship, cost 200 production pts
			{
				shipQueueType[shipsInQueue] = 2;
				shipQueueCost[shipsInQueue] = 200;
			}
			shipsInQueue ++;
		}
	
	}
	
	colony::colony(int planetNumber, int systemNumber, int locationXin, int locationYin, int locationZin)
	{
		population = 0;
		populationGrowthRate = 0;
		productionPts = (population / 1000);
		basicBuildings = 0;
		farms = 0;
		mines = 0;
		generators = 0;
		buildingsInQueue = 0;
		shipsInQueue = 0;
		newShipInQueue = 1;
		shipQueueNotEnoughMetal = 0;
		metal = 0;
		metalProduction = 0;
		energy = 0;
		energyProduction = 0;
		energyUsage = 0;
		food = 0;
		foodProduction = 0;
		foodUsage = (population / 1000000);	
		locationX = locationXin;
		locationY = locationYin;
		locationZ = locationZin;
		planetID.systemNumber = systemNumber;
		planetID.planetNumber = planetNumber;
		flagOwnedColony = 0;
		for(int i=0; i<4; i++)
		{
			buildingQueueType[i] = 0;
			buildingQueueCost[i] = 0;
			shipQueueType[i] = 0;
			shipQueueCost[i] = 0;
			shipsInOrbit[i] = 0;
		}	
	}

	void colony::colonize()
	{
		flagOwnedColony = 1;
		population = 1000;
		populationGrowthRate = population * .005;
		metal = 400;
		energy = 50;
		energyUsage = 2;
		food = 100;
	}

	int colony::getPopulation()
	{
		return population;
	}

	void colony::buildBasicBuilding(int basicBuildingType)	//basic building queued
	//post: basicBuildingType has been entered into queue if queue isnt full
	//1 = farm
	//2 = mine
	//3 = generator
	{
		if ((buildingsInQueue < 4) && ((buildingsInQueue + basicBuildings) < 12))
		{
			if (basicBuildingType == 1) //farm, 50 production pts
			{
				buildingQueueType[buildingsInQueue] = 1;
				buildingQueueCost[buildingsInQueue] = 50;
			}
			else if (basicBuildingType == 2) //mine, 75 production pts
			{
				buildingQueueType[buildingsInQueue] = 2;
				buildingQueueCost[buildingsInQueue] = 75;
			}
			else if (basicBuildingType == 3) //generator, 75 production pts
			{
				buildingQueueType[buildingsInQueue] = 3;
				buildingQueueCost[buildingsInQueue] = 75;
			}
			buildingsInQueue ++;
		}
	}
	
	void colony::nextTurn()	//pass the turn
	//post: all turn sensitive variables are iterated to next turn
	{
		if ((newShipInQueue) && (metal < shipQueueCost[0]))
		{
			shipQueueNotEnoughMetal = 1;
		}
		else
		{
			if (shipQueueType[0] != 0)
			{
				if (newShipInQueue)
				{
					metal -= shipQueueCost[0];
					newShipInQueue = 0;
				}
				shipQueueCost[0] -= productionPts;
				if(shipQueueCost[0] <= 0)
				{
					ship* shipBorn = new ship(planetID.systemNumber, planetID.planetNumber, shipQueueType[0], locationX, locationY, locationZ);
					//ship shipBorn(planetID.systemNumber, planetID.planetNumber, 1, locationX, locationY, locationZ);
					shipList[IDticker] = shipBorn; //make a new ship
					IDticker++;

					shipsInOrbit[shipQueueType[0]-1]++;
					newShipInQueue = 1;
					for(int i=0; i<3; i++)
					{
						shipQueueType[i] = shipQueueType[i+1];
						shipQueueCost[i] = shipQueueCost[i+1];
					}
					shipQueueType[3] = 0;
					shipQueueCost[3] = 0;
					shipsInQueue --;
				}
			}
		}
		if (population < 60000)
			population += populationGrowthRate;

		populationGrowthRate = (population * .005);
		productionPts = (population / 1000);

		if (buildingQueueType[0] != 0)
		{
			buildingQueueCost[0] -= productionPts;
			if(buildingQueueCost[0] <= 0)
			{
				basicBuildings ++;
				switch (buildingQueueType[0]) {
					case 1:
						farms ++;
						break;
					case 2:
						mines ++;
						break;
					case 3:
						generators ++;
						break;
				}
				for(int j = 0; j<3; j++)
				{
					buildingQueueType[j] = buildingQueueType[j+1];
					buildingQueueCost[j] = buildingQueueCost[j+1];
				}
				buildingQueueType[3] = 0;
				buildingQueueCost[3] = 0;
				buildingsInQueue --;
			}
		}
		food -= (population / 1000);
		food += (farms * 10);
		energy -= 4;
		energy += (generators * 4);
		metal += (mines * 10);

	}

	void colony::display()	//display all colony info
	{
		cout<<"+++++++++++++++++++++++++++++++++++++++++++++="<<endl;
		cout<<"Population: "<<population<<" | "<<populationGrowthRate<<endl;
		cout<<"Production: "<<productionPts<<endl;
		cout<<"Food |"<<food<<"| +"<<(farms * 10)<<"| -"<<(population / 1000);
		if (((farms * 10) - (population / 1000)) <= 0)
			cout<<"| "<<((farms * 10) - (population / 1000))<<endl;
		else
			cout<<"| +"<<((farms * 10) - (population / 1000))<<endl;

		cout<<"Energy |"<<energy<<"| +"<<(generators * 4)<<"| -"<<(4);
		if (((generators * 4) - 4) <= 0)
			cout<<"| "<<((generators * 4) - 4)<<endl;
		else
			cout<<"| +"<<((generators * 4) - 4)<<endl;

		cout<<"Metal |"<<metal<<"| +"<<(mines * 10)<<endl;

		cout<<"Ship In Production: ";
		
		switch(shipQueueType[0]){
			case 0:
				cout<<"Nothing |";
				break;
			case 1:
				cout<<"Destroyer |";
				break;
			case 2:
				cout<<"Colony Ship |";
				break;
		}

		if(productionPts)
		cout<<(shipQueueCost[0]/productionPts)<<" turns"<<endl;
		if(shipQueueNotEnoughMetal)
			cout<<" **Not Enough Metal!**"<<endl;

		cout<<"Basic Building in Production: ";

		switch(buildingQueueType[0]){
			case 0:
				cout<<"Nothing |";
				break;
			case 1:
				cout<<"Farm |";
				break;
			case 2:
				cout<<"Mine |";
				break;
			case 3:
				cout<<"Generator |";
				break;
		}

		if(productionPts)
		cout<<(buildingQueueCost[0]/productionPts)<<" turns"<<endl;
		cout<<"Farms |"<<farms<<endl;
		cout<<"Mines |"<<mines<<endl;
		cout<<"Generators |"<<generators<<endl;
		cout<<"Ships In Orbit"<<endl;
		cout<<"--------------"<<endl;
		if(shipsInOrbit[0])
		{
			cout<<shipsInOrbit[0]<<" Colony Ship";
			if (shipsInOrbit[0] > 1)
				cout<<"s";
			cout<<endl;
		}
		if(shipsInOrbit[1])
		{
			cout<<shipsInOrbit[1]<<" Destroyer";
			if (shipsInOrbit[1] > 1)
				cout<<"s";
			cout<<endl;
		}
	}

	string colony::getDisplayString() // brian added
	{
		ostringstream oss;
	//	oss<<"+++++++++++++++++++++++++++++++++++++++++++++="<<endl;
		oss<<"Population: "<<population<<" | "<<populationGrowthRate<<endl;
		oss<<"Production: "<<productionPts<<endl;
		oss<<"Food |"<<food<<"| +"<<(farms * 10)<<"| -"<<(population / 1000);
		if (((farms * 10) - (population / 1000)) <= 0)
			oss<<"| "<<((farms * 10) - (population / 1000))<<endl;
		else
			oss<<"| +"<<((farms * 10) - (population / 1000))<<endl;

		oss<<"Energy |"<<energy<<"| +"<<(generators * 4)<<"| -"<<(4);
		if (((generators * 4) - 4) <= 0)
			oss<<"| "<<((generators * 4) - 4)<<endl;
		else
			oss<<"| +"<<((generators * 4) - 4)<<endl;

		oss<<"Metal |"<<metal<<"| +"<<(mines * 10)<<endl;

		oss<<"Ship In Production: ";
		
		switch(shipQueueType[0]){
			case 0:
				oss<<"Nothing |";
				break;
			case 1:
				oss<<"Destroyer |";
				break;
			case 2:
				oss<<"Colony Ship |";
				break;
		}

		if(productionPts)
			oss<<(shipQueueCost[0]/productionPts)<<" turns"<<endl;
		else
			oss<<"- turns"<<endl;

		if(shipQueueNotEnoughMetal)
			oss<<" **Not Enough Metal!**"<<endl;

		oss<<"Basic Building in Production: ";

		switch(buildingQueueType[0]){
			case 0:
				oss<<"Nothing |";
				break;
			case 1:
				oss<<"Farm |";
				break;
			case 2:
				oss<<"Mine |";
				break;
			case 3:
				oss<<"Generator |";
				break;
		}

		if(productionPts)
			oss<<(buildingQueueCost[0]/productionPts)<<" turns"<<endl;
		else
			oss<<"- turns"<<endl;
		oss<<"Farms |"<<farms<<endl;
		oss<<"Mines |"<<mines<<endl;
		oss<<"Generators |"<<generators<<endl;
		oss<<"Ships In Orbit"<<endl;
		oss<<"--------------"<<endl;
		if(shipsInOrbit[0])
		{
			oss<<shipsInOrbit[0]<<" Colony Ship";
			if (shipsInOrbit[0] > 1)
				oss<<"s";
			oss<<endl;
		}
		if(shipsInOrbit[1])
		{
			oss<<shipsInOrbit[1]<<" Destroyer";
			if (shipsInOrbit[1] > 1)
				oss<<"s";
			oss<<endl;
		}
		return oss.str();
	}
	string colony::getOverviewDisplayString()
	{
		ostringstream oss;
		if(flagOwnedColony)
		{
	//	oss<<"+++++++++++++++++++++++++++++++++++++++++++++="<<endl;
		oss<<"Pop: "<<population<<", ";
		oss<<"Prod: "<<productionPts<<endl;
		oss<<"Food:";
		if (((farms * 10) - (population / 1000)) <= 0)
			oss<<((farms * 10) - (population / 1000))<<", ";
		else
			oss<<"+"<<((farms * 10) - (population / 1000))<<", ";

		oss<<"Egy:";
		if (((generators * 4) - 4) <= 0)
			oss<<((generators * 4) - 4)<<endl;
		else
			oss<<"+"<<((generators * 4) - 4)<<endl;

		oss<<"Metal |"<<metal<<"| +"<<(mines * 10)<<endl;

		oss<<"Ships:\nProd.:";
		
		switch(shipQueueType[0]){
			case 0:
				oss<<"Nothing |";
				break;
			case 1:
				oss<<"Destroyer |";
				break;
			case 2:
				oss<<"Colony Ship |";
				break;
		}

		if(productionPts)
			oss<<(shipQueueCost[0]/productionPts)<<" turns"<<endl;
		else
			oss<<"- turns"<<endl;

		if(shipQueueNotEnoughMetal)
			oss<<" **Not Enough Metal!**"<<endl;

		oss<<"Orbit:";
		if(shipsInOrbit[0])
		{
			oss<<shipsInOrbit[0]<<" Colony Ship";
			if (shipsInOrbit[0] > 1)
				oss<<"s";
			oss<<endl;
		}
		if(shipsInOrbit[1])
		{
			oss<<shipsInOrbit[1]<<" Destroyer";
			if (shipsInOrbit[1] > 1)
				oss<<"s";
			oss<<endl;
		}
		}
		return oss.str();
	}
