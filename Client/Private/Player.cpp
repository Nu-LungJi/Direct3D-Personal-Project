#include "../Default/PCH.h"

Player::Player(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {  }
Player::~Player()	{}

HRESULT Player::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform		= Add_Component<Transform>	 (CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Model			= Add_Component<MeshLoader>	 (CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Animator		= Add_Component<Animator>	 (CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_ANIMATOR);
	Component_Shader		= Add_Component<Shader>		 (CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER);
	Component_NavMeshAgent  = Add_Component<NavMeshAgent>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_NAVIGATION);
	Component_Collider		= Add_Component<Collider>	 (CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_COLLIDER);
	if		(CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_1)) {
		Component_NavMeshAgent->Load_NavigationData("../../Resource/NavigationData/NavigationData SCENE_1.bin");
	}
	else if (CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_2)) {
		Component_NavMeshAgent->Load_NavigationData("../../Resource/NavigationData/NavigationData SCENE_2.bin");
	}
	Component_Model->Load_FBXModel(L"../../Resource/Asset/Character/JangLi/JangLi.fbx", ANIMATION_TYPE::ANIMATION, Component_Animator);

	return S_OK;
}
HRESULT	Player::Initialize(VOID* _ARG) {
	Component_Transform		= static_pointer_cast<Transform>	 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_TRANSFORM		)]);
	Component_Model			= static_pointer_cast<MeshLoader>	 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_MODEL			)]);
	Component_Animator		= static_pointer_cast<Animator>		 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_ANIMATOR		)]);
	Component_Collider		= static_pointer_cast<Collider>		 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_COLLIDER		)]);
	Component_Shader		= static_pointer_cast<Shader>		 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER	)]);
	Component_NavMeshAgent	= static_pointer_cast<NavMeshAgent>	 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_NAVIGATION		)]);

	Component_Collider->Set_AutomaticTransform(Component_Model, Component_Transform, COLLIDER_TYPE::COLLIDER_OBB);
	Component_Collider->Set_ColliderOwner(shared_from_this());

	GameInstance::GetInstance().Get_CollisionManager()->Register_Collider(Component_Collider);
	GameInstance::GetInstance().Set_GamePlayer(shared_from_this());
	GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(shared_from_this());

	Component_Animator->Set_ComponentOwner(shared_from_this());
	Component_Animator->Register_TransformComponent(Component_Transform);
	Component_Animator->Register_NavMeshAgentComponent(Component_NavMeshAgent);
	Component_Animator->Set_MovementAmount(0.5f, 0.5f, 0.5f);

	StateMachine = PlayerStateMachine::Create(static_pointer_cast<Player>(shared_from_this()));
	StateMachine->FSM_StateChange(PLAYER_STATE::IDLE);

	shared_ptr<Scene> CurrentScene = GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene();
	CurrentScene->Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_HitBoxPool", "HitBoxPool", nullptr);
	PlayerHitBoxPool = static_pointer_cast<HitBoxPool>(CurrentScene->Get_GameObject("HitBoxPool"));
	
	Component_NavMeshAgent->Register_TransformComponent(Component_Transform);

	MatCapResource = GameInstance::GetInstance().Get_TextureManager()->Load_Texture("../../Resource/Asset/Texture/T_MatCap_360003.dds");

	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;
	if		(CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_1)) {
		Component_Transform->Set_WorldPosition(XMVectorSetW(Component_NavMeshAgent->Get_CellByIndex(1)->Get_CellVertex(NAVMESH_VERTEX::A), 1.f));
	}
	else if (CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_2)) {
		Component_Transform->Set_WorldPosition(-5.5f, 12.5f, -358.f);
	}
	Generate_EffectList();

	return S_OK;
}
VOID Player::Update(CONST _float& _DT) {
	Player_Controller(_DT);
	Component_Animator->Update_Animation(_DT);
	StateMachine->Update_StateMachine(_DT);
}
VOID Player::Late_Update(CONST _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
}
HRESULT	Player::Render() {
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());

	if (FAILED(Component_Transform->Bine_ShaderResource(Component_Shader, "g_WorldMatrix")))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))							return E_FAIL;

	if (FAILED(Component_Shader->Bind_ShaderResourceView("g_MatCapTexture", MatCapResource)))		return E_FAIL;

	Component_Model->Render_Mesh(Component_Shader);
	return S_OK;
}
VOID Player::On_CollisionEnter(shared_ptr<GameObject> _ColliderOwner) {

}
VOID Player::On_CollisionStay(shared_ptr<GameObject> _ColliderOwner) {

}
VOID Player::On_CollisionExit(shared_ptr<GameObject> _ColliderOwner) {

}
VOID Player::Player_Controller(const _float& _DT) {
	if (nullptr == PlayerActionCamera)	{ PlayerActionCamera = static_pointer_cast<ActionCamera>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("ActionCamera")); }
	if (nullptr == PlayerMainCamera)	{ PlayerMainCamera = static_pointer_cast<PlayerCamera>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("PlayerCamera")); }
	if (GameInstance::GetInstance().Get_MainCamera() == PlayerMainCamera && PlayerMainCamera->Get_MouseControlState()) {
		if (MOUSE_RBUTTON) {
			PLAYER_STATE PS = StateMachine->FSM_GetCurrentPlayerState();
			if (PS != PLAYER_STATE::BOOST && PS != PLAYER_STATE::ULTIMATE && PS != PLAYER_STATE::SKILL) {
				StateMachine->FSM_StateChange(PLAYER_STATE::DASH);
			}
		}
		if (MOUSE_LBUTTON) {
			PLAYER_STATE PS = StateMachine->FSM_GetCurrentPlayerState();
			if (PS != PLAYER_STATE::BOOST && PS != PLAYER_STATE::ULTIMATE && PS != PLAYER_STATE::SKILL) {
				if (AttackCombo >= 1) { AttackComboInitTime += _DT; }

				if (AttackComboInitTime >= 8.f)
					AttackCombo = 0;

				StateMachine->FSM_StateChange(PLAYER_STATE::ATTACK);

				HitBox_Controller(_DT);
			}
		}
		if (KEY_HOLD(DIK_W) && KEY_DOWN(DIK_SPACE)) {
			if		(JumpCount == 0)	JumpCount = 1;
			else if (JumpCount == 1)	JumpCount = 2;
			else if (JumpCount == 2)	JumpCount = 999;

			if (JumpCount == 1 || JumpCount == 2) {
				StateMachine->FSM_StateChange(PLAYER_STATE::JUMP);
			}
		}
		if (KEY_HOLD(DIK_S) && KEY_DOWN(DIK_SPACE)) {
			if		(JumpCount == 0)	JumpCount = 1;
			else if (JumpCount == 1)	JumpCount = 3;
			else if (JumpCount == 3)	JumpCount = 999;

			if (JumpCount == 1 || JumpCount == 3) {
				StateMachine->FSM_StateChange(PLAYER_STATE::JUMP);
			}
		}
		if (KEY_DOWN(DIK_SPACE)) {
			if		(JumpCount == 0)	JumpCount = 1;
			else if (JumpCount == 1)	JumpCount = 2;
			else if (JumpCount == 2)	JumpCount = 999;
			
			if (JumpCount == 1 || JumpCount == 2) {
				StateMachine->FSM_StateChange(PLAYER_STATE::JUMP);
			}
		}

		if (ObjectOnAir != static_cast<uint32_t>(JUMPSTATE::JUMPING) &&
			ObjectOnAir != static_cast<uint32_t>(JUMPSTATE::FALLING)) {
			if (KEY_DOWN(DIK_E)) StateMachine->FSM_StateChange(PLAYER_STATE::SKILL);
			if (KEY_DOWN(DIK_T)) StateMachine->FSM_StateChange(PLAYER_STATE::BOOST);
			if (KEY_DOWN(DIK_R)) StateMachine->FSM_StateChange(PLAYER_STATE::ULTIMATE);
		}
	}
}

VOID Player::Generate_EffectList() {
	shared_ptr<PlayerEffect>	EffectPrototype = { nullptr };
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();

	wstring UsualDissolveTexFilePath = L"../../Resource/Asset/Effect/T_Tile_230004.png";
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_AirBorne_SlenderSlash");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_AirBorne_SlenderSlash.fbx");
		//EffectPrototype->Allocate_EffectSourceTexture(L"../../Resource/Asset/Effect/T_Changli_25004_wings.png");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 0.f, 0.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.2f);
		EffectPrototype->Set_DissolveEdgeWidth(1.5f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_AirBorne_WideSlash");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_AirBorne_WideSlash.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 0.f, 0.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.2f);
		EffectPrototype->Set_DissolveEdgeWidth(1.5f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_BlueCurveTrail");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_BlueCurveTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 0.f, 0.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.2f);
		EffectPrototype->Set_DissolveEdgeWidth(1.5f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_BlueSlenderSpline");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_BlueSlenderSpline.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 0.f, 0.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.2f);
		EffectPrototype->Set_DissolveEdgeWidth(1.5f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Boost_CoreFeather_v1");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Boost_CoreFeather_v1.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Boost_InnerFlame_v1");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Boost_InnerFlame_v1.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Boost_OuterFlame_v1");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Boost_OuterFlame_v1.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.f, 0.f });  
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}

	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Boost_CoreFeather_v2");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Boost_CoreFeather_v2.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Boost_InnerFlame_v2");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Boost_InnerFlame_v2.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Boost_OuterFlame_v2");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Boost_OuterFlame_v2.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}

	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_PinkFlatTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_PinkFlatTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));


		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_RedCurveTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_RedCurveTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_PinkTrailTwoLine");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_PinkTrailTwoLine.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 0.f, 0.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_RedFlatTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_RedFlatTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Skill_CoreFlame");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Skill_CoreFlame.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Skill_OuterFlame");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Skill_OuterFlame.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Slam_PinkFlatTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Slam_PinkFlatTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_PinkFlatTrail_v2");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_PinkFlatTrail_v2.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_BlueFlatTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_BlueFlatTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_RedCurveTrail_VFlip");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_RedCurveTrail_VFlip.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_PinkCurveTrail_VFlip");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_PinkCurveTrail_VFlip.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	} 
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_RedCCurveTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_RedCCurveTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_PinkCCurveTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_PinkCCurveTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_PinkSlenderTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_PinkSlenderTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_RedSlenderTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_RedSlenderTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Skill_BackTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Skill_BackTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Skill_FrontTrail");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Skill_FrontTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}

	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Skill_Charging");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Skill_Charging.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Skill_GroundSlam");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Skill_GroundSlam.fbx");
		EffectPrototype->Allocate_NoiseTexture(L"../../Resource/Asset/Effect/T_Ring_30052.png");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Ariel_Ribbon_Air");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Ariel_Ribbon_Air.fbx");
		EffectPrototype->Allocate_DissolveTexture(L"../../Resource/Asset/Effect/T_Aura_230004.png");
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Ariel_Ribbon_ColorAir");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Ariel_Ribbon_ColorAir.fbx");
		EffectPrototype->Allocate_DissolveTexture(L"../../Resource/Asset/Effect/T_Aura_230004.png");
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = PlayerEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_Ariel_Ribbon_Feather");
		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/EFF_Ariel_Ribbon_Feather.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EmissiveOption({ 1.f, 1.f, 1.f }, 1.f);
		EffectPrototype->Set_EffectScrollSpeed({ -0.7f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -0.3f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 1.f, 0.6f, 0.6f });

		EffectPrototype->Set_EffectScale(XMFLOAT3(7.f, 7.f, 7.f));

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
}

VOID Player::HitBox_Controller(const _float& _DT) {
	XMVECTOR Zero = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	if		(AttackCombo == 1) {
		XMVECTOR Pos = Component_Transform->Get_WorldPosition() - 1.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK) 
			+ 0.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR FSca = XMVectorSet(2.f, 2.f, 1.5f, 1.f);
		XMVECTOR ESca = XMVectorSet(2.f, 2.f, 2.f, 1.f);
		PlayerHitBoxPool->Generate_HitBox(Pos, Zero, FSca, ESca, 0.6f, 0.4f, 0.25f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo == 2) {
		XMVECTOR Pos = Component_Transform->Get_WorldPosition() - 1.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 0.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR FSca = XMVectorSet(2.f, 2.f, 1.5f, 1.f);
		XMVECTOR ESca = XMVectorSet(2.f, 2.f, 2.f, 1.f);
		PlayerHitBoxPool->Generate_HitBox(Pos, Zero, FSca, ESca, 0.6f, 0.4f, 0.25f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo == 3) {
		XMVECTOR Pos = Component_Transform->Get_WorldPosition() - 1.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 0.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR FSca = XMVectorSet(2.f, 2.f, 1.5f, 1.f);
		XMVECTOR ESca = XMVectorSet(2.f, 2.f, 7.f, 1.f);

		PlayerHitBoxPool->Generate_HitBox(Pos, Zero, FSca, ESca, 0.75f, 0.f, 0.25f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);
		PlayerHitBoxPool->Generate_HitBox(Pos, Zero, FSca, ESca, 0.75f, 0.f, 0.25f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
		PlayerHitBoxPool->Generate_HitBox(Pos, Zero, FSca, ESca, 0.75f, 0.f, 0.25f, RANDOM(9999, 1000), 0.4f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo == 4) {
		XMVECTOR Pos = Component_Transform->Get_WorldPosition() - 1.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 0.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR FSca = XMVectorSet(2.f, 2.f, 0.f, 1.f);
		XMVECTOR ESca = XMVectorSet(2.f, 2.f, 7.f, 1.f);

		XMVECTOR InvRot = XMVectorSet(0.f, XMConvertToRadians(180.f), 0.f, 0.f);

		PlayerHitBoxPool->Generate_HitBox(Pos, InvRot, FSca, ESca, 0.75f, 0.f, 0.25f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);
		PlayerHitBoxPool->Generate_HitBox(Pos, InvRot, FSca, ESca, 0.75f, 0.f, 0.25f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo == 5) {
		XMVECTOR RightPos = Component_Transform->Get_WorldPosition() - 9.f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 2.f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR RightRot = XMVectorSet(0.f, XMConvertToRadians(-30.f), 0.f, 0.f);

		XMVECTOR LeftPos = Component_Transform->Get_WorldPosition() - 9.f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			- 2.f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR LeftRot = XMVectorSet(0.f, XMConvertToRadians(30.f), 0.f, 0.f);

		XMVECTOR CenterPos = Component_Transform->Get_WorldPosition() - 10.f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK);

		XMVECTOR FSca = XMVectorSet(0.3f, 0.3f,  0.f, 1.f);
		XMVECTOR ESca = XMVectorSet(0.3f, 0.3f,  4.5f, 1.f);

		PlayerHitBoxPool->Generate_HitBox(RightPos, RightRot, FSca, ESca, 0.25f, 0.4f, 0.4f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);
		PlayerHitBoxPool->Generate_HitBox(CenterPos, Zero, FSca, ESca, 0.25f, 0.5f, 0.4f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
		PlayerHitBoxPool->Generate_HitBox(LeftPos, LeftRot, FSca, ESca, 0.25f, 0.6f, 0.4f, RANDOM(9999, 1000), 0.4f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo == 7) {
		XMVECTOR Pos = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.85f, 0.f, 0.f) + 
			- 6.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 3.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR Rot = XMVectorSet(XMConvertToRadians(-10.f), XMConvertToRadians(100.f), 0.f, 0.f);

		XMVECTOR FSca = XMVectorSet(1.5f, 0.5f, 0.f, 1.f);
		XMVECTOR ESca = XMVectorSet(1.5f, 0.5f, 8.f, 1.f); 

		PlayerHitBoxPool->Generate_HitBox(Pos, Rot, FSca, ESca, 0.2f, 0.2f, 0.2f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo == 8) {
		XMVECTOR Pos = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.85f, 0.f, 0.f) +
			- 3.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			- 3.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR Rot = XMVectorSet(XMConvertToRadians(-10.f), XMConvertToRadians(-100.f), 0.f, 0.f);

		XMVECTOR FSca = XMVectorSet(1.5f, 0.5f, 0.f, 1.f);
		XMVECTOR ESca = XMVectorSet(1.5f, 0.5f, 6.f, 1.f);

		PlayerHitBoxPool->Generate_HitBox(Pos, Rot, FSca, ESca, 0.2f, 0.2f, 0.2f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo == 9) {
		XMVECTOR RightPos = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.85f, 0.f, 0.f) +
			+ 2.0f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 3.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR RightRot = XMVectorSet(XMConvertToRadians(5.f), 0.f, 0.f, 0.f);

		XMVECTOR RightFSca = XMVectorSet(1.5f, 0.5f, 0.f, 1.f);
		XMVECTOR RightESca = XMVectorSet(1.5f, 0.5f, 8.f, 1.f);

		PlayerHitBoxPool->Generate_HitBox(RightPos, RightRot, RightFSca, RightESca, 0.1f, 0.1f, 0.2f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);

		XMVECTOR UPPos = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.35f, 0.f, 0.f) +
			- 4.0f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 3.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		XMVECTOR UPRot = XMVectorSet(0.f, 0.f, XMConvertToRadians(-55.f), 0.f);

		XMVECTOR UPFSca = XMVectorSet(0.f, 0.5f, 1.5f, 1.f);
		XMVECTOR UPESca = XMVectorSet(8.f, 0.5f, 1.5f, 1.f);
		PlayerHitBoxPool->Generate_HitBox(UPPos, UPRot, UPFSca, UPESca, 0.3f, 0.2f, 0.2f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo == 10) {
		XMVECTOR Pos1 = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.65f, 0.f, 0.f) +
			- 3.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			- 2.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);

		XMVECTOR Rot1 = XMVectorSet(0.f, XMConvertToRadians(170.f), 0.f, 0.f);

		XMVECTOR FSca1 = XMVectorSet(1.25f, 0.5f, +0.f, 1.f);
		XMVECTOR ESca1 = XMVectorSet(1.25f, 0.5f, +10.f, 1.f);

		PlayerHitBoxPool->Generate_HitBox(Pos1, Rot1, FSca1, ESca1, 0.1f, 0.1f, 0.2f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
		XMVECTOR Pos2 = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.65f, 0.f, 0.f) +
			+ 6.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			- 2.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);

		XMVECTOR Rot2 = XMVectorSet(0.f, XMConvertToRadians(-100.f), 0.f, 0.f);

		XMVECTOR FSca2 = XMVectorSet(1.f, 0.5f, +0.f, 1.f);
		XMVECTOR ESca2 = XMVectorSet(1.f, 0.5f, +8.f, 1.f);

		PlayerHitBoxPool->Generate_HitBox(Pos2, Rot2, FSca2, ESca2, 0.2f, 0.25f, 0.2f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo == 11) {
		XMVECTOR Pos1 = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.5f, 0.f, 0.f)
			- 0.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 2.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);

		XMVECTOR Rot1 = XMVectorSet(XMConvertToRadians(30.f), 0.f, 0.f, 0.f);

		XMVECTOR FSca1 = XMVectorSet(3.5f, 0.25f, +0.f, 1.f);
		XMVECTOR ESca1 = XMVectorSet(3.5f, 0.25f, +3.f, 1.f);

		PlayerHitBoxPool->Generate_HitBox(Pos1, Rot1, FSca1, ESca1, 0.1f, 0.1f, 0.2f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
		XMVECTOR Pos2 = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.5f, 0.f, 0.f)
			- 3.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 2.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);

		XMVECTOR Rot2 = XMVectorSet(XMConvertToRadians(45.f), 0.f, 0.f, 0.f);

		XMVECTOR FSca2 = XMVectorSet(3.5f, 0.25f, +0.f, 1.f);
		XMVECTOR ESca2 = XMVectorSet(3.5f, 0.25f, +5.f, 1.f);

		PlayerHitBoxPool->Generate_HitBox(Pos2, Rot2, FSca2, ESca2, 0.1f, 0.2f, 0.2f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
	}
	else if (AttackCombo >= 12) {
		XMVECTOR Pos1 = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.5f, 0.f, 0.f)
			+ 0.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);

		XMVECTOR Rot1 = XMVectorSet(XMConvertToRadians(270.f), 0.f, 0.f, 0.f);

		XMVECTOR FSca1 = XMVectorSet(1.5f, 1.5f, +0.f, 1.f);
		XMVECTOR ESca1 = XMVectorSet(1.5f, 1.5f, 10.f, 1.f);

		PlayerHitBoxPool->Generate_HitBox(Pos1, Rot1, FSca1, ESca1, 0.2f, 0.7f, 0.2f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);

		_float NextHeight = 0.f, CellHeight = 0.f;
		XMVECTOR Pos2 = Component_Transform->Get_WorldPosition()
			- 1.5f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
			+ 3.25f * Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT);
		Component_NavMeshAgent->Evaluate_NextPositionCell(Pos2, &NextHeight, &CellHeight, _DT);

		XMVECTOR Rot2 = XMVectorSet(XMConvertToRadians(90.f), 0.f, 0.f, 0.f);
		
		XMVECTOR FSca2 = XMVectorSet(7.5f, 7.5f, +0.5f, 1.f);
		XMVECTOR ESca2 = XMVectorSet(7.5f, 7.5f, +0.5f, 1.f);
		
		PlayerHitBoxPool->Generate_HitBox(XMVectorSetY(Pos2, CellHeight - 1.5f), Rot2, FSca2, ESca2, 0.2f, 0.9f, 0.2f, RANDOM(9999, 1000), 0.2f, FLOWTYPE::SMOOTHSTEP);
		}
}

shared_ptr<PlayerEffect> Player::Get_EffectProto(const string& _EffectTag) {
	shared_ptr<GameObject>	GOBJ = EffectProtoList.find(_EffectTag)->second->Clone(nullptr);
	GOBJ->Set_ObjectTag(GOBJ->Get_ObjectTag() + "_Clone");
	GOBJ->Set_AssetPath("");
	return static_pointer_cast<PlayerEffect>(GOBJ);
}
VOID Player::Play_Effect(shared_ptr<PlayerEffect> _ProtoEffect) {
	_ProtoEffect->Update_EffectTransform();
	GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Add_GameObject(8, _ProtoEffect);
}
unique_ptr<Player>		Player::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Player>(new Player(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Player.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	Player::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Player>(new Player(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Player.");
		return nullptr;
	}
	return Instance;
}