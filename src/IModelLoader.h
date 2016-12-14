

#ifndef IMODELLOADER_H
#define IMODELLOADER_H

#include "CCalModel.h"

#include <string>
using namespace std;

// an interface for model loader classes
class IModelLoader
{
public:
	virtual bool ReadModelFromFile(string filename) = 0; // should open the model with filename specified and process it and then close the file
	virtual void LoadModel(CCalModel& model) = 0; // copy the model to the CCalModel object
};


#endif