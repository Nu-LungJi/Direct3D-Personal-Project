#include "../Default/PCH.h"
#include "GameInstance.h"

BossEffect::BossEffect(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
BossEffect::BossEffect(CONST BossEffect& _PRTOBJ) : GameObject(_PRTOBJ), EffectTexture(_PRTOBJ.EffectTexture), NoiseTexture(_PRTOBJ.NoiseTexture), DissolveTexture(_PRTOBJ.DissolveTexture),
EffectScrollSpeed(_PRTOBJ.EffectScrollSpeed), EffectAlpha(_PRTOBJ.EffectAlpha), EffectEmissiveColor(_PRTOBJ.EffectEmissiveColor), EffectEmissiveIntensity(_PRTOBJ.EffectEmissiveIntensity), EffectTextureOffset(_PRTOBJ.EffectTextureOffset),
DissolveScrollSpeed(_PRTOBJ.DissolveScrollSpeed), DissolveStrength(_PRTOBJ.DissolveStrength), DissolveEdgeWidth(_PRTOBJ.DissolveEdgeWidth), DissolveEdgeColor(_PRTOBJ.DissolveEdgeColor),
DissolveTextureOffset(_PRTOBJ.DissolveTextureOffset), EffectFadeOutValue(_PRTOBJ.EffectFadeOutValue), MaxLifeTime(_PRTOBJ.MaxLifeTime), EffectScale(_PRTOBJ.EffectScale), EffectRotation(_PRTOBJ.EffectRotation),
Component_Shader(_PRTOBJ.Component_Shader), Distance(_PRTOBJ.Distance), EffectOrbitRotation(_PRTOBJ.EffectOrbitRotation), EffectOffset(_PRTOBJ.EffectOffset), ShaderPassNumb(_PRTOBJ.ShaderPassNumb){}

HRESULT BossEffect::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform = Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Model		= Add_Component<MeshLoader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Shader	= Add_Component<Shader>(GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_EFFECT);

	return S_OK;
}
HRESULT BossEffect::Initialize(VOID* _ARG) {
	Component_Model		= static_pointer_cast<MeshLoader>	(ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_MODEL)]);
	Component_Transform = static_pointer_cast<Transform>	(ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_TRANSFORM)]);
	
	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());
	Component_Transform->Set_WorldPosition(GamePlayer->Get_TransformComponent()->Get_WorldPosition() + XMVectorSet(0.f, 0.f, 1.f, 0.f));

	EffectEmissiveColor = { 50.f / 255.f , 35.f / 255.f, 100.f / 255.f };
	EffectEmissiveIntensity = 3.f;

	return S_OK;
}
VOID BossEffect::Update(CONST _float& _DT) {
	Update_TimeProgress(_DT);
	Update_SpecialEffect(_DT);
}
VOID BossEffect::Late_Update(CONST _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_EFFECT, shared_from_this());
}
HRESULT BossEffect::Render() {
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

	if (FAILED(Component_Shader->Bind_RawValue("EffectScrollSpeed", &ActualTScrollSpeed, sizeof(XMFLOAT2))))			return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("EffectAlpha", &EffectAlpha, sizeof(_float))))							return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("EffectEmissiveColor", &EffectEmissiveColor, sizeof(XMFLOAT3))))			return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("EffectEmissiveIntensity", &EffectEmissiveIntensity, sizeof(_float))))	return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("DissolveScrollSpeed", &ActualDScrollSpeed, sizeof(XMFLOAT2))))			return E_FAIL;
	
	if (FAILED(Component_Shader->Bind_RawValue("NoiseScrollSpeed", &NoiseScrollSpeed, sizeof(XMFLOAT2))))				return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("DissolveStrength", &ActualDStrength, sizeof(_float))))					return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("DissolveEdgeWidth", &DissolveEdgeWidth, sizeof(_float))))				return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("DissolveEdgeColor", &DissolveEdgeColor, sizeof(XMFLOAT3))))				return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("TimeProgress", &TimeProgress, sizeof(_float))))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("TimeAccumulation", &TimeAccumulation, sizeof(_float))))					return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("EffectTextureOffset", &EffectTextureOffset, sizeof(XMFLOAT2))))			return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("DissolveTextureOffset", &DissolveTextureOffset, sizeof(XMFLOAT2))))		return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("EffectFadeOutValue", &EffectFadeOutValue, sizeof(_float))))				return E_FAIL;

	if (FAILED(Component_Shader->Shader_Begin(ShaderPassNumb)))	return E_FAIL;

	for (auto& Mesh : *MeshList)
		Mesh->Render_RawMesh();

	return S_OK;
}
VOID BossEffect::Update_EffectTransform() {
	XMVECTOR OwnerPosition = EffectOwner->Get_TransformComponent()->Get_WorldPosition();
	XMVECTOR OwnerRotation = EffectOwner->Get_TransformComponent()->Get_WorldRotationQuat();

	XMVECTOR HorizontalAxis = XMVector3Normalize(EffectOwner->Get_TransformComponent()->Get_WorldTransform(VECTOR_TYPE::VECTOR_UP));
	XMVECTOR VerticalAxis = XMVector3Normalize(EffectOwner->Get_TransformComponent()->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT));

	XMVECTOR HorizontalQuat = XMQuaternionRotationAxis(HorizontalAxis, XMConvertToRadians(EffectOrbitRotation.x));
	XMVECTOR VerticalQuat = XMQuaternionRotationAxis(VerticalAxis, XMConvertToRadians(EffectOrbitRotation.y));
	XMVECTOR ComBinedQuat = XMQuaternionMultiply(VerticalQuat, HorizontalQuat);

	XMVECTOR OrbitRotationQuat = XMQuaternionMultiply(OwnerRotation, ComBinedQuat);

	XMVECTOR RawOffset = XMLoadFloat3(&EffectOffset);
	XMVECTOR RotatedOffset = XMVector3Rotate(RawOffset, OwnerRotation);

	XMVECTOR FinalEffectPos = XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), OrbitRotationQuat);
	XMVECTOR SelfRotationQuat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(EffectRotation.x), XMConvertToRadians(EffectRotation.y), XMConvertToRadians(EffectRotation.z));

	Component_Transform->Set_WorldScale(EffectScale.x, EffectScale.y, EffectScale.z);
	Component_Transform->Set_WorldRotationQuat(XMQuaternionMultiply(SelfRotationQuat, OrbitRotationQuat));
	Component_Transform->Set_WorldPosition(OwnerPosition + FinalEffectPos * Distance + RotatedOffset);

	if (BillBoardEnable) {
		XMMATRIX InvView = GameInstance::GetInstance().Get_MainCamera()->Get_InvViewMatrix();
		InvView.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		XMFLOAT3 ScaleVec = {}, TransformVec = {};

		XMStoreFloat3(&ScaleVec, Component_Transform->Get_WorldScale());
		XMStoreFloat3(&TransformVec, Component_Transform->Get_WorldPosition());

		XMMATRIX ScaleMat = XMMatrixScaling(ScaleVec.x, ScaleVec.y, ScaleVec.z);
		XMMATRIX TranslationMat = XMMatrixTranslation(TransformVec.x, TransformVec.y, TransformVec.z);
		XMMATRIX Rotation = XMMatrixRotationX(XMConvertToRadians(90.f));

		XMMATRIX WorldBillBoard = ScaleMat * Rotation * InvView * TranslationMat;
		Component_Transform->Set_WorldMatrix(WorldBillBoard);
	}
}
VOID BossEffect::Update_SpecialEffect(const _float& _DT) {
	if (OBJTAG == "EFF_SwordAura_Clone") {
		
		int32_t INDEX	 = EffectOwner->Get_ModelComponent()->Get_ModelBoneIndex("WeaponProp04");
		auto	BoneList = EffectOwner->Get_ModelComponent()->Get_ModelBoneList();
		
		XMVECTOR Position = BoneList[INDEX]->Get_ComBinedTransform().r[3] + EffectOwner->Get_TransformComponent()->Get_WorldPosition();
		Component_Transform->Set_WorldPosition(Position);

		//Update_EffectTransform();
	}
}
VOID	BossEffect::Update_TimeProgress(CONST _float& _DT) {
	TimeAccumulation += _DT;
	TimeProgress = TimeAccumulation / MaxLifeTime;
	if (ShaderPassNumb != 1) {
		TimeProgress = EASEOUT(TimeProgress, 2);
	}

	if (TimeAccumulation >= MaxLifeTime) {
		TimeProgress		= 0.f;
		TimeAccumulation	= 0.f;
		if (EffectLoop == false) OBJDEAD = true;
	}
}
VOID BossEffect::Allocate_EffectTexture(CONST wstring& _FilePath) {
	EffectTexture	= GameInstance::GetInstance().Get_TextureManager()->Load_Texture(_FilePath);
}
VOID BossEffect::Allocate_DissolveTexture(CONST wstring& _FilePath) {
	DissolveTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(_FilePath);
}
VOID BossEffect::Allocate_NoiseTexture(const wstring& _FilePath) {
	NoiseTexture	= GameInstance::GetInstance().Get_TextureManager()->Load_Texture(_FilePath);
}
VOID BossEffect::Load_FBXModel(CONST wstring& _FilePath) {
	ModelRenderFlag = true;

	Component_Model->Load_FBXModel(_FilePath, ANIMATION_TYPE::NON_ANIMATION);
	Component_Model->Update_BoundingBox(Component_Transform->Get_WorldMatrix());
}

unique_ptr<BossEffect>	BossEffect::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<BossEffect>(new BossEffect(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create BossEffect.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	BossEffect::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<BossEffect>(new BossEffect(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone BossEffect.");
		return nullptr;
	}
	return Instance;
}