#include "CBaseReferenceFrame.h"

/*
CBaseReferenceFrame::CBaseReferenceFrame()
{
}
CBaseReferenceFrame::CBaseReferenceFrame(const CReferenceFrame& rhs)
{
	CopyFrom(rhs);
}
CBaseReferenceFrame::~CBaseReferenceFrame()
{
}
void CBaseReferenceFrame::CopyFrom(const CBaseReferenceFrame& rhs)
{
	if(this == &rhs)
		return;

	
}
CBaseReferenceFrame CBaseReferenceFrame::operator= (const CBaseReferenceFrame& rhs);
*/

CMatrix4x4 CReferenceFrame::GetRelativeMatrix(); // this function returns the world matrix of this reference frame relative to the currentFrameOfReference
void CReferenceFrame::SetRelativeMatrix(CMatrix4x4);

void CReferenceFrame::SelectFrameOfReference(const CReferenceFrame& frameOfReference)
{
	currentFrameOfReference = &frameOfReference;
}
CReferenceFrame* CBaseReferenceFrame::currentFrameOfReference;
