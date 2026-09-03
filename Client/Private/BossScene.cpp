#include "../Default/PCH.h"
#include "GameInstance.h"

BossScene::BossScene(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Scene(_GRPDEV, _DEVCTX) {}
BossScene::~BossScene() {}
HRESULT BossScene::Initialize() {

    return S_OK;
}

VOID BossScene::Update(const _float& _DT) {
    Scene::Update(_DT);

	if (KEY_HOLD(DIK_LCONTROL) && MOUSE_LBUTTON)		{ GameInstance::GetInstance().Get_RayCaster()->RayCast_Collider(); }

	if (KEY_HOLD(DIK_LSHIFT) && MOUSE_LBUTTON_PRESSED)	{ GameInstance::GetInstance().Get_Navigator()->Generate_NavMeshRay();		 }
	if (KEY_HOLD(DIK_LSHIFT) && MOUSE_RBUTTON_PRESSED)	{ GameInstance::GetInstance().Get_Navigator()->Eliminate_NavMeshCell();		 }
	if (KEY_HOLD(DIK_LSHIFT) && KEY_DOWN(DIK_L))		{ GameInstance::GetInstance().Get_Navigator()->Set_ControlLock();			 }
	if (KEY_DOWN(DIK_DELETE))							{ GameInstance::GetInstance().Get_RayCaster()->Delete_RayCastObject();		 }
	if (KEY_HOLD(DIK_LSHIFT) && KEY_DOWN(DIK_TAB))		{ GameInstance::GetInstance().Get_Navigator()->Select_NextRayCastedObject(); }
	if (KEY_DOWN(DIK_TAB))								{ GameInstance::GetInstance().Get_RayCaster()->Select_NextRayCastedObject(); }

	if (KEY_HOLD(DIK_LCONTROL) && KEY_HOLD(DIK_LSHIFT) && KEY_DOWN(DIK_F12)) {
		if (6 == MessageBoxA(hWnd, "Save WorldMap Data", "Save Data?", MB_YESNO)) {
			//GameInstance::GetInstance().Get_MapEditor()->Save_WorldMap_ObjectList("../../Resource/WorldMapData/BuildingData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_BUILDING);
			//GameInstance::GetInstance().Get_MapEditor()->Save_WorldMap_ObjectList("../../Resource/WorldMapData/MapObjectData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_MAPOBJECT);
			//GameInstance::GetInstance().Get_MapEditor()->Save_WorldMap_ObjectList("../../Resource/WorldMapData/LandScapeData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_LANDSCAPE);
			//GameInstance::GetInstance().Get_MapEditor()->Save_WorldMap_ObjectList("../../Resource/WorldMapData/FoliageData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_FOLIAGE);
			GameInstance::GetInstance().Get_MapEditor()->Save_WorldMap_ObjectList("../../Resource/WorldMapData/BossSceneData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_BOSS);

			MSG_BOX("Complete To Save");
		}
	}
	if (KEY_HOLD(DIK_LCONTROL) && KEY_HOLD(DIK_LSHIFT) && KEY_DOWN(DIK_F11)) {
		if (6 == MessageBoxA(hWnd, "Load WorldMap Data", "Load Data?", MB_YESNO)) {
			//GameInstance::GetInstance().Get_MapEditor()->Load_WorldMap_ObjectList("../../Resource/WorldMapData/BuildingData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_BUILDING);
			//GameInstance::GetInstance().Get_MapEditor()->Load_WorldMap_ObjectList("../../Resource/WorldMapData/MapObjectData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_MAPOBJECT);
			//GameInstance::GetInstance().Get_MapEditor()->Load_WorldMap_ObjectList("../../Resource/WorldMapData/LandScapeData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_LANDSCAPE);
			//GameInstance::GetInstance().Get_MapEditor()->Load_WorldMap_ObjectList("../../Resource/WorldMapData/FoliageData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_FOLIAGE);
			
			MSG_BOX("Complete To Load");
		}
	}

	filesystem::path FilePath = "../../Resource/NavigationData/Player_Navigation_Data.bin";
	if (KEY_HOLD(DIK_LCONTROL) && KEY_HOLD(DIK_LSHIFT) && KEY_DOWN(DIK_F10)) {
		if (6 == MessageBoxA(hWnd, "Save Navigation Data", "Save Data?", MB_YESNO)) {
			GameInstance::GetInstance().Get_Navigator()->Save_NavigationData(FilePath);
			MSG_BOX("Complete To Save");
		}
	}

	if (KEY_DOWN(DIK_GRAVE)) {
		GameInstance::GetInstance().Set_MainCamera(++CurrentCamera % GameInstance::GetInstance().Get_SceneOption().CurrentCameraCount);
	}
}

HRESULT BossScene::Register_ProtoType() {

	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_TRANSFORM		  , Transform	::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_BUFFER			  , Buffer		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_TEXTURE		  , TexBuffer	::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_RECT_BUFFER	  , RectBuffer	::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_TERRAIN		  , Terrain		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_MODEL			  , MeshLoader	::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_ANIMATOR		  , Animator	::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_CUBETEX		  , CubeBuffer	::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_NAVIGATION		  , NavMeshAgent::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_COLLIDER		  , Collider	::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER	  , Shader		::Create(GRPDEV, DEVCTX, L"../../Shader_VtxAnimMesh.hlsl" , VTXANIMMESH::Elements	, VTXANIMMESH::ElementsCount));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_STATIC_SHADER	  , Shader		::Create(GRPDEV, DEVCTX, L"../../Shader_VtxMesh.hlsl"	  , VTXMESH::Elements		, VTXMESH::ElementsCount));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_TERRAIN_SHADER	  , Shader		::Create(GRPDEV, DEVCTX, L"../../Shader_VtxNorTex.hlsl"	  , VTXNORTEX::Elements	, VTXNORTEX::ElementsCount));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_SKYSPHERE_SHADER , Shader		::Create(GRPDEV, DEVCTX, L"../../Shader_SkySphere.hlsl"	  , VTXMESH::Elements		, VTXMESH::ElementsCount));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_UI_SHADER		  , Shader		::Create(GRPDEV, DEVCTX, L"../../Shader_VtxPosTex.hlsl"	  , VTXTEX::Elements		, VTXTEX::ElementsCount));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_MESHINSTANCE	  , Shader		::Create(GRPDEV, DEVCTX, L"../../Shader_IVTX_Mesh.hlsl"	  , VTXINS::Elements		, VTXINS::ElementsCount));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_SCROLL			  , Shader		::Create(GRPDEV, DEVCTX, L"../../Shader_ScrollUI.hlsl"	  , VTXTEX::Elements		, VTXTEX::ElementsCount));
	Add_ProtoType_CurrentScene(COMPONENT_TYPE::COMPONENT_EFFECT			  , Shader		::Create(GRPDEV, DEVCTX, L"../../Shader_Effect.hlsl"	  , VTXTEX::Elements		, VTXTEX::ElementsCount));

	Add_ProtoType_CurrentScene("PRT_DynamicCamera"		, DynamicCamera		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_PlayerCamera"		, PlayerCamera		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_ActionCamera"		, ActionCamera		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_ActionCamera_Boss"	, ActionCamera_Boss	::Create(GRPDEV, DEVCTX));

	Add_ProtoType_CurrentScene("PRT_Player"				, Player			::Create(GRPDEV, DEVCTX));
	//Add_ProtoType_CurrentScene("PRT_Monster_Knight"	, Monster_Knight	::Create(GRPDEV, DEVCTX));
	//Add_ProtoType_CurrentScene("PRT_Monster_Void"		, Monster_Void		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_Monster_FinalBoss"	, Monster_FinalBoss	::Create(GRPDEV, DEVCTX));

	//Add_ProtoType_CurrentScene("PRT_Portal"				, Portal		::Create(GRPDEV, DEVCTX));
	//Add_ProtoType_CurrentScene("PRT_Bicon"				, Bicon			::Create(GRPDEV, DEVCTX));
	//Add_ProtoType_CurrentScene("PRT_RewardChest"		, RewardChest		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_BossCard"			, BossCard::Create(GRPDEV, DEVCTX));
	//Add_ProtoType_CurrentScene("PRT_Topography"			, Topography		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_SkySphere"			, SkySphere			::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_MainUI"				, MainUI			::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_UIObject"			, UIObject			::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_FontObject"			, FontObject		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_DamageFontUI"		, DamageFontUI		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_EventUI"			, EventUI			::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_POPUPUI"			, PopUpUI			::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_HitBoxPool"			, HitBoxPool		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_PlayerEffect"		, PlayerEffect		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_BossEffect"			, BossEffect		::Create(GRPDEV, DEVCTX));
	Add_ProtoType_CurrentScene("PRT_BossUI"				, BossUI			::Create(GRPDEV, DEVCTX));

    return S_OK;
}
HRESULT BossScene::Initialize_SceneObject() {
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_SkySphere", "SkySphere", nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_Player", "Player", nullptr);
	//Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_Topography", "Terrain", nullptr);

	//Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_Monster_Knight", "Monster_Knight", nullptr);
	//Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_Monster_Void"		, "Monster_Void"		, nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_Monster_FinalBoss", "Monster_FinalBoss"		, nullptr);

	//Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_Portal", "Portal", nullptr);
	//Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_Bicon", "Bicon", nullptr);
	//Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_RewardChest", "RewardChest", nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_BossCard", "BossCard", nullptr);
	
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_CAM, "PRT_DynamicCamera", "DynamicCamera", nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_CAM, "PRT_PlayerCamera", "PlayerCamera", nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_CAM, "PRT_ActionCamera", "ActionCamera", nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_CAM, "PRT_ActionCamera_Boss", "ActionCamera_Boss"	, nullptr);

	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_UI, "PRT_EventUI", "EventUI", nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_UI, "PRT_POPUPUI", "PopUpUI", nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_UI, "PRT_DamageFontUI", "DamageFontUI", nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_UI, "PRT_MainUI", "MainUI", nullptr);
	Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_UI, "PRT_BossUI", "BossUI", nullptr);

	//GameInstance::GetInstance().Get_MeshGenerator()->Set_LayerIndex(LANDSCAPE::BUILDING, static_cast<uint32_t>(LAYER_TYPE::LAYER_BUILDING));
	//GameInstance::GetInstance().Get_MeshGenerator()->Set_LayerIndex(LANDSCAPE::MAPOBJ, static_cast<uint32_t>(LAYER_TYPE::LAYER_MAPOBJECT));
	//GameInstance::GetInstance().Get_MeshGenerator()->Set_LayerIndex(LANDSCAPE::LANDSCAPE, static_cast<uint32_t>(LAYER_TYPE::LAYER_LANDSCAPE));
	//GameInstance::GetInstance().Get_MeshGenerator()->Set_LayerIndex(LANDSCAPE::FOLIAGE, static_cast<uint32_t>(LAYER_TYPE::LAYER_FOLIAGE));
	GameInstance::GetInstance().Get_MeshGenerator()->Set_LayerIndex(LANDSCAPE::BOSS, static_cast<uint32_t>(LAYER_TYPE::LAYER_BOSS));

	GameInstance::GetInstance().Get_LightManager()->Generate_Light(LIGHT_TYPE::DIRECTIONAL, _float4(1.f, 1.f, 1.f, 1.f), _float4(0.2f, 0.2f, 0.2f, 1.f), _float4(1.f, 1.f, 1.f, 1.f)
		, _float4(1.f, -1.f, 1.f, 0.f), _float4(10.f, 10.f, 10.f, 0.f), 0.f);
	//GameInstance::GetInstance().Get_LightManager()->Generate_Light(LIGHT_TYPE::POINT, _float4(1.f, 0.4f, 0.4f, 1.f), _float4(0.8f, 0.8f, 0.8f, 1.f), _float4(0.8f, 0.8f, 0.8f, 1.f), _float4(1.f, -1.f, 1.f, 0.f),
	//	_float4(0.f, 5.f, 0.f, 1.f), 20.f);

    return S_OK;
}
HRESULT BossScene::Initialize_MapObject() {
    //GameInstance::GetInstance().Get_MeshGenerator()->Import_FBXFiles("../../Resource/Asset/Building/", LANDSCAPE::BUILDING);
    //GameInstance::GetInstance().Get_MeshGenerator()->Import_FBXFiles("../../Resource/Asset/MapResource/", LANDSCAPE::MAPOBJ);
    //GameInstance::GetInstance().Get_MeshGenerator()->Import_FBXFiles("../../Resource/Asset/LandScape/", LANDSCAPE::LANDSCAPE);
    //GameInstance::GetInstance().Get_MeshGenerator()->Import_FBXFiles("../../Resource/Asset/Foliage/", LANDSCAPE::FOLIAGE);
	GameInstance::GetInstance().Get_MeshGenerator()->Import_FBXFiles("../../Resource/Asset/BossScene/", LANDSCAPE::BOSS);

	GameInstance::GetInstance().Get_MapEditor()->Load_WorldMap_ObjectList("../../Resource/WorldMapData/BossSceneData/", (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_BOSS);

    return S_OK;
}
shared_ptr<BossScene>   BossScene::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto	SceneInstance = shared_ptr<BossScene>(new BossScene(_GRPDEV, _DEVCTX));
    if (FAILED(SceneInstance->Initialize())) {
        MSG_BOX("Failed to Created : BossScene");
        return nullptr;
    }
    return SceneInstance;
}
shared_ptr<GameObject>  BossScene::Clone(VOID* _ARG) {
    return nullptr;
}
