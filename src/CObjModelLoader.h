#ifndef COBJMODELLOADER_H
#define COBJMODELLOADER_H

#include "IModelLoader.h"

class CObjModelLoader: public IModelLoader
{
public:
	CObjModelLoader();
	~CObjModelLoader();

	virtual bool ReadModelFromFile(string filename); // should open the model with filename specified and process it and then close the file
	virtual void LoadModel(CCalModel& model); // copy the model to the CCalModel object

private:
	bool ReadDataFromFile(string filename); // read the data into m_pFileData
	void FreeMemory(); // free memory and set to NULL

	char* m_pFileData;
	unsigned long m_fileSize;

	CCalModel* m_pModel; 
};

#endif