#include "../Default/PCH.h"
#include "GameInstance.h"

RewardChest::RewardChest(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) { }
RewardChest::RewardChest(CONST RewardChest& _PRTOBJ) : GameObject(_PRTOBJ), Component_Container_Model(_PRTOBJ.Component_Container_Model){ }

HRESULT RewardChest::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform = Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_ChestLid_Model = Add_Component<MeshLoader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Container_Model = static_pointer_cast<MeshLoader>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL, nullptr));
	Component_Shader	= Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_STATIC_SHADER);
	Component_Collider	= Add_Component<Collider>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_COLLIDER);

	Component_Container_Model->Load_FBXModel(L"../../Resource/Asset/MapResource/RewardChest_Container.fbx", ANIMATION_TYPE::NON_ANIMATION);
	Component_ChestLid_Model ->Load_FBXModel(L"../../Resource/Asset/MapResource/RewardChest_Lid.fbx", ANIMATION_TYPE::NON_ANIMATION);

    return S_OK;
}
HRESULT RewardChest::Initialize(VOID* _ARG) {
	Component_Transform = static_pointer_cast<Transform> (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);
	Component_ChestLid_Model = static_pointer_cast<MeshLoader>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]);
	Component_Shader	= static_pointer_cast<Shader>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_STATIC_SHADER]);
	Component_Collider	= static_pointer_cast<Collider>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_COLLIDER]);

	Component_Collider->Set_AutomaticTransform(Component_ChestLid_Model, Component_Transform, COLLIDER_TYPE::COLLIDER_AABB);
	Component_Collider->Set_ColliderOwner(shared_from_this());
	GameInstance::GetInstance().Get_CollisionManager()->Register_Collider(Component_Collider);
	GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(shared_from_this());

	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());

	ChestScale		= { 1.5f, 1.5f, 1.5f };
	ChestRotation	= { 0.f, 75.f, 0.f };
	ChestPosition	= { 30.f, 3.f, -60.f };

	Component_Transform->Set_WorldScale(ChestScale.x, ChestScale.y, ChestScale.z);
	Component_Transform->Set_WorldRotationQuat(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(ChestRotation.x), XMConvertToRadians(ChestRotation.y), XMConvertToRadians(ChestRotation.z)));
	Component_Transform->Set_WorldPosition(ChestPosition.x, ChestPosition.y, ChestPosition.z);

	Component_ChestLid_Model->Update_BoundingBox(Component_Transform->Get_WorldMatrix());
	Component_Container_Model->Update_BoundingBox(Component_Transform->Get_WorldMatrix());

	NoiseTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(L"../../Resource/Asset/Effect/BossEffect/T_Tile_300192.png");

	EmissiveValue = 1.f;

    return S_OK;
}
VOID RewardChest::Update(const _float& _DT) {
	Component_Collider->Update(_DT);

	ContainerMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&ChestScale), XMVectorZero(), 
		XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(ChestRotation.y), 0.f), XMLoadFloat3(&ChestPosition));

	XMVECTOR HingeAxis = XMVectorSet(0.f, 1.35f, 1.1f, 0.f);
	XMMATRIX PureRotMat = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(ChestRotation.x), 0.f, 0.f));
	XMMATRIX ToTargetMat = XMMatrixTranslationFromVector(HingeAxis);

	XMMATRIX LidLocalMatrix = PureRotMat * ToTargetMat;
	LidMatrix = LidLocalMatrix * ContainerMatrix;

	if (Interactable) {
		//if		(EmissiveValue <= 1.f)	EmissiveValue += _DT * 50.f;
		if (ChestRotation.x <= 80.f)ChestRotation.x = ChestRotation.x += (80.f - ChestRotation.x) * _DT * 5.f;
		else if (fabsf(60.f - ChestRotation.x) < 0.01f) { 
			ChestRotation.x = 60.f;
		}
	}

	if (DissolveEnable == PLAY_TYPE::ONSTART) {
		DissolveEnable = PLAY_TYPE::ONPROGRESS;
	}
	else if (DissolveEnable == PLAY_TYPE::ONPROGRESS) {
		if (DissolveAmount >= 1.f) {
			DissolveAmount = 1.f;
			DissolveEnable = PLAY_TYPE::FINISHED;
		}
		DissolveAmount += _DT * DissolveSpeed;
	}
}
VOID RewardChest::Late_Update(const _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
	Component_Container_Model->Set_SubMeshRenderFlag(0, true);
}
HRESULT RewardChest::Render() {
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());

	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))							return E_FAIL;

	XMFLOAT4 DissolveColor = { 50.f / 255.f, 50.f / 255.f, 1.f, 1.f };
	if (FAILED(Component_Shader->Bind_RawValue("g_EmissiveIntensity", &EmissiveValue, sizeof(_float))))		return E_FAIL;
	if (DissolveAmount > 0.f && DissolveAmount <= 1.f) {
		if (FAILED(Component_Shader->Bind_ShaderResourceView("g_NoiseTexture", NoiseTexture)))				return E_FAIL;
		if (FAILED(Component_Shader->Bind_RawValue("DissolveColor", &DissolveColor, sizeof(XMFLOAT4))))		return E_FAIL;
		if (FAILED(Component_Shader->Bind_RawValue("DissolveAmount", &DissolveAmount, sizeof(_float))))		return E_FAIL;
	}
	{
		XMFLOAT4X4 LidWorldMatrix;
		XMStoreFloat4x4(&LidWorldMatrix, LidMatrix);

		if (FAILED(Component_Shader->Bind_Matrix("g_WorldMatrix", &LidWorldMatrix)))						return E_FAIL;

		Component_ChestLid_Model->Render_Mesh(Component_Shader, XMMatrixIdentity(), 0);
	}
	{
		XMFLOAT4X4 ContainerWorldMatrix;
		XMStoreFloat4x4(&ContainerWorldMatrix, ContainerMatrix);

		if (FAILED(Component_Shader->Bind_Matrix("g_WorldMatrix", &ContainerWorldMatrix)))				return E_FAIL;
		Component_Container_Model->Render_Mesh(Component_Shader, XMMatrixIdentity(), 0);
	}
	
    return S_OK;
}
VOID RewardChest::Open_RewardBox() {
	Interactable = true;
}
unique_ptr<RewardChest>	RewardChest::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<RewardChest>(new RewardChest(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create RewardChest.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	RewardChest::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<RewardChest>(new RewardChest(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone RewardChest.");
		return nullptr;
	}
	return Instance;
}