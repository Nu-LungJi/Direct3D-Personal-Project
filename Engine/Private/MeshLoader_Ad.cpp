#include "MeshLoader_Ad.h"
MeshLoader_Ad::MeshLoader_Ad(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Component(_GRPDEV, _DEVCTX) {}
MeshLoader_Ad::MeshLoader_Ad(CONST MeshLoader_Ad& _PRTOBJ) : Component(_PRTOBJ), AnimationType(_PRTOBJ.AnimationType), MeshCount(_PRTOBJ.MeshCount),
MeshList(_PRTOBJ.MeshList), MaterialCount(_PRTOBJ.MaterialCount), MaterialList(_PRTOBJ.MaterialList), BoneList(_PRTOBJ.BoneList), BoneCount(_PRTOBJ.BoneCount),
AnimationList(_PRTOBJ.AnimationList), AnimationCount(_PRTOBJ.AnimationCount), AnimationLoop(_PRTOBJ.AnimationLoop), AnimationFrame(_PRTOBJ.AnimationFrame){ }
MeshLoader_Ad::~MeshLoader_Ad() {}

HRESULT MeshLoader_Ad::Initialize_ProtoType() {
	return S_OK;
}
HRESULT MeshLoader_Ad::Initialize(VOID* _ARG) {
	return S_OK;
}
VOID MeshLoader_Ad::Render_Mesh(uint32_t _MeshIndex) {
	MeshList[_MeshIndex]->Bind_Resources();
	MeshList[_MeshIndex]->Render_Buffer();
}
HRESULT MeshLoader_Ad::Play_Animation(_float _DT) {
	AnimationLoop = false;
	HRESULT HR = AnimationList[0]->Update_Animation(_DT, BoneList, AnimationLoop);

	for (auto& Bone : BoneList) {
		Bone->Update_CombinedBoneMatrix(BoneList);
	}
	return HR;	
}
HRESULT MeshLoader_Ad::Bind_BoneMatrix(shared_ptr<class Shader> _Shader, const string& _SRVName, uint32_t _MeshIndex) {
	return MeshList[_MeshIndex]->Bind_BoneMatrix(BoneList, _Shader, _SRVName);
}
HRESULT MeshLoader_Ad::Bind_Material(shared_ptr<class Shader> _Shader, const string& _SRVName, uint32_t _MeshIndex, aiTextureType _MatType, uint32_t _TexIndex) {
	return MaterialList[MeshList[_MeshIndex]->Get_MaterialIndex()]->Bind_ShaderResourceView(_Shader, _SRVName, _MatType, _TexIndex);
}
HRESULT	MeshLoader_Ad::Load_FBXModel(const wstring& _FilePath, ANIMATION_TYPE _AnimationType) {
	ModelFile_Path = _FilePath;							// FBX 파일명, 경로 포함
	ModelFile_Name = ModelFile_Path.filename();				// FBX 파일명, 경로 제외
	ModelFile_Directory = ModelFile_Path.parent_path();			// FBX 파일 저장되어 있는 폴더

	filesystem::path Binary_SaveFile = "../../Resource/Asset/BinaryAsset" / ModelFile_Name;

	AnimationType = _AnimationType;

	//if (Check_BinaryExist(Binary_SaveFile) == S_OK) {				// Binary 파일 유/무 확인
	//	Decompose_BinaryData(Binary_SaveFile, _AnimationType);		// Binary 파일이 있을 경우, Binary로 읽어들임.(성능 최적화)
	//
	//	return S_OK;
	//}

	uint32_t GraphicsFlags = 0;
	GraphicsFlags |= aiProcess_ConvertToLeftHanded;		// DirectX 왼손 좌표계 표준화
	GraphicsFlags |= aiProcess_PopulateArmatureData;		// 애니메이션 최적화(본-노드 사이의 연산 단순화)
	GraphicsFlags |= aiProcess_GlobalScale;				// Blender 편집 크기와 DirectX에서의 크기를 동기화
	GraphicsFlags |= aiProcess_OptimizeMeshes;			// 너무 잘게 쪼개진 메쉬 통합시켜 DrawCall 낮춤.
	GraphicsFlags |= aiProcess_ImproveCacheLocality;		// 캐시 히트율을 증가 시킴. (데이터 순서를 재배치)
	GraphicsFlags |= aiProcessPreset_TargetRealtime_Fast;	// 빠른 로딩이 필요한 최적화 옵션 모음.

	//GraphicsFlags |= aiProcessPreset_TargetRealtime_Quality;
	//GraphicsFlags |= aiProcessPreset_TargetRealtime_MaxQuality;

	Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);		// FBX 파일의 계층 구조를 원본 그대로 유지시킴.
	Importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 0.25f);			// 모델을 import할 때, 배율을 지정.
	if (AnimationType == ANIMATION_TYPE::ANIMATION)
		Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ANIMATIONS, true);	// 애니메이션 데이터를 읽어들일지 결정.

	Scene = Importer.ReadFile(ModelFile_Path.string().c_str(), GraphicsFlags);

	if (_AnimationType == ANIMATION_TYPE::ANIMATION) {
		if (FAILED(Ready_Bone(Scene->mRootNode, -1))) {
			MSG_BOX("Cannot Create Bone");
			return E_FAIL;
		}
	}
	
	if (FAILED(Ready_Mesh())) {
		MSG_BOX("Cannot Create Mesh");
		return E_FAIL;
	}
	if (FAILED(Ready_Material())) {
		MSG_BOX("Cannot Create Material");
		return E_FAIL;
	}
	if (_AnimationType == ANIMATION_TYPE::ANIMATION) {
		if (FAILED(Ready_Animation())) {
			MSG_BOX("Cannot Create Animation");
			return E_FAIL;
		}
	}
	//Decompose_Hierarchy(Scene->mRootNode);
	//
	//if (FAILED(Create_AnimaionList())) return E_FAIL;
	//
	//Compose_BinaryData(Binary_SaveFile);

	return S_OK;
}

HRESULT MeshLoader_Ad::Ready_Mesh() {
	MeshCount = Scene->mNumMeshes;

	for (uint32_t IDX = 0; IDX < MeshCount; ++IDX) {
		auto Mesh = Mesh_Ad::Create(GRPDEV, DEVCTX, AnimationType, this, Scene->mMeshes[IDX]);
		MeshList.push_back(Mesh);
	}
	return S_OK;
}
HRESULT MeshLoader_Ad::Ready_Material() {
	MaterialCount = Scene->mNumMaterials;

	for (uint32_t IDX = 0; IDX < MaterialCount; ++IDX) {
		auto Material = Material_Ad::Create(GRPDEV, DEVCTX, Scene->mMaterials[IDX], ModelFile_Path);
		MaterialList.push_back(Material);
	}
	return S_OK;
}
HRESULT MeshLoader_Ad::Ready_Bone(const aiNode* _Node, int32_t _ParentIndex) {
	auto Bone = Bone_Ad::Create(_Node, _ParentIndex);
	if (nullptr == Bone)	return E_FAIL;

	BoneList.push_back(Bone);

	int32_t ParentIndex = static_cast<int32_t>(BoneList.size() - 1);

	for (uint32_t IDX = 0; IDX < _Node->mNumChildren; ++IDX) {
		Ready_Bone(_Node->mChildren[IDX], ParentIndex);
	}
	return S_OK;
}

HRESULT MeshLoader_Ad::Ready_Animation() {
	AnimationCount = Scene->mNumAnimations;

	for (uint32_t IDX = 0; IDX < AnimationCount; ++IDX) {
		auto Animation = Animation_Ad::Create(Scene->mAnimations[IDX], this);	
		if (nullptr == Animation) return E_FAIL;

		AnimationList.push_back(Animation);
	}
	
	return S_OK;
}

int32_t MeshLoader_Ad::Get_ModelBoneIndex(const string& _BoneName) {
	int32_t BoneIndex = -1;
	auto iter = find_if(BoneList.begin(), BoneList.end(), [&](shared_ptr<Bone_Ad> Bone)->_bool {
		++BoneIndex;
		if(Bone->Get_BoneName() == _BoneName) return true;
		return false;
	});
	if (iter == BoneList.end()) return -1;

	return BoneIndex;
}

unique_ptr<MeshLoader_Ad>	MeshLoader_Ad::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<MeshLoader_Ad>(new MeshLoader_Ad(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create ModelLoader");
		return nullptr;
	}
	return Instance;
}
shared_ptr<Component>		MeshLoader_Ad::Clone(void* _ARG) {
	auto   Instance = shared_ptr<MeshLoader_Ad>(new MeshLoader_Ad(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone ModelLoader");
		return nullptr;
	}
	return Instance;
}