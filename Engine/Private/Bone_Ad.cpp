#include "Bone_Ad.h"
Bone_Ad::Bone_Ad() {}
Bone_Ad::~Bone_Ad() {}

HRESULT Bone_Ad::Initialize(const aiNode* _Node, int32_t _ParentIndex) {
	
	BoneName = _Node->mName.C_Str();
	memcpy(&BoneMatrix, &_Node->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&BoneMatrix, XMMatrixTranspose(XMLoadFloat4x4(&BoneMatrix)));
	XMStoreFloat4x4(&CombinedBoneMatrix, XMMatrixIdentity());

	ParentBoneIndex = _ParentIndex;

	return S_OK;
}
VOID Bone_Ad::Update_CombinedBoneMatrix(const vector<shared_ptr<Bone_Ad>>& _BoneList) {
	if (-1 == ParentBoneIndex) {		// RootNode일 경우, BoneMatrix 그대로 저장 후, return
		XMStoreFloat4x4(&CombinedBoneMatrix, XMLoadFloat4x4(&BoneMatrix));
		return;
	}
	XMStoreFloat4x4(&CombinedBoneMatrix, XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&_BoneList[ParentBoneIndex]->CombinedBoneMatrix));
}

VOID Bone_Ad::Update_BoneMatrix(XMMATRIX _TransformedMatrix) {
	XMStoreFloat4x4(&BoneMatrix, _TransformedMatrix);
}

shared_ptr<Bone_Ad> Bone_Ad::Create(const aiNode* _Node, int32_t _ParentIndex) {
	auto Instance = shared_ptr<Bone_Ad>(new Bone_Ad());
	if (FAILED(Instance->Initialize(_Node, _ParentIndex))) {
		MSG_BOX("Cannot Create Bone");
		return nullptr;
	}
	return Instance;
}
