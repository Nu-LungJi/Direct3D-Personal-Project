#include "Bone.h"

Bone::Bone() {};
Bone::~Bone() {}

HRESULT Bone::Initialize() {
	XMStoreFloat4x4(&CombinedBoneMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&PrevCombinedBoneMatrix, XMMatrixIdentity());

	return S_OK;
}
VOID Bone::Update_Bone(const vector<shared_ptr<Bone>>& _BoneList)	{
	
	if (-1 == ParentBoneIndex) {		// RootNode일 경우, BoneMatrix 그대로 저장 후, return
		XMStoreFloat4x4(&CombinedBoneMatrix, XMLoadFloat4x4(&LocalTransform));
		return;
	}
	XMStoreFloat4x4(&CombinedBoneMatrix, XMLoadFloat4x4(&LocalTransform) * XMLoadFloat4x4(&_BoneList[ParentBoneIndex]->CombinedBoneMatrix));
}

flatbuffers::Offset<FB_Bone::BoneData>	Bone::Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder) {

	return FB_Bone::CreateBoneData(_Builder, reinterpret_cast<const FB_Utility::Matrix4x4*>(&LocalTransform), 
		reinterpret_cast<const FB_Utility::Matrix4x4*>(&CombinedBoneMatrix), ParentBoneIndex, _Builder.CreateString(BoneName));
}

VOID Bone::Set_ComBinedTransformTranslation(XMFLOAT3 _TRS) {
	CombinedBoneMatrix._41 = _TRS.x; CombinedBoneMatrix._42 = _TRS.y; CombinedBoneMatrix._43 = _TRS.z;
}

VOID Bone::Set_LocalTransformTranslation(XMFLOAT3 _TRS) {
	LocalTransform._41 = _TRS.x; LocalTransform._42 = _TRS.y; LocalTransform._43 = _TRS.z;
}

HRESULT Bone::Decompose_BoneData(const aiNode* _Node, int32_t _ParentBoneListIndex)  {
	BoneName = _Node->mName.C_Str();
	memcpy(&LocalTransform, &_Node->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&LocalTransform, XMMatrixTranspose(XMLoadFloat4x4(&LocalTransform)));
	XMStoreFloat4x4(&CombinedBoneMatrix, XMMatrixIdentity());

	ParentBoneIndex = _ParentBoneListIndex;
	return S_OK;
}

HRESULT Bone::Decompose_BoneData(const FB_Bone::BoneData* _BoneData) {

	BoneName = _BoneData->BoneName()->c_str();
	ParentBoneIndex = _BoneData->ParentBoneIndex();

	memcpy(&LocalTransform, _BoneData->LocalTransform(), sizeof(XMFLOAT4X4));
	XMStoreFloat4x4(&CombinedBoneMatrix, XMMatrixIdentity());

	XMMatrixDecompose(&BoneScale, &BoneRotation, &BonePosition, XMLoadFloat4x4(&LocalTransform));

	return S_OK;
}
shared_ptr<Bone> Bone::Create() {
	auto Instance = shared_ptr<Bone>(new Bone());
	if (FAILED(Instance->Initialize())) {
		MSG_BOX("Cannot Create Bone");
		return nullptr;
	}
	return Instance;
}