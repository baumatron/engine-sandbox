/* colony class,
	population
	stardock production
	orbital production
	research
	infrastructure
	four basic buildings
	ship in production
		time left
		cost

	money
	metal
	energy
	food

	food use per turn
	energy use per turn
	metal mined per turn
	food farmed per turn
	energy generated per turn


	*/
#ifndef _colony_h_
#define _colony_h_

#include <iostream>


using namespace std;


class colony
{
public:

	colony();

	colony(int planetNumber, int systemNumber, int locationXin, int locationYin, int locationZin, int flagHomeworld);	//construct new colony
	//pre: colony isnt made
	//post: colony is made with 20,000 people
	colony(int planetNumber, int systemNumber, int locationXin, int locationYin, int locationZin);	//construct new colony

	~colony() {}	//deconstructor

	void queueShip(int shipType);	//new ship queued
	//post: shipType has been entered into queue if queue isnt full

	void colonize();

	int getPopulation();
	
	void buildBasicBuilding(int basicBuildingType);	//basic building queued
	//post: basicBuildingType has been entered into queue if queue isnt full
	
	void nextTurn();	//pass the turn
	//post: all turn sensitive variables are iterated to next turn

	void display();	//display all colony info

	string getDisplayString(); // brian added
	string getOverviewDisplayString();

	struct{
		int systemNumber;
		int planetNumber;
	}	planetID;

	int locationX;
	int locationY;
	int locationZ;
	bool flagOwnedColony;		//determines ownership

private:

	int population;				//total population
	int populationGrowthRate;	//population growth rate per turn
	int productionPts;			//population / 1000
	int basicBuildings;
	int farms;
	int mines;
	int generators;
	int buildingQueueType[4];	//array of building queue, building types
	int buildingQueueCost[4];	//array of building queue, production cost left
	int buildingsInQueue;
	int shipQueueType[4];		//array of the ship queue, ship types
	int shipQueueCost[4];		//array of the ship queue, production cost left
	int shipsInQueue;
	bool newShipInQueue;
	bool shipQueueNotEnoughMetal;
	int metal;					//stockpile of metal
	int metalProduction;		//metal mining per turn
	int energy;					//stockpile of energy
	int energyProduction;		//energy generated per turn
	int energyUsage;			//energy used per turn
	int food;					//stockpile of food
	int foodProduction;			//food produced per turn
	int foodUsage;				//food used per turn
	int shipsInOrbit[4];		//all ships in orbit, [0] = #of colony ships, [1] = #of destroyers
	
};

#endif

	
