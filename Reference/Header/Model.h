#pragma once
#include "Engine_Define.h"
#include "Mesh.h"

class Model {
private:
	Model();
public:
	virtual ~Model();

public:
	VOID		Set_VertexBoneDataToDefault(ModelVertex& _VTX);
	VOID		Set_VertexBoneData(ModelVertex& _VTX, uint32_t _BoneID, _float _Weight);
	VOID		ExtractBoneWeightForVertices(vector<ModelVertex>& _Vertices, aiMesh* _Mesh, const aiScene* _Scene);
	Mesh		ProcessMesh(aiMesh* _Mesh, const aiScene* _Scene);

public:
	VOID		ProcessNode(aiNode* _Node, const aiScene* _Scene);

	map<string, BoneInfo>*	Get_BoneMap()		{ return &BoneMap;		}
	uint32_t				Get_BoneCounter()	{ return BoneCounter;	}


private:
	vector<ModelVertex> Vertices;
	vector<uint16_t>	Indices;
	vector<Texture>		Textures;

	map<string, BoneInfo>	BoneMap;
	uint32_t				BoneCounter = 0;
};

