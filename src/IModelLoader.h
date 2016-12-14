
#include "CModel.h"

#include <string>
using namespace std;

#ifndef IMODELLOADER_H
#define IMODELLOADER_H


// an interface for model loader classes
class IModelLoader
{
public:
	virtual bool ReadModelFromFile(string filename) = 0; // should open the model with filename specified and process it and then close the file
	virtual void LoadModel(CModel& model) = 0; // copy the model to the CModel object
};


#endif