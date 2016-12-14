//
//
//
//                MilkShape 3D 1.7.3 File Format Specification
//
//
//                  This specifcation is written in C style.
//
//
// The data structures are defined in the order as they appear in the .ms3d file.
//
//
//
//
//

//#include "IModelLoader.h"
#include "CSmartPointer.h"
#include "CModel.h"

class CMilkShapeModelLoader
{
public:
	CMilkShapeModelLoader();
	~CMilkShapeModelLoader();

	static CMilkShapeModelLoader* GetInstance();
	//virtual bool ReadModelFromFile(string filename); // should open the model with filename specified and process it and then close the file
	//virtual void LoadModel(CModel& model); // copy the model to the CModel object

	CSmartPointer<CCoreModel> LoadModel(string filename);
//	bool LoadModel(CModel& model, string filename);
private:
	bool ReadModelFromFile(string filename); // should open the model with filename specified and process it and then close the file
	void LoadModel(CSmartPointer<CCoreModel>& model); // copy the model to the CModel object

	bool ReadDataFromFile(string filename); // read the data into m_pFileData
	void FreeMemory(); // free memory and set to NULL

	char* m_pFileData;
	unsigned long m_fileSize;

	CSmartPointer<CCoreModel> m_pModel; 
	static CMilkShapeModelLoader* instance;
};
/*
	static CXMLParser* GetInstance();

	string GetTagData(ifstream& file, string tag, int number = 0);
	string GetTagData(string source, string tag, int number = 0);
private:
	CXMLParser();
	~CXMLParser();

	static CXMLParser* instance;*/

//extern CMilkShapeModelLoader MilkshapeModelLoader;


//
// max values
//
#define MAX_VERTICES    65534
#define MAX_TRIANGLES   65534
#define MAX_GROUPS      255
#define MAX_MATERIALS   128
#define MAX_JOINTS      128
#define MAX_KEYFRAMES   216     // increase when needed



//
// flags
//
#define SELECTED        1
#define HIDDEN          2
#define SELECTED2       4
#define DIRTY           8



//
// types
//
#ifndef byte
typedef unsigned char byte;
#endif // char

#ifndef word
typedef unsigned short word;
#endif // word


// force one char alignment
#pragma pack(1)

//
// First comes the header.
//
typedef struct
{
    char    id[10];                                     // always "MS3D000000"
    int     version;                                    // 4
} ms3d_header_t;

//
// Then comes the number of vertices
//
//word nNumVertices;

//
// Then comes nNumVertices * sizeof (ms3d_vertex_t)
//
typedef struct
{
    byte    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    float   vertex[3];                                  //
    byte    boneId;                                     // -1 = no bone
    byte    referenceCount;
} ms3d_vertex_t;

//
// number of triangles
//
//word nNumTriangles;

//
// nNumTriangles * sizeof (ms3d_triangle_t)
//
typedef struct
{
    word    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    word    vertexIndices[3];                           //
    float   vertexNormals[3][3];                        //
    float   s[3];                                       //
    float   t[3];                                       //
    byte    smoothingGroup;                             // 1 - 32
    byte    groupIndex;                                 //
} ms3d_triangle_t;

//
// number of groups
//
//word nNumGroups;

//
// nNumGroups * sizeof (ms3d_group_t)
//
typedef struct
{
    byte            flags;                              // SELECTED | HIDDEN
    byte            name[32];                           //
    word            numtriangles;                       //
    word*            triangleIndices/*[numtriangles]*/;      // the groups group the triangles
    byte            materialIndex;                      // -1 = no material
} ms3d_group_t;

//
// number of materials
//
//word nNumMaterials;

//
// nNumMaterials * sizeof (ms3d_material_t)
//
typedef struct
{
    char            name[32];                           //
    float           ambient[4];                         //
    float           diffuse[4];                         //
    float           specular[4];                        //
    float           emissive[4];                        //
    float           shininess;                          // 0.0f - 128.0f
    float           transparency;                       // 0.0f - 1.0f
    char            mode;                               // 0, 1, 2 is unused now
    char            texture[128];                        // texture.bmp
    char            alphamap[128];                       // alpha.bmp
} ms3d_material_t;

//
// save some keyframer data
//
//float fAnimationFPS;
//float fCurrentTime;
//int iTotalFrames;

//
// number of joints
//
//word nNumJoints;

//
// nNumJoints * sizeof (ms3d_joint_t)
//
//
typedef struct
{
    float           time;                               // time in seconds
    float           rotation[3];                        // x, y, z angles
} ms3d_keyframe_rot_t;

typedef struct
{
    float           time;                               // time in seconds
    float           position[3];                        // local position
} ms3d_keyframe_pos_t;

typedef struct
{
    byte            flags;                              // SELECTED | DIRTY
    byte            name[32];                           //
    byte            parentName[32];                     //
    float           rotation[3];                        // local reference matrix
    float           position[3];

    word            numKeyFramesRot;                    //
    word            numKeyFramesTrans;                  //

    ms3d_keyframe_rot_t keyFramesRot/*[numKeyFramesRot]*/;      // local animation matrices
    ms3d_keyframe_pos_t keyFramesTrans/*[numKeyFramesTrans]*/;  // local animation matrices
} ms3d_joint_t;

//
// subversion, stuff attached to file
//
//int subVersion											// must be 1

//int nNumGroupComments;

//
// nNumGroupComments * ms3d_comment_t
//
typedef struct
{
	int index;											// index of group, material or joint
	int commentLength;									// length of comment (terminating '\0' is not saved), "MC" has comment length of 2 (not 3)
	byte* comment/*[commentLength]*/;						// comment
} ms3d_comment_t;

//int nNumMaterialComments;

//
// nNumMaterialComments * ms3d_comment_t
//

//int nNumJointComments;

//
// nNumJointComments * ms3d_comment_t
//

//int nHasModelComment;

//
// nHasModelComment * ms3d_comment_t
//

#pragma pack()


//
// Mesh Transformation:
// 
// 0. Build the transformation matrices from the rotation and position
// 1. Multiply the vertices by the inverse of local reference matrix (lmatrix0)
// 2. then translate the result by (lmatrix0 * keyFramesTrans)
// 3. then multiply the result by (lmatrix0 * keyFramesRot)
//
// For normals skip step 2.
//
//
//
// NOTE:  this file format may change in future versions!
//
//
// - Mete Ciragan
//
