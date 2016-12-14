#include "PythonFunctions.h"

#include "CModel.h"

void SetTriangleBudget(float triangles)
{
	CModelRepository::Instance()->SetTriangleBudget(triangles);
}
