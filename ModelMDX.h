#pragma once

#include <vector>
#include "vgl.h"

typedef char CHAR;
typedef unsigned char BYTE; //   - 8bit unsigned integer
typedef unsigned short WORD; //  - 16bit unsigned integer
//typedef unsigned int DWORD; //  - 32bit unsigned integer // windows定义
typedef float FLOAT; //  - 32bit floating point number
typedef float FLOAT2[2]; // - 2-dimensional floating point vector (elements ordered as x, y)
typedef float FLOAT3[3]; // - 3-dimensional floating point vector (elements ordered as x, y, z or b, g, r)
typedef float FLOAT4[4];
typedef float MAT4[16];

struct Sequence     //NrOfSequences = ChunkSize / 132
{
	CHAR Name[80];

	DWORD IntervalStart;
	DWORD IntervalEnd;
	FLOAT MoveSpeed;
	DWORD Flags;                       //0 - Looping
									   //1 - NonLooping
	FLOAT Rarity;
	DWORD SyncPoint;

	FLOAT BoundsRadius;
	FLOAT3 MinimumExtent;
	FLOAT3 MaximumExtent;
};

typedef struct {
	GLuint vao;
	GLuint index;
	DWORD NrOfVertexPositions; // 顶点数
	DWORD TotalNrOfIndexes; // 面数 = TotalNrOfIndexes/3
	DWORD MaterialId; // 材质id

	GLuint groupOffset; // 缓存贴图对象
	GLuint groupSize; // 同上
	GLuint groupIndex; // 同上
} Geoset;

struct ScalingTrack
{
	DWORD Time;
	FLOAT3 Scaling;
	FLOAT3 InTan;
	FLOAT3 OutTan;
};

struct GeosetScaling
{
	DWORD KGSC;

	DWORD NrOfTracks;
	DWORD InterpolationType;          
	DWORD GlobalSequenceId;

	struct ScalingTrack *scalingTrack;
};

struct RotationTrack
{
	DWORD Time;
	FLOAT4 Rotation;
	FLOAT4 InTan;
	FLOAT4 OutTan;
};

struct GeosetRotation
{
	DWORD KGRT;

	DWORD NrOfTracks;
	DWORD InterpolationType;       
	DWORD GlobalSequenceId;

	struct RotationTrack *rotationTrack;
};

struct TranslationTrack
{
	DWORD Time;
	FLOAT3 Translation;
	FLOAT3 InTan;
	FLOAT3 OutTan;
};

struct GeosetTranslation
{
	DWORD KGTR;

	DWORD NrOfTracks;
	DWORD InterpolationType;        
	DWORD GlobalSequenceId;

	struct TranslationTrack *translationTrack;
};

struct Node
{
	DWORD InclusiveSize;

	CHAR Name[80];
	DWORD ObjectId;
	DWORD ParentId;
	DWORD Flags;                  

	GeosetTranslation *translation;
	GeosetRotation *rotation;
	GeosetScaling *scaling;

	Node *next;
	Node *parent;
	Node *child;
	Node *brother;

	MAT4 mat;

	Node()
	{
		next = NULL;
		parent = NULL;
		child = NULL;
		brother = NULL;
		translation = NULL;
		rotation = NULL;
		scaling = NULL;
	}
};

class ModelMDX
{
public:
	ModelMDX();
	~ModelMDX();

	void loadModel(const char* filename);
	void draw();
//private:
	Sequence *seqs;
	int numOfSeqs;
	std::vector<Geoset> geos;
	std::vector<GLuint> matTexId;
	std::vector<GLuint> texId;
	Node *nodes;
	FLOAT3 *pivotPoints;
	int numOfPivotPoint;

	void loadSEQS(FILE *handle);
	void loadGeosets(FILE *handle);
	void loadBONE(FILE *handle);
	void loadTex(FILE *handle);
	void loadMTLS(FILE *handle);
	Node* readNode(FILE *Handle);
	void addNode(Node *n);
	void buildNode(Node *n);
	void buildBone();
};

