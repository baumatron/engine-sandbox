/*
id
type
location (x,y,z)
location (id)
destination (id)

constructor
change destination
next turn
*/
#ifndef _ship_h_
#define _ship_h_
#include <iostream>
#include <string>

using namespace std;



class ship
{
public:

	ship(int buildLocationSystemNumber, int buildLocationPlanetNumber, int typeIN, int buildLocationX, int buildLocationY, int buildLocationZ);

	~ship() {};

	void changeDestination(int destinationSystemNumber, int destinationPlanetNumber);

	ship operator=(ship src);

	void display();

	string getDisplayString();

	void colonize();

	void nextTurn();

	struct{
		int systemNumber;
		int planetNumber;
	}	destinationID;

	struct{
		int systemNumber;
		int planetNumber;
	}	locationID;

private:
	int id;
	int type;
	int locationX;
	int locationY;
	int locationZ;
	int destLocationX;
	int destLocationY;
	int destLocationZ;
	int destETA;
	int shield;
	int hull;
	int speed;
	int gunDam;
	int gunAcc;
};

#endif