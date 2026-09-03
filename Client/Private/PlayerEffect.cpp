#include "../Default/PCH.h"
#include "GameInstance.h"

PlayerEffect::PlayerEffect(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
PlayerEffect::PlayerEffect(CONST PlayerEffect& _PRTOBJ) : GameObject(_PRTOBJ), EffectTexture(_PRTOBJ.EffectTexture), NoiseTexture(_PRTOBJ.NoiseTexture), DissolveTexture(_PRTOBJ.DissolveTexture), 
EffectScrollSpeed(_PRTOBJ.EffectScrollSpeed), EffectAlpha(_PRTOBJ.EffectAlpha), EffectEmissiveColor(_PRTOBJ.EffectEmissiveColor), EffectEmissiveIntensity(_PRTOBJ.EffectEmissiveIntensity), 
DissolveScrollSpeed(_PRTOBJ.DissolveScrollSpeed), DissolveStrength(_PRTOBJ.DissolveStrength), DissolveEdgeWidth(_PRTOBJ.DissolveEdgeWidth), DissolveEdgeColor(_PRTOBJ.DissolveEdgeColor), 
EffectTextureOffset(_PRTOBJ.EffectTextureOffset), DissolveTextureOffset(_PRTOBJ.DissolveTextureOffset), EffectFadeOutValue(_PRTOBJ.EffectFadeOutValue), MaxLifeTime(_PRTOBJ.MaxLifeTime),
EffectScale(_PRTOBJ.EffectScale), EffectRotation(_PRTOBJ.EffectRotation), Component_Shader(_PRTOBJ.Component_Shader), Distance(_PRTOBJ.Distance), EffectOrbitRotation(_PRTOBJ.EffectOrbitRotation), EffectOffset(_PRTOBJ.EffectOffset) {}
PlayerEffect::~PlayerEffect() {}

HRESULT PlayerEffect::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform	= Add_Component<Transform>	(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Model		= Add_Component<MeshLoader>	(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Shader	= Add_Component<Shader>		(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_EFFECT);
	
	return S_OK;
}
HRESULT PlayerEffect::Initialize(VOID* _ARG) {
	Component_Model		= static_pointer_cast<MeshLoader>	(ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_MODEL)]);
	Component_Transform = static_pointer_cast<Transform>	(ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_TRANSFORM)]);

	GamePlayer			= static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());
	Component_Transform->Set_WorldPosition(GamePlayer->Get_TransformComponent()->Get_WorldPosition() + XMVectorSet(0.f, 0.f, 1.f, 0.f));

	EffectEmissiveColor = { 1.f ,0.2f, 0.2f };
	EffectEmissiveIntensity = 1.f;

	return S_OK;
}
VOID PlayerEffect::Update(CONST _float& _DT) {
	Update_TimeProgress(_DT);
	if (EffectOption == 1) {
		Descend_ScrollSpeed(_DT);
	}
}
VOID PlayerEffect::Update_EffectTransform() {
	XMVECTOR PlayerPosition = GamePlayer->Get_TransformComponent()->Get_WorldPosition();
	XMVECTOR PlayerRotation = GamePlayer->Get_TransformComponent()->Get_WorldRotationQuat();

	XMVECTOR HorizontalAxis = XMVector3Normalize(GamePlayer->Get_TransformComponent()->Get_WorldTransform(VECTOR_TYPE::VECTOR_UP));
	XMVECTOR VerticalAxis = XMVector3Normalize(GamePlayer->Get_TransformComponent()->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT));

	XMVECTOR HorizontalQuat = XMQuaternionRotationAxis(HorizontalAxis, XMConvertToRadians(EffectOrbitRotation.x));
	XMVECTOR VerticalQuat = XMQuaternionRotationAxis(VerticalAxis, XMConvertToRadians(EffectOrbitRotation.y));
	XMVECTOR ComBinedQuat = XMQuaternionMultiply(VerticalQuat, HorizontalQuat);

	XMVECTOR OrbitRotationQuat = XMQuaternionMultiply(PlayerRotation, ComBinedQuat);

	XMVECTOR RawOffset = XMLoadFloat3(&EffectOffset);
	XMVECTOR RotatedOffset = XMVector3Rotate(RawOffset, PlayerRotation);

	XMVECTOR FinalEffectPos = XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), OrbitRotationQuat);
	XMVECTOR SelfRotationQuat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(EffectRotation.x), XMConvertToRadians(EffectRotation.y), XMConvertToRadians(EffectRotation.z));

	Component_Transform->Set_WorldScale(EffectScale.x, EffectScale.y, EffectScale.z);
	Component_Transform->Set_WorldRotationQuat(XMQuaternionMultiply(SelfRotationQuat, OrbitRotationQuat));
	Component_Transform->Set_WorldPosition(PlayerPosition + FinalEffectPos * Distance + RotatedOffset);
}
VOID PlayerEffect::Update_TimeProgress(CONST _float& _DT) {
	TimeAccumulation += _DT;
	TimeProgress = TimeAccumulation / MaxLifeTime;
	TimeProgress = EASEOUT(TimeProgress, 2);

	if (TimeAccumulation >= MaxLifeTime) {
		TimeProgress = 0.f;
		TimeAccumulation = 0.f;
		if (EffectLoop == false) OBJDEAD = true;
	}
}
VOID PlayerEffect::Descend_ScrollSpeed(CONST _float& _DT) {
	if (EffectScrollSpeed.x > 0.f) {
		EffectScrollSpeed.x -= _DT;
	}
	else if (EffectScrollSpeed.x < 0.f) {
		EffectScrollSpeed.x += _DT;
	}
	else {
		EffectScrollSpeed.x = 0.f;
	}
	if (EffectScrollSpeed.y > 0.f) {
		EffectScrollSpeed.y -= _DT;
	}
	else if (EffectScrollSpeed.y < 0.f) {
		EffectScrollSpeed.y += _DT;
	}
	else {
		EffectScrollSpeed.y = 0.f;
	}
}
VOID PlayerEffect::Late_Update(CONST _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_EFFECT, shared_from_this());
}
HRESULT PlayerEffect::Render() {
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());

	if (FAILED(Component_Transform->Bine_ShaderResource(Component_Shader, "g_WorldMatrix")))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))							return E_FAIL;

	vector<shared_ptr<StaticMesh>>* MeshList = Component_Model->Get_StaticMeshList();

	if (nullptr != EffectTexture) {
		if (FAILED(Component_Shader->Bind_ShaderResourceView("EffectTexture", EffectTexture)))				return E_FAIL;
	}
	else {
		vector<Texture> TexVec = (*(*MeshList)[0]->Get_TextureList())[static_cast<uint32_t>(aiTextureType_DIFFUSE)];
		if (FAILED(Component_Shader->Bind_ShaderResourceView("EffectTexture", TexVec[0].TextureResource)))	return E_FAIL;
	}
	if (nullptr != DissolveTexture) {
		if (FAILED(Component_Shader->Bind_ShaderResourceView("DissolveTexture", DissolveTexture)))					return E_FAIL;
	}
	if (nullptr != NoiseTexture) {
		if (FAILED(Component_Shader->Bind_ShaderResourceView("NoiseTexture", NoiseTexture)))					return E_FAIL;
	}


	XMFLOAT2 ActualTScrollSpeed = (1.4f - TimeProgress) * EffectScrollSpeed;
	XMFLOAT2 ActualDScrollSpeed = (1.4f - TimeProgress) * DissolveScrollSpeed;
	_float ActualDStrength = (1.f - TimeProgress / 8.f) * DissolveStrength;

	if (FAILED(Component_Shader->Bind_RawValue("EffectScrollSpeed"		, &ActualTScrollSpeed, sizeof(XMFLOAT2))))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("EffectAlpha"			, &EffectAlpha, sizeof(_float))))				return E_FAIL;



	if (FAILED(Component_Shader->Bind_RawValue("EffectEmissiveColor"	, &EffectEmissiveColor, sizeof(XMFLOAT3))))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("EffectEmissiveIntensity", &EffectEmissiveIntensity, sizeof(_float))))	return E_FAIL;
	
	if (FAILED(Component_Shader->Bind_RawValue("DissolveScrollSpeed"	, &ActualDScrollSpeed, sizeof(XMFLOAT2))))		return E_FAIL;
	
	if (FAILED(Component_Shader->Bind_RawValue("DissolveStrength"		, &ActualDStrength, sizeof(_float))))			return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("DissolveEdgeWidth"		, &DissolveEdgeWidth, sizeof(_float))))			return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("DissolveEdgeColor"		, &DissolveEdgeColor, sizeof(XMFLOAT3))))		return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("NoiseScrollSpeed"		, &NoiseScrollSpeed, sizeof(XMFLOAT2))))		return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("TimeProgress"			, &TimeProgress, sizeof(_float))))				return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("TimeAccumulation"		, &TimeAccumulation, sizeof(_float))))			return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("EffectTextureOffset"	, &EffectTextureOffset, sizeof(XMFLOAT2))))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("DissolveTextureOffset"	, &DissolveTextureOffset, sizeof(XMFLOAT2))))	return E_FAIL;
	
	if (FAILED(Component_Shader->Bind_RawValue("EffectFadeOutValue"		, &EffectFadeOutValue, sizeof(_float))))		return E_FAIL;

	if (FAILED(Component_Shader->Shader_Begin(ShaderPassIndex)))	return E_FAIL;

	for (auto& Mesh : *MeshList)
		Mesh->Render_RawMesh();

	return S_OK;
}

VOID PlayerEffect::Allocate_EffectTexture(CONST wstring& _FilePath) {
	EffectTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(_FilePath);
}
VOID PlayerEffect::Allocate_NoiseTexture(CONST wstring& _FilePath) {
	NoiseTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(_FilePath);
}
VOID PlayerEffect::Allocate_DissolveTexture(CONST wstring& _FilePath) {
	DissolveTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(_FilePath);
}
VOID PlayerEffect::Load_FBXModel(CONST wstring& _FilePath) {
	ModelRenderFlag = true;

	Component_Model->Load_FBXModel(_FilePath, ANIMATION_TYPE::NON_ANIMATION);
	Component_Model->Update_BoundingBox(Component_Transform->Get_WorldMatrix());
}

unique_ptr<PlayerEffect>	PlayerEffect::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<PlayerEffect>(new PlayerEffect(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create PlayerEffect.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>		PlayerEffect::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<PlayerEffect>(new PlayerEffect(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone PlayerEffect.");
		return nullptr;
	}
	return Instance;
}