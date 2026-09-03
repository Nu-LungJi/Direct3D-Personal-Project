#pragma once

#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL Bone_Ad {
private:
	Bone_Ad();
public:
	~Bone_Ad();

public:
	XMMATRIX		Get_CombinedBoneMatrix()	{ return XMLoadFloat4x4(&CombinedBoneMatrix); }
	const string&	Get_BoneName()				{ return BoneName; }	

public:
	HRESULT		Initialize(const aiNode* _Node, int32_t _ParentIndex);
	VOID		Update_CombinedBoneMatrix(const vector<shared_ptr<Bone_Ad>>& _BoneList);
	VOID		Update_BoneMatrix(XMMATRIX _TransformedMatrix);
	static  shared_ptr<Bone_Ad>   Create(const aiNode* _Node, int32_t _ParentIndex);
private:
	string		BoneName			= {		};
	_float4x4	BoneMatrix			= {		};
	_float4x4	CombinedBoneMatrix	= {		};
	int32_t		ParentBoneIndex		= {  -1	};
};
END