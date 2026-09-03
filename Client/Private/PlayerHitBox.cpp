#include "../Default/PCH.h"
#include "GameInstance.h"

PlayerHitBox::PlayerHitBox(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
PlayerHitBox::PlayerHitBox(CONST PlayerHitBox& _PRTOBJ) : GameObject(_PRTOBJ) {}
PlayerHitBox::~PlayerHitBox() { }

HRESULT	PlayerHitBox::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform = Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Shader	= Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER);
	Component_Collider	= Add_Component<Collider>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_COLLIDER);

	return S_OK;
}
HRESULT	PlayerHitBox::Initialize(VOID* _ARG) {
	Component_Transform = static_pointer_cast<Transform>	 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_TRANSFORM)]);
	Component_Collider	= static_pointer_cast<Collider>		 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_COLLIDER)]);
	Component_Shader	= static_pointer_cast<Shader>		 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER)]);

	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());

	Component_Collider->Create_ColliderBox(COLLIDER_TYPE::COLLIDER_OBB);
	Component_Collider->Register_TransformComponent(Component_Transform);
	Component_Collider->Set_ColliderOwner(shared_from_this());

	HitBoxCollider = static_pointer_cast<OBBCollider>(Component_Collider->Create_ColliderBox(COLLIDER_TYPE::COLLIDER_OBB));
	HitBoxCollider->Set_ColliderBoxVolume(XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(-1.f, -1.f, -1.f, 1.f));

	GameInstance::GetInstance().Get_CollisionManager()->Register_Collider(Component_Collider);

	Component_Collider->Set_ColliderActiveState(false);
	HitBoxActivateState = false;

	return S_OK;
}
VOID	PlayerHitBox::Update(CONST _float& _DT) {
	if (HitBoxActivateState == false) return;
	HitBoxCollider->Set_ColliderBoxVolume(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f));
	if (HitBoxDuration < HitBoxMaxDuration) {
		HitBoxDuration += _DT;
		if (HitBoxDuration > HitBoxAppearTimeOffset){
			if (HitBoxFlowType == FLOWTYPE::LINEAR) {
				HitBoxAccScale.z += _DT;
			}
			else {
				HitBox_CurrentScailingTime += _DT;
				if (HitBox_CurrentScailingTime >= HitBox_MaximumScailingTime) { HitBox_CurrentScailingTime = HitBox_MaximumScailingTime; }

				if (HitBoxFlowType == FLOWTYPE::LERP) {
					HitBoxAccScale = LERP(HitBoxStartScale, HitBoxEndScale, HitBox_CurrentScailingTime / HitBox_MaximumScailingTime);
				}
				else if (HitBoxFlowType == FLOWTYPE::SMOOTHSTEP) {
					HitBoxAccScale = LERP(HitBoxStartScale, HitBoxEndScale, SMOOTHSTEP(HitBox_CurrentScailingTime / HitBox_MaximumScailingTime));
				}
				else if (HitBoxFlowType == FLOWTYPE::EASEOUT) {
					HitBoxAccScale = LERP(HitBoxStartScale, HitBoxEndScale, EASEOUT(HitBox_CurrentScailingTime / HitBox_MaximumScailingTime, 1.5f));
				}
			}
			HitBoxCollider->Set_ColliderBoxVolume(XMLoadFloat3(&(HitBoxStartScale)), XMVectorSet(-HitBoxStartScale.x - HitBoxAccScale.x, -HitBoxStartScale.y - HitBoxAccScale.y, -HitBoxStartScale.z - HitBoxAccScale.z, 1.f));
		}
	}
	else {
		DeActivate_HitBox();
	}

	if (OverlapInit == false && DamageFontAppearCheck) {
		DamageFontTimeAcc += _DT;
	}
}
VOID	PlayerHitBox::Late_Update(CONST _float& _DT) {

}
HRESULT	PlayerHitBox::Render() {

	return S_OK;
}
VOID	PlayerHitBox::On_CollisionEnter(shared_ptr<GameObject> _ColliderOwner) {
	if (OverlapInit == false && _ColliderOwner->Get_ObjectTag() == "Monster_Knight") {
		OverlapInit = true;
	}
}
VOID	PlayerHitBox::On_CollisionStay(shared_ptr<GameObject> _ColliderOwner) {
	if (OverlapInit == false && _ColliderOwner->Get_ObjectTag() == "Monster_Knight" && _ColliderOwner->Get_DeadState() == false) {
		DamageFontAppearCheck = true;
		if (DamageFontTimeAcc >= DamageFontTimeOffset) {
			On_CollisionEnter(_ColliderOwner);
			_ColliderOwner->On_CollisionEnter(shared_from_this());
		}
	}
}
VOID	PlayerHitBox::On_CollisionExit(shared_ptr<GameObject> _ColliderOwner) {

}

VOID	PlayerHitBox::Activate_HitBox(XMVECTOR _Position, XMVECTOR _Rotation, XMVECTOR _StartScale, XMVECTOR _EndScale, _float _Duration, _float _AppearTimeOffset, _float _AfterDurationTime, _float _Damage, _float _DFTTimeOffset, FLOWTYPE _FTYPE) {
	
	HitBoxActivateState = true;
	HitBoxDamage		= static_cast<uint32_t>(_Damage);
	HitBoxDuration		= 0.f;
	HitBoxMaxDuration	= _AppearTimeOffset + _Duration + _AfterDurationTime;
	HitBoxAppearTimeOffset = _AppearTimeOffset;

	HitBox_CurrentScailingTime = 0.f;
	HitBox_MaximumScailingTime = _Duration;

	DamageFontTimeOffset = _AppearTimeOffset + _DFTTimeOffset;
	DamageFontAppearCheck = false;
	DamageFontTimeAcc = 0.f;

	HitBoxFlowType = _FTYPE;

	XMStoreFloat3(&HitBoxStartScale	, _StartScale);
	XMStoreFloat3(&HitBoxEndScale	, _EndScale	 );
	 
	HitBoxAccScale = { 0.f, 0.f, 0.f };

	Component_Transform->Set_WorldPosition(_Position + XMVectorSet(0.f, 2.f, 0.f, 0.f));
	XMVECTOR FinalQuat = XMQuaternionMultiply(XMQuaternionRotationRollPitchYawFromVector(_Rotation), GamePlayer->Get_TransformComponent()->Get_WorldRotationQuat());
	Component_Transform->Set_WorldRotationQuat(FinalQuat);
	Component_Collider->Set_ColliderActiveState(true);

	GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(shared_from_this());
}

VOID	PlayerHitBox::DeActivate_HitBox() {

	HitBoxActivateState			= false;
	HitBoxDamage				= 0;
	HitBoxDuration				= 0.f;
	HitBoxAppearTimeOffset		= 0.f;

	DamageFontTimeAcc			= 0.f;
	DamageFontTimeOffset		= 0.f;

	HitBoxFlowType				= FLOWTYPE::LINEAR;

	HitBoxStartScale			= { 0.f, 0.f, 0.f };
	HitBoxEndScale				= { 0.f, 0.f, 0.f };
	HitBoxAccScale				= { 0.f, 0.f, 0.f };

	HitBox_CurrentScailingTime  = 0.f;
	HitBox_MaximumScailingTime  = 0.f;

	DamageFontAppearCheck = false;
	OverlapInit = false;

	DamagedObjectList.clear();

	Component_Collider->Set_ColliderActiveState(false);

	GameInstance::GetInstance().Get_RayCaster()->UnRegister_Recipient(shared_from_this());
}

shared_ptr<GameObject> PlayerHitBox::Find_DamagedObject(const string& _Tag) {
	for (auto iter = DamagedObjectList.begin(); iter != DamagedObjectList.end();) {
		if (iter->get()->Get_ObjectTag() == _Tag) return *iter;
		iter++;
	}
	return nullptr;
}

unique_ptr<PlayerHitBox>	PlayerHitBox::Create(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<PlayerHitBox>(new PlayerHitBox(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Player.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>		PlayerHitBox::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<PlayerHitBox>(new PlayerHitBox(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Player.");
		return nullptr;
	}
	return Instance;
}