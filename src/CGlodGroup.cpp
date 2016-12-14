

#include "CGlodGroup.h"

GLuint CGlodGroup::glodGroupCounter = 0;

CGlodGroup::CGlodGroup()
{
	return;
	m_glodGroup = glodGroupCounter++;
	glodNewGroup(m_glodGroup);
	GLenum error = glodGetError();
	switch(error)
	{
	case GLOD_INVALID_NAME:
		{
			error = 0;
		}
	default:
		{
			error = 1;
		}
		break;
	}
}
CGlodGroup::~CGlodGroup()
{
	return;
	glodDeleteGroup(m_glodGroup);
}

void CGlodGroup::SetTriangleBudget(int triangles)
{
	return;
	glodGroupParameteri(m_glodGroup, GLOD_MAX_TRIANGLES, triangles);
}
float CGlodGroup::GetTriangleBudget()
{
	return 0;
	GLint triangles;
	glodGetGroupParameteriv(m_glodGroup, GLOD_MAX_TRIANGLES, &triangles); // maybe this function doesn't work yet?
	return triangles;
}
void CGlodGroup::AdaptGroup()
{
	return;
	glodGroupParameteri(m_glodGroup, GLOD_ADAPT_MODE, GLOD_TRIANGLE_BUDGET);
	glodAdaptGroup(m_glodGroup);
}
