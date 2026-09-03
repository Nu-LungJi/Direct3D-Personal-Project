#include "MeshLoader.h"
#include "GameInstance.h"

MeshLoader::MeshLoader(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Component(_GRPDEV, _DEVCTX) { }
MeshLoader::MeshLoader(CONST MeshLoader& _PRTOBJ) : Component(_PRTOBJ), StaticMeshList(_PRTOBJ.StaticMeshList), DynamicMeshList(_PRTOBJ.DynamicMeshList), InstanceMeshList(_PRTOBJ.InstanceMeshList),
ReferencedByBinary(_PRTOBJ.ReferencedByBinary), Scene(_PRTOBJ.Scene), AnimationList(_PRTOBJ.AnimationList), AnimationType(_PRTOBJ.AnimationType),
ModelBoneList(_PRTOBJ.ModelBoneList), MaxCoordVertex(_PRTOBJ.MaxCoordVertex), MinCoordVertex(_PRTOBJ.MinCoordVertex), Component_Animator(_PRTOBJ.Component_Animator),
ModelRootBone(_PRTOBJ.ModelRootBone), ModelPelvisBone(_PRTOBJ.ModelPelvisBone){
	SubMeshBoundingBoxList.resize(_PRTOBJ.SubMeshBoundingBoxList.size());
	RenderFlagList.resize(_PRTOBJ.RenderFlagList.size(), false);
}
MeshLoader::~MeshLoader() {  }

HRESULT	MeshLoader::Initialize_ProtoType() {
	MaxCoordVertex = { -9999999.f,-9999999.f ,-9999999.f };
	MinCoordVertex = { +9999999.f,+9999999.f ,+9999999.f };

    return S_OK;
}
HRESULT	MeshLoader::Initialize(VOID* _ARG) {

    return S_OK;
}
VOID	MeshLoader::Update_BoundingBox(XMMATRIX _ParentMatrix) {
	if		(AnimationType == ANIMATION_TYPE::INSTANCED) {
		uint32_t SubMesListSize = static_cast<uint32_t>(InstanceMeshList.size());
		SubMeshBoundingBoxList.resize(SubMesListSize);
		RenderFlagList.resize(SubMesListSize, false);
		for (uint32_t IDX = 0; IDX < SubMesListSize; ++IDX) {
			BoundingBox LocalBox = *InstanceMeshList[IDX]->Get_BoundingBox()->Get_ColliderBox().get();

			XMFLOAT3 LocalBoxVertex[8];
			LocalBox.GetCorners(LocalBoxVertex);

			XMVECTOR MinCoordVertex = { +FLT_MAX, +FLT_MAX, +FLT_MAX, 1.f };
			XMVECTOR MaxCoordVertex = { -FLT_MAX, -FLT_MAX, -FLT_MAX, 1.f };

			for (uint32_t i = 0; i < 8; ++i) {
				XMVECTOR WorldBoxVertex = XMVector3TransformCoord(XMLoadFloat3(&LocalBoxVertex[i]), _ParentMatrix);

				MinCoordVertex = XMVectorMin(MinCoordVertex, WorldBoxVertex);
				MaxCoordVertex = XMVectorMax(MaxCoordVertex, WorldBoxVertex);
			}

			BoundingBox::CreateFromPoints(SubMeshBoundingBoxList[IDX], MinCoordVertex, MaxCoordVertex);
		}
	}
	else if (AnimationType == ANIMATION_TYPE::NON_ANIMATION) {
		uint32_t SubMesListSize = static_cast<uint32_t>(StaticMeshList.size());
		SubMeshBoundingBoxList.resize(SubMesListSize);
		RenderFlagList.resize(SubMesListSize, false);
		for (uint32_t IDX = 0; IDX < SubMesListSize; ++IDX) {
			BoundingBox LocalBox = *StaticMeshList[IDX]->Get_BoundingBox()->Get_ColliderBox().get();

			XMFLOAT3 LocalBoxVertex[8];
			LocalBox.GetCorners(LocalBoxVertex);

			XMVECTOR MinCoordVertex = { +FLT_MAX, +FLT_MAX, +FLT_MAX, 1.f };
			XMVECTOR MaxCoordVertex = { -FLT_MAX, -FLT_MAX, -FLT_MAX, 1.f };

			for (uint32_t i = 0; i < 8; ++i) {
				XMVECTOR WorldBoxVertex = XMVector3TransformCoord(XMLoadFloat3(&LocalBoxVertex[i]), _ParentMatrix);

				MinCoordVertex = XMVectorMin(MinCoordVertex, WorldBoxVertex);
				MaxCoordVertex = XMVectorMax(MaxCoordVertex, WorldBoxVertex);
			}

			BoundingBox::CreateFromPoints(SubMeshBoundingBoxList[IDX], MinCoordVertex, MaxCoordVertex);
		}
	}
}

HRESULT MeshLoader::Load_FBXModel(CONST wstring& _FilePath, ANIMATION_TYPE _AnimationType, shared_ptr<Animator> _Animator) {
	       
	FBXFile_Path = _FilePath;							// FBX 파일명, 경로 포함
	FBXFile_Name = FBXFile_Path.filename();				// FBX 파일명, 경로 제외
	FBXFile_Directory = FBXFile_Path.parent_path();			// FBX 파일 저장되어 있는 폴더

	filesystem::path Binary_SaveFile = "../../Resource/Asset/BinaryAsset" / FBXFile_Name;

	if (nullptr != _Animator) {
		Component_Animator = _Animator;
	}

	AnimationType = _AnimationType;

	if (Check_BinaryExist(Binary_SaveFile) == S_OK) {			// Binary 파일 유/무 확인
		Decompose_BinaryData(Binary_SaveFile);					// Binary 파일이 있을 경우, Binary로 읽어들임.(성능 최적화)

		return S_OK;
	}

	uint32_t GraphicsFlags = 0;
	GraphicsFlags |= aiProcess_ConvertToLeftHanded;								// DirectX 왼손 좌표계 표준화
	GraphicsFlags |= aiProcess_PopulateArmatureData;							// 애니메이션 최적화(본-노드 사이의 연산 단순화)
	GraphicsFlags |= aiProcess_GlobalScale;										// Blender 편집 크기와 DirectX에서의 크기를 동기화
	GraphicsFlags |= aiProcess_OptimizeMeshes;									// 너무 잘게 쪼개진 메쉬 통합시켜 DrawCall 낮춤.
	GraphicsFlags |= aiProcess_ImproveCacheLocality;							// 캐시 히트율을 증가 시킴. (데이터 순서를 재배치)
	GraphicsFlags |= aiProcessPreset_TargetRealtime_Fast;						// 빠른 로딩이 필요한 최적화 옵션 모음.

	//GraphicsFlags |= aiProcessPreset_TargetRealtime_Quality;
	//GraphicsFlags |= aiProcessPreset_TargetRealtime_MaxQuality;

	Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);		// FBX 파일의 계층 구조를 원본 그대로 유지시킴.
	Importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 0.025f);		// 모델을 import할 때, 배율을 지정.
	if (AnimationType == ANIMATION_TYPE::ANIMATION)
		Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ANIMATIONS, true);	// 애니메이션 데이터를 읽어들일지 결정.

	Scene = Importer.ReadFile(FBXFile_Path.string().c_str(), GraphicsFlags);

	Decompose_Hierarchy(Scene->mRootNode);

	if (FAILED(Create_AnimaionList())) return E_FAIL;

	Compose_BinaryData(Binary_SaveFile);

	return S_OK;
}
VOID	MeshLoader::Render_Mesh(shared_ptr<Shader> _Shader, XMMATRIX _WorldMatrix, uint32_t _ShaderPass) {
	if (ReferencedByBinary == true) {
		if (AnimationType == ANIMATION_TYPE::NON_ANIMATION) {
			for (uint32_t IDX = 0; IDX < StaticMeshList.size(); ++IDX) {				// FBX Read
				if (RenderFlagList[IDX] == false) continue;
				XMFLOAT4	Position;

				XMStoreFloat4(&Position, GameInstance::GetInstance().Get_MainCamera()->Get_CameraPosition());

				_Shader->Bind_RawValue("g_vCamPosition", &Position, sizeof(XMFLOAT4));
				Bind_TextureResource(_Shader, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
				Bind_TextureResource(_Shader, "g_NormalTexture", aiTextureType_NORMALS, 0);
				if (StaticMeshList[IDX]->Get_TextureTypeList(aiTextureType_EMISSIVE).size() >= 1) {
					StaticMeshList[IDX]->Bind_TextureResource(_Shader, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0);
				}
				else {
					StaticMeshList[IDX]->UnBind_TextureResource(_Shader, "g_EmissiveTexture");
				}
				if (FAILED(_Shader->Shader_Begin(_ShaderPass)))	return;

				StaticMeshList[IDX]->Render_Mesh();

			}
		}
		else if (AnimationType == ANIMATION_TYPE::ANIMATION) {
			for (uint32_t IDX = 0; IDX < DynamicMeshList.size(); ++IDX) {				// FBX Read

				XMFLOAT4	Position;
				XMStoreFloat4(&Position, GameInstance::GetInstance().Get_MainCamera()->Get_CameraPosition());

				if (FAILED(_Shader->Bind_RawValue("g_vCamPosition", &Position, sizeof(XMFLOAT4))))		return;
				Bind_BoneResource(_Shader, "g_BoneMatrices", _WorldMatrix);
				Bind_TextureResource(_Shader, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
				Bind_TextureResource(_Shader, "g_NormalTexture", aiTextureType_NORMALS, 0);
				Bind_TextureResource(_Shader, "g_LightDiffuseTexture", aiTextureType_SHININESS, 0);
				if (DynamicMeshList[IDX]->Get_TextureTypeList(aiTextureType_EMISSIVE).size() >= 1) {
					DynamicMeshList[IDX]->Bind_TextureResource(_Shader, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0);
				}
				else {
					_Shader->UnBind_ShaderResourceView("g_EmissiveTexture");
				}
				if (FAILED(_Shader->Shader_Begin(_ShaderPass)))	return;

				DynamicMeshList[IDX]->Render_Mesh();
			}
		}
	}
	else if (ReferencedByBinary == false) {
		if (AnimationType == ANIMATION_TYPE::NON_ANIMATION) {
			for (uint32_t IDX = 0; IDX < StaticMeshList.size(); ++IDX) {				// FBX Read
				if (RenderFlagList[IDX] == false) continue;
				XMFLOAT4	Position;

				XMStoreFloat4(&Position, GameInstance::GetInstance().Get_MainCamera()->Get_CameraPosition());

				if (FAILED(_Shader->Bind_RawValue("g_vCamPosition", &Position, sizeof(XMFLOAT4))))		return;
				Bind_TextureResource(_Shader, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
				//Bind_TextureResource(_Shader, "g_NormalTexture", aiTextureType_NORMALS, 0);
				if (FAILED(_Shader->Shader_Begin(_ShaderPass)))	return;

				StaticMeshList[IDX]->Render_Mesh();

			}
		}
		else if (AnimationType == ANIMATION_TYPE::ANIMATION) {
			for (uint32_t IDX = 0; IDX < DynamicMeshList.size(); ++IDX) {				// FBX Read

				XMFLOAT4	Position;
				XMStoreFloat4(&Position, GameInstance::GetInstance().Get_MainCamera()->Get_CameraPosition());

				if (FAILED(_Shader->Bind_RawValue("g_vCamPosition", &Position, sizeof(XMFLOAT4))))		return;
				Bind_BoneResource(_Shader, "g_BoneMatrices", _WorldMatrix);
				Bind_TextureResource(_Shader, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
				Bind_TextureResource(_Shader, "g_NormalTexture", aiTextureType_NORMALS, 0);
				Bind_TextureResource(_Shader, "g_LightDiffuseTexture", aiTextureType_SHININESS, 0);
				if (DynamicMeshList[IDX]->Get_TextureTypeList(aiTextureType_EMISSIVE).size() >= 1) {
					DynamicMeshList[IDX]->Bind_TextureResource(_Shader, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0);
				}
				else {
					DynamicMeshList[IDX]->UnBind_TextureResource(_Shader, "g_EmissiveTexture");
				}

				if (FAILED(_Shader->Shader_Begin(_ShaderPass)))	return;

				DynamicMeshList[IDX]->Render_Mesh();
			}
		}
	}
}

HRESULT MeshLoader::Create_AnimaionList() {
	vector<aiMeshAnim*> AnimMeshList;
	if (AnimationType == ANIMATION_TYPE::ANIMATION) {
		for (uint32_t IDX = 0; IDX < Scene->mNumAnimations; ++IDX) {
			AnimationList.push_back(Scene->mAnimations[IDX]);
			for (uint32_t AIDX = 0; AIDX < Scene->mAnimations[IDX]->mNumMeshChannels; ++AIDX) {
				AnimMeshList.push_back(Scene->mAnimations[IDX]->mMeshChannels[AIDX]);
				string Name = Scene->mAnimations[IDX]->mMeshChannels[AIDX]->mName.C_Str();
			}
		}
		Component_Animator->Decompose_Animator(static_pointer_cast<MeshLoader>(shared_from_this()));
	}

	return S_OK;
}

HRESULT MeshLoader::Decompose_Hierarchy(aiNode* _Node) {
	if (AnimationType == ANIMATION_TYPE::ANIMATION) {
		Decompose_ModelBoneData(Scene->mRootNode, -1);
	}

	Decompose_NodeTree(_Node, XMMatrixIdentity());

	ModelRootBone = Find_ModelBoone("Root");
	ModelPelvisBone = Find_ModelBoone("Bip001");

	RenderFlagList.resize(StaticMeshList.size(), false);

	return S_OK;
}

HRESULT MeshLoader::Decompose_NodeTree(aiNode* _Node, XMMATRIX _ParentMatrix) {
	if (nullptr == _Node) return E_FAIL;

	XMMATRIX NodeMatrix = XMMatrixTranspose(XMMATRIX(&_Node->mTransformation.a1));
	XMMATRIX AccumulatedMatrix = NodeMatrix * _ParentMatrix;

	Decompose_MeshData(_Node, AccumulatedMatrix);

	for (uint32_t IDX = 0; IDX < _Node->mNumChildren; ++IDX) {
		Decompose_NodeTree(_Node->mChildren[IDX], AccumulatedMatrix);
	}

	return S_OK;
}

HRESULT MeshLoader::Bind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName, aiTextureType _TexType, uint32_t _TextureIndex) {
	
	if		(AnimationType == ANIMATION_TYPE::NON_ANIMATION) {
		for (auto& MeshObject : StaticMeshList)
			MeshObject->Bind_TextureResource(_Shader, _SRVName, _TexType, _TextureIndex);
	}
	else if (AnimationType == ANIMATION_TYPE::INSTANCED) {
		for (auto& MeshObject : InstanceMeshList)
			MeshObject->Bind_TextureResource(_Shader, _SRVName, _TexType, _TextureIndex);
	}
	else if (AnimationType == ANIMATION_TYPE::ANIMATION) {
		for (auto& MeshObject : DynamicMeshList)
			MeshObject->Bind_TextureResource(_Shader, _SRVName, _TexType, _TextureIndex);
	}

	return S_OK;
}


HRESULT MeshLoader::Bind_BoneResource(shared_ptr<Shader> _Shader, const string& _SRVName, XMMATRIX _WorldMatrix) {
	
	for (auto& MeshObject : DynamicMeshList)
		MeshObject->Bind_BoneMatrix(_Shader, _SRVName, ModelBoneList, _WorldMatrix);

	return S_OK;
}

HRESULT MeshLoader::Decompose_MeshData (aiNode* _Node, XMMATRIX _ParentMatrix) {
	
	wstring FileName = FBXFile_Name.wstring() + L"_";
	for (uint32_t IDX = 0; IDX < _Node->mNumMeshes; ++IDX) {
		aiMesh* MeshData = Scene->mMeshes[_Node->mMeshes[IDX]];
		
		if		(AnimationType == ANIMATION_TYPE::ANIMATION) {
			auto DynamicMesh	= DynamicMesh::Create(GRPDEV, DEVCTX);
			DynamicMesh->Decompose_MeshData(this, Scene, MeshData);
			if (nullptr == DynamicMesh)	return E_FAIL;
			
			DynamicMeshList.push_back(DynamicMesh);

		}
		else if (AnimationType == ANIMATION_TYPE::NON_ANIMATION){
			auto StaticMesh		= StaticMesh ::Create(GRPDEV, DEVCTX);
			if (FAILED(StaticMesh->Decompose_MeshData(this, Scene, MeshData))) {
				MSG_BOX("Cannot Decompose StaticMesh.");
				return E_FAIL;
			}
			StaticMeshList.push_back(StaticMesh);
		}
		else if (AnimationType == ANIMATION_TYPE::INSTANCED) {
			auto InstanceMesh = InstanceMesh::Create(GRPDEV, DEVCTX);
			if (FAILED(InstanceMesh->Decompose_MeshData(this, Scene, MeshData))) {
				MSG_BOX("Cannot Decompose InstanceMesh.");
				return E_FAIL;
			}
			InstanceMeshList.push_back(InstanceMesh);
		}
	}

	return S_OK;
}
HRESULT MeshLoader::Decompose_ModelBoneData(const aiNode* _Node, int32_t _ParentIndex) {
	auto Bone = Bone::Create();

	Bone->Decompose_BoneData(_Node, _ParentIndex);
	if (nullptr == Bone)	return E_FAIL;

	ModelBoneList.push_back(Bone);

	int32_t ParentIndex = static_cast<int32_t>(ModelBoneList.size() - 1);

	for (uint32_t IDX = 0; IDX < _Node->mNumChildren; ++IDX) {
		Decompose_ModelBoneData(_Node->mChildren[IDX], ParentIndex);
	}
	return S_OK;
}
VOID MeshLoader::Update_CoordVertex(XMFLOAT3 _MAX, XMFLOAT3 _MIN) {
	_MAX.x > MaxCoordVertex.x ? MaxCoordVertex.x = _MAX.x : MaxCoordVertex.x = MaxCoordVertex.x;
	_MAX.y > MaxCoordVertex.y ? MaxCoordVertex.y = _MAX.y : MaxCoordVertex.y = MaxCoordVertex.y;
	_MAX.z > MaxCoordVertex.z ? MaxCoordVertex.z = _MAX.z : MaxCoordVertex.z = MaxCoordVertex.z;

	_MIN.x < MinCoordVertex.x ? MinCoordVertex.x = _MIN.x : MinCoordVertex.x = MinCoordVertex.x;
	_MIN.y < MinCoordVertex.y ? MinCoordVertex.y = _MIN.y : MinCoordVertex.y = MinCoordVertex.y;
	_MIN.z < MinCoordVertex.z ? MinCoordVertex.z = _MIN.z : MinCoordVertex.z = MinCoordVertex.z;
}
int32_t MeshLoader::Get_ModelBoneIndex(const string& _BoneName) {
	int32_t BoneIndex = -1;
	auto iter = find_if(ModelBoneList.begin(), ModelBoneList.end(), [&](shared_ptr<Bone> Bone)->_bool {
		++BoneIndex;
		if (Bone->Get_BoneName() == _BoneName) return true;
		return false;
		});
	if (iter == ModelBoneList.end()) return -1;

	return BoneIndex;
}

shared_ptr<Bone> MeshLoader::Find_ModelBoone(const string& _BoneName) {
	for (uint32_t IDX = 0; IDX < ModelBoneList.size(); ++IDX) {
		if (_BoneName == ModelBoneList[IDX]->Get_BoneName()) return ModelBoneList[IDX];
	}
	return nullptr;
}

HRESULT MeshLoader::Compose_BinaryData  (filesystem::path& _SaveFilePath) {
	flatbuffers::FlatBufferBuilder Builder(1024 * 1024 * 32);
	filesystem::path FilePath(_SaveFilePath);
	FilePath = FilePath.replace_extension(".bin");

	flatbuffers::Offset <FB_Model::Model>			FB_Model;

	flatbuffers::Offset<FB_MeshLoader::MeshLoader>	FB_MeshLoader;
	flatbuffers::Offset<FB_Animator::Animator>		FB_Animator;

	flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<FB_StaticMesh::StaticMeshData>>>	FB_StaticMeshDataList	= NULL;
	flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<FB_DynamicMesh::DynamicMeshData>>>	FB_DynamicMeshDataList	= NULL;

	if (AnimationType == ANIMATION_TYPE::ANIMATION) {
		vector<flatbuffers::Offset<FB_DynamicMesh::DynamicMeshData>>	FB_DynamicMeshVector;
		vector<flatbuffers::Offset<FB_Bone::BoneData>>					FB_BoneVector;

		for (auto& DynamicMesh : DynamicMeshList) {
			FB_DynamicMeshVector.push_back(DynamicMesh->Compose_BinaryData(Builder));
		}

		for (auto& BoneData : ModelBoneList) {
			FB_BoneVector.push_back(BoneData->Compose_BinaryData(Builder));
		}

		FB_DynamicMeshDataList = Builder.CreateVector(FB_DynamicMeshVector.data(), FB_DynamicMeshVector.size());

		auto FB_BoneList = Builder.CreateVector(FB_BoneVector.data(), FB_BoneVector.size());

		FB_Utility::Vector3 FB_MaxCoordVertex = { MaxCoordVertex.x, MaxCoordVertex.y, MaxCoordVertex.z };
		FB_Utility::Vector3 FB_MinCoordVertex = { MinCoordVertex.x, MinCoordVertex.y, MinCoordVertex.z };

		FB_MeshLoader = FB_MeshLoader::CreateMeshLoader(Builder, FB_StaticMeshDataList, FB_DynamicMeshDataList, FB_BoneList, &FB_MaxCoordVertex, &FB_MinCoordVertex);
		if (nullptr != Component_Animator) {
			FB_Animator = Component_Animator->Compose_BinaryData(Builder);
		}
		FB_Model = FB_Model::CreateModel(Builder, FB_Animator, FB_MeshLoader);

		Builder.Finish(FB_Model);

		ofstream	File(FilePath, ios::binary);
		if (File.is_open()) {
			File.write(reinterpret_cast<const char*>(Builder.GetBufferPointer()), Builder.GetSize());
			File.close();
		}
		Builder.Clear();

		return S_OK;
	}
	else {
		vector<flatbuffers::Offset<FB_StaticMesh::StaticMeshData>>	FB_StaticMeshVector;

		if		(AnimationType == ANIMATION_TYPE::NON_ANIMATION) {
			for (auto& StaticMesh : StaticMeshList) {
				FB_StaticMeshVector.push_back(StaticMesh->Compose_BinaryData(Builder));
			}
		}
		else if (AnimationType == ANIMATION_TYPE::INSTANCED) {
			for (auto& InstanceMesh : InstanceMeshList) {
				FB_StaticMeshVector.push_back(InstanceMesh->Compose_BinaryData(Builder));
			}
		}

		FB_StaticMeshDataList = Builder.CreateVector(FB_StaticMeshVector.data(), FB_StaticMeshVector.size());

		FB_Utility::Vector3 FB_MaxCoordVertex = { MaxCoordVertex.x, MaxCoordVertex.y, MaxCoordVertex.z };
		FB_Utility::Vector3 FB_MinCoordVertex = { MinCoordVertex.x, MinCoordVertex.y, MinCoordVertex.z };

		FB_MeshLoader = FB_MeshLoader::CreateMeshLoader(Builder, FB_StaticMeshDataList, FB_DynamicMeshDataList, 0, &FB_MaxCoordVertex, &FB_MinCoordVertex);
	
		Builder.Finish(FB_MeshLoader);

		ofstream	File(FilePath, ios::binary);
		if (File.is_open()) {
			File.write(reinterpret_cast<const char*>(Builder.GetBufferPointer()), Builder.GetSize());
			File.close();
		}
		Builder.Clear();

		return S_OK;
	}
	return E_FAIL;
	
}
HRESULT MeshLoader::Decompose_BinaryData(filesystem::path& _SaveFilePath) {
	flatbuffers::FlatBufferBuilder Builder(1024 * 1024 * 32);
	filesystem::path FilePath(_SaveFilePath);
	filesystem::path BinaryFilePath = FilePath.replace_extension(".bin");

	ifstream Stream(BinaryFilePath, ios::binary | ios::ate);
	if (!Stream.is_open())	return E_FAIL;
	streamsize StreamSize = Stream.tellg();
	Stream.seekg(0, ios::beg);

	vector<char>	Buffer(StreamSize);
	if (!Stream.read(Buffer.data(), StreamSize))	return E_FAIL;

	if (AnimationType == ANIMATION_TYPE::ANIMATION) {
		auto FB_ModelData = FB_Model::GetModel(Buffer.data());
		auto FB_MeshLoader = FB_ModelData->Model_MeshLoader();

		Decompose_BoneBinaryData(FB_MeshLoader->BoneList());
		Decompose_MeshLoaderBinaryData(FB_MeshLoader);
		Component_Animator->Decompose_BinaryData(FB_ModelData->Model_Animator()->AnimationList(), static_pointer_cast<MeshLoader>(shared_from_this()));
	}
	else {
		auto FB_LoaderData = FB_MeshLoader::GetMeshLoader(Buffer.data());
		Decompose_MeshLoaderBinaryData(FB_LoaderData);
		
		RenderFlagList.resize(FB_LoaderData->StaticMeshList()->size(), false);
	}

	ModelRootBone	= Find_ModelBoone("Root");
	ModelPelvisBone = Find_ModelBoone("Bip001");

	return S_OK;
}

HRESULT MeshLoader::Decompose_MeshLoaderBinaryData(const FB_MeshLoader::MeshLoader* _MeshLoader) {

	const flatbuffers::Vector<flatbuffers::Offset<FB_StaticMesh::StaticMeshData>>*	 FB_StaticMeshList;
	const flatbuffers::Vector<flatbuffers::Offset<FB_DynamicMesh::DynamicMeshData>>* FB_DynamicMeshList;

	// StaticMesh
	if		(AnimationType == ANIMATION_TYPE::NON_ANIMATION) {
		FB_StaticMeshList = _MeshLoader->StaticMeshList();
		wstring FileName = FBXFile_Name.wstring() + L"_";
		for (uint32_t IDX = 0; IDX < FB_StaticMeshList->size(); ++IDX) {
			shared_ptr<StaticMesh> FB_StaticMesh = StaticMesh::Create(GRPDEV, DEVCTX);
			FB_StaticMesh->Decompose_MeshData(this, FB_StaticMeshList->Get(IDX));
			StaticMeshList.push_back(FB_StaticMesh);
		}
	}
	// DynamicMesh
	else if (AnimationType == ANIMATION_TYPE::ANIMATION) {
		FB_DynamicMeshList = _MeshLoader->DynamicMeshList();
		for (uint32_t IDX = 0; IDX < FB_DynamicMeshList->size(); ++IDX) {
			shared_ptr<DynamicMesh> FB_DynamicMesh = DynamicMesh::Create(GRPDEV, DEVCTX);
			FB_DynamicMesh->Decompose_MeshData(this, FB_DynamicMeshList->Get(IDX));
			DynamicMeshList.push_back(FB_DynamicMesh);
		}
	}
	// InstanceMesh
	else if (AnimationType == ANIMATION_TYPE::INSTANCED) {
		FB_StaticMeshList = _MeshLoader->StaticMeshList();
		for (uint32_t IDX = 0; IDX < FB_StaticMeshList->size(); ++IDX) {
			shared_ptr<InstanceMesh> FB_InstanceMesh = InstanceMesh::Create(GRPDEV, DEVCTX);
			FB_InstanceMesh->Decompose_MeshData(this, FB_StaticMeshList->Get(IDX));
			InstanceMeshList.push_back(FB_InstanceMesh);
		}
	}
	
	// MaxCoordVertex, MinCoordVertex
	{
		const FB_Utility::Vector3* FB_MaxCoord = _MeshLoader->MaxVertexCoord();
		const FB_Utility::Vector3* FB_MinCoord = _MeshLoader->MinVertexCoord();

		MaxCoordVertex = { FB_MaxCoord->x(), FB_MaxCoord->y(), FB_MaxCoord->z() };
		MinCoordVertex = { FB_MinCoord->x(), FB_MinCoord->y(), FB_MinCoord->z() };
	}
	
	return S_OK;
}

HRESULT MeshLoader::Decompose_BoneBinaryData(const flatbuffers::Vector<::flatbuffers::Offset<FB_Bone::BoneData>>* _BoneList) {
	for (uint32_t IDX = 0; IDX < _BoneList->size(); ++IDX) {
		shared_ptr<Bone> FB_Bone = Bone::Create();
		FB_Bone->Decompose_BoneData(_BoneList->Get(IDX));
		ModelBoneList.push_back(FB_Bone);
	}
	return S_OK;
}

HRESULT MeshLoader::Check_BinaryExist	(filesystem::path& _FilePath) {
	// 동일한 파일명의 Binary파일이 존재하는지 확인.
	filesystem::path FBXFilePath(_FilePath);
	// ****.fbx 확장자로 저장되어 있던 FilePath 문자열을 ****.bin 으로 교체
	filesystem::path BINFilePath = FBXFilePath;
	BINFilePath.replace_extension(".bin");

	if (filesystem::exists(BINFilePath)) { // 원본 FBX 파일 유/무 확인

		auto FBXFile_CurrentEditedTime = filesystem::last_write_time(FBXFile_Path);
		auto BINFile_CurrentEditedTime = filesystem::last_write_time(BINFilePath);
		
		// FBX파일과 파일명이 같은 .bin 파일이 같은 폴더에 있는지 탐색, 
		// 그리고 FBX파일이 최근에 수정되었을 경우, .bin 파일을 다시 새롭게 생성.
		if (filesystem::exists(BINFilePath) && FBXFile_CurrentEditedTime > BINFile_CurrentEditedTime) return E_FAIL;
		ReferencedByBinary = true;		// Binary 파일로 읽어들이는 방식으로 진행하면, Render시, 그에 맞는 MeshList로 그린다.
		return S_OK;
	}
	return E_FAIL;
}

vector<uint8_t> MeshLoader::Compose_EmbaddedTextures(Texture* _Tex) {

	ifstream	Stream(_Tex->TextureFilePath, ios::binary | ios::ate);
	streamsize	StreamSize = Stream.tellg();
	Stream.seekg(0, ios::beg);

	vector<uint8_t>	BinaryBuffer(StreamSize);
	if (Stream.read((char*)BinaryBuffer.data(), StreamSize)) {
		return BinaryBuffer;
	}
	return {};
}

unique_ptr<MeshLoader>	MeshLoader::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<MeshLoader>(new MeshLoader(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create ModelLoader");
		return nullptr;
	}
	return Instance;
}
shared_ptr<Component>	MeshLoader::Clone (VOID* _ARG) {
	auto   Instance = shared_ptr<MeshLoader>(new MeshLoader(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone ModelLoader");
		return nullptr;
	}
	return Instance;
}