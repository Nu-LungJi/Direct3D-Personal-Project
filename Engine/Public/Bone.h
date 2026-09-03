#pragma once
#include "Engine_Define.h"
#include "FB_Bone_generated.h"

BEGIN(Engine)
class ENGINE_DLL Bone {
private:
	Bone();
public:
	~Bone();

public:
	HRESULT		Initialize();
	VOID		Update_Bone(const vector<shared_ptr<Bone>>& _BoneList);

	HRESULT		Decompose_BoneData(const aiNode* _Node, int32_t _ParentBoneListIndex);
	HRESULT		Decompose_BoneData(const FB_Bone::BoneData* _BoneData);

	flatbuffers::Offset<FB_Bone::BoneData>	Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder);

public:
	VOID		Set_LocalTransform(XMMATRIX _TRS)		{ XMStoreFloat4x4(&LocalTransform, _TRS);	  }
	VOID		Set_LocalTransformTranslation(XMFLOAT3 _TRS);
	XMMATRIX	Get_LocalTransform()					{ return XMLoadFloat4x4(&LocalTransform);	  }

	VOID		Set_ComBinedTransform(XMMATRIX _TRS)	{ XMStoreFloat4x4(&CombinedBoneMatrix, _TRS); }
	VOID		Set_ComBinedTransformTranslation(XMFLOAT3 _TRS);
	XMMATRIX	Get_ComBinedTransform()					{ return XMLoadFloat4x4(&CombinedBoneMatrix); }

	VOID		Set_PrevComBinedTransform(XMMATRIX _TRS){ XMStoreFloat4x4(&PrevCombinedBoneMatrix, _TRS); }
	XMMATRIX	Get_PrevComBinedTransform()				{ return XMLoadFloat4x4(&PrevCombinedBoneMatrix); }

	int32_t		Get_ParentBoneIndex()					{ return ParentBoneIndex;	}
	string		Get_BoneName()							{ return BoneName;			}

	BoneTransform	Get_BindPoseTRS() { return BoneTransform{ BonePosition, BoneRotation, BoneScale }; }

	static		shared_ptr<Bone> Create();

private:
	XMFLOAT4X4			LocalTransform;
	XMFLOAT4X4			CombinedBoneMatrix;
	XMFLOAT4X4			PrevCombinedBoneMatrix;
	int32_t				ParentBoneIndex = { -1 };

	XMVECTOR			BonePosition;
	XMVECTOR			BoneRotation;
	XMVECTOR			BoneScale;

	string				BoneName;
};
END