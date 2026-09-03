#include "../Default/PCH.h"
#include "Monster_Knight.h"

Monster_Knight::Monster_Knight(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
Monster_Knight::Monster_Knight(CONST Monster_Knight& _PRTOBJ) : GameObject(_PRTOBJ) {}
Monster_Knight::~Monster_Knight() {}

HRESULT Monster_Knight::Initialize_ProtoType() {

	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform		= Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Model			= Add_Component<MeshLoader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Animator		= Add_Component<Animator>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_ANIMATOR);
	Component_Shader		= Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER);
	Component_NavMeshAgent	= Add_Component<NavMeshAgent>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_NAVIGATION);
	Component_Collider		= Add_Component<Collider>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_COLLIDER);

	Component_Model->Load_FBXModel(L"../../Resource/Asset/Character/Monster/Monster_Knight/Monster_Knight.fbx", ANIMATION_TYPE::ANIMATION, Component_Animator);
	
	return S_OK;
}
HRESULT Monster_Knight::Initialize(VOID* _ARG) {
	Component_Transform		= static_pointer_cast<Transform>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);
	Component_Model			= static_pointer_cast<MeshLoader>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]);
	Component_Animator		= static_pointer_cast<Animator>		 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_ANIMATOR]);
	Component_Collider		= static_pointer_cast<Collider>		 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_COLLIDER]);
	Component_Shader		= static_pointer_cast<Shader>		 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER]);
	Component_NavMeshAgent = static_pointer_cast<NavMeshAgent>		 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_NAVIGATION]);

	Component_Collider->Set_AutomaticTransform(Component_Model, Component_Transform, COLLIDER_TYPE::COLLIDER_OBB);
	Component_Collider->Set_ColliderOwner(shared_from_this());
	GameInstance::GetInstance().Get_CollisionManager()->Register_Collider(Component_Collider);

	Component_Animator->Set_ComponentOwner(shared_from_this());
	Component_Animator->Register_TransformComponent(Component_Transform);
	Component_Animator->Register_NavMeshAgentComponent(Component_NavMeshAgent);

	Component_Animator->Play_Animation(static_cast<uint32_t>(MONSTER_KNIGHT::WAKEUP), true);
	Component_Animator->Stop_Animation();
	Component_Animator->Get_CurrentAnimation()->Set_AnimationState(ANIMATION_STATE::ONREADY);

	GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(shared_from_this());
	
	OBJHP = MaxObjectHP = 100000.f;
	OBJHP = 30000.f;
	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());
	GamePlayer->Get_NavMeshAgentComponent()->Copy_CellList(Component_NavMeshAgent);
	XMVECTOR InitialPosition = XMVectorSetW(Component_NavMeshAgent->Get_CellByIndex(1)->Get_CellVertex(NAVMESH_VERTEX::A), 1.f);
	Component_Transform->Set_WorldPosition(InitialPosition);
	Component_NavMeshAgent->Register_TransformComponent(Component_Transform);

	HPBar = HPBarUI::Create(GRPDEV, DEVCTX);
	HPBar->Set_FontContent(L"Lv 81");

	Generate_BehaviorTree();

	NoiseTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(L"../../Resource/Asset/Effect/BossEffect/T_Tile_300192.png");

	return S_OK;
}

VOID	Monster_Knight::Update(CONST _float& _DT) {
	BehaviorTree->BehaviorTree_Update(_DT);
	Update_StateUI(_DT);
	Component_Animator->Update_Animation(_DT);
	if (nullptr == MonsterDamageFont) {
		MonsterDamageFont = static_pointer_cast<DamageFontUI>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("DamageFontUI"));
	}

	if (DissolveEnable == PLAY_TYPE::ONSTART) {
		DissolveEnable = PLAY_TYPE::ONPROGRESS;
	}
	else if (DissolveEnable == PLAY_TYPE::ONPROGRESS) {
		if (DissolveAmount >= 1.f) {
			DissolveAmount = 1.f;
			DissolveEnable = PLAY_TYPE::FINISHED;
			OBJDEAD = true;
		}
		DissolveAmount += _DT * DissolveSpeed;
	}
}
VOID	Monster_Knight::Late_Update(CONST _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
	if (Visualize_StateUI && OBJHP > 0.f) { 
		HPBar->Late_Update(_DT);
		HPBar->Set_CurrentHPPercentage(OBJHP / MaxObjectHP);
	}
}
HRESULT		Monster_Knight::Render() {
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMFLOAT4 DissolveColor = { 1.f, 1.f, 40.f / 255.f, 1.f };
	XMStoreFloat4x4(&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());

	if (FAILED(Component_Transform->Bine_ShaderResource(Component_Shader, "g_WorldMatrix")))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))							return E_FAIL;
	
	if (FAILED(Component_Shader->Bind_ShaderResourceView("g_NoiseTexture", NoiseTexture)))					return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("DissolveAmount", &DissolveAmount, sizeof(_float))))			return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("DissolveColor", &DissolveColor, sizeof(XMFLOAT4))))			return E_FAIL;

	Component_Model->Render_Mesh(Component_Shader, XMMatrixIdentity(), 1);

	return S_OK;
}
VOID Monster_Knight::On_CollisionEnter(shared_ptr<GameObject> _ColliderOwner) {
	if (OBJDEAD) return;
	for (uint32_t IDX = 0; IDX < MAX_HITBOXPOOL_SIZE; IDX++) {
		string HitBoxTag = "PlayerHitBox" + to_string(IDX);
		if (_ColliderOwner->Get_ObjectTag() == HitBoxTag) {
			uint32_t HitBoxDMG = static_pointer_cast<PlayerHitBox>(_ColliderOwner)->Get_HitBoxDamage();
			MonsterDamageFont->Generate_DamageFont(Component_Transform->Get_WorldPosition(), HitBoxDMG);
			OBJHP -= HitBoxDMG;
			HitCall = true;

			return;
		}
	}
}
VOID Monster_Knight::On_CollisionStay(shared_ptr<GameObject> _ColliderOwner) {

}
VOID Monster_Knight::On_CollisionExit(shared_ptr<GameObject> _ColliderOwner) {

}
VOID Monster_Knight::Generate_BehaviorTree() {
	BlackBoard	 = BTBlackBoard::Create();
	BehaviorTree = BehaviorTree_Knight::Create(BlackBoard);

	shared_ptr<Transform> PlayerTransform = static_pointer_cast<Transform>(GamePlayer->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	vector<uint32_t> AttackAnimIndexList = {
		static_cast<uint32_t>(MONSTER_KNIGHT::ATTACK01),
		static_cast<uint32_t>(MONSTER_KNIGHT::ATTACK02),
		static_cast<uint32_t>(MONSTER_KNIGHT::ATTACK03)
	};

	BlackBoard->Set_Value<shared_ptr<Transform>>("OwnerTransform", Component_Transform);
	BlackBoard->Set_Value<shared_ptr<Transform>>("PlayerTransform", PlayerTransform);
	BlackBoard->Set_Value<shared_ptr<Animator>> ("OwnerAnimator", Component_Animator);

	BlackBoard->Set_Value<_bool>("Awake Completed", false);
	BlackBoard->Set_Value<_bool>("StandByStance", false);
	BlackBoard->Set_Value<_bool>("CombatStance", false);
	BlackBoard->Set_Value<_float>("MovementAmount", 0.5f);

	BlackBoard->Set_Value<XMVECTOR>("Center Position", Component_Transform->Get_WorldPosition());

	auto RootNode = make_unique<Selector>(BlackBoard, shared_from_this());

	auto DefaultStateSelector = make_unique<Selector>(BlackBoard, shared_from_this());
	{
		auto DeathSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
		DeathSequencer->AddNode(make_unique<Check_DeathCondition>(BlackBoard, shared_from_this()));
		DeathSequencer->AddNode(make_unique<Play_DeathAnimation>(BlackBoard, shared_from_this(), Component_Transform, Component_Animator, static_cast<uint32_t>(MONSTER_KNIGHT::DEATH)));
		DeathSequencer->AddNode(make_unique<Play_DissolveEffect>(BlackBoard, shared_from_this(), static_pointer_cast<Monster_Knight>(shared_from_this())));
		DeathSequencer->AddNode(make_unique<Destroy_OwnerActor>(BlackBoard, shared_from_this()));
		DefaultStateSelector->AddNode(move(DeathSequencer));
	}
	{
		auto DamageSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
		DamageSequencer->AddNode(make_unique<Play_AttackedAnimation>(BlackBoard, shared_from_this(), Component_Animator, Component_Transform, &HitCall, static_cast<uint32_t>(MONSTER_KNIGHT::BEHIT_LEFT), 2.f));
		DamageSequencer->AddNode(make_unique<AlwaysFailureNode>(BlackBoard, shared_from_this()));
		DefaultStateSelector->AddNode(move(DamageSequencer));
	}
	
	auto DetectSelector = make_unique<Selector>(BlackBoard, shared_from_this());
	{
		auto FirstDetectSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
		FirstDetectSequencer->AddNode(make_unique<Check_FirstDetect>(BlackBoard, shared_from_this()));
		FirstDetectSequencer->AddNode(make_unique<Check_TargetDistance>(BlackBoard, shared_from_this(), PlayerTransform, 20.f));
		FirstDetectSequencer->AddNode(make_unique<Play_StagingAnimation>(BlackBoard, shared_from_this(), static_cast<uint32_t>(MONSTER_KNIGHT::WAKEUP), 1.5f));
		FirstDetectSequencer->AddNode(make_unique<Play_CombatStateAnimation>(BlackBoard, shared_from_this(), static_cast<uint32_t>(MONSTER_KNIGHT::BATTLESTANCE)));
		FirstDetectSequencer->AddNode(make_unique<Disable_FirstDetect>(BlackBoard, shared_from_this()));
	
		DetectSelector->AddNode(move(FirstDetectSequencer));
	}
	{
		auto OverDetectSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
		OverDetectSequencer->AddNode(make_unique<Check_TargetDistance>(BlackBoard, shared_from_this(), PlayerTransform, 20.f));
		OverDetectSequencer->AddNode(make_unique<Play_ConvertAnimation>(BlackBoard, shared_from_this(), Component_Animator, static_cast<uint32_t>(MONSTER_KNIGHT::STANDBY_END)));
		OverDetectSequencer->AddNode(make_unique<Convert_CombatState>(BlackBoard, shared_from_this()));
		OverDetectSequencer->AddNode(make_unique<AlwaysFailureNode>(BlackBoard, shared_from_this()));
	
		DetectSelector->AddNode(move(OverDetectSequencer));
	}
	
	auto ComBatSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
	ComBatSequencer->AddNode(make_unique<Check_TargetDistance>(BlackBoard, shared_from_this(), PlayerTransform, 20.f));
	ComBatSequencer->AddNode(make_unique<Attack_TranslateNode>(BlackBoard, shared_from_this(), static_cast<uint32_t>(MONSTER_KNIGHT::WALK_LEFT), 2.f, 2.f));
	ComBatSequencer->AddNode(make_unique<Attack_RotateNode>(BlackBoard, shared_from_this(), 1.5f));
	ComBatSequencer->AddNode(make_unique<Attack_AnimationNode>(BlackBoard, shared_from_this(), AttackAnimIndexList, 2.f));
	
	auto RevertSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
	RevertSequencer->AddNode(INVERTNODE(make_unique<Check_FirstDetect>(BlackBoard, shared_from_this())));
	RevertSequencer->AddNode(INVERTNODE(make_unique<Check_TargetDistance>(BlackBoard, shared_from_this(), PlayerTransform, 20.f)));
	{
		auto RotateMoveSequencer = make_unique<Paralle>(BlackBoard, shared_from_this());
		RotateMoveSequencer->AddNode(make_unique<Rotate_CenterPoint>(BlackBoard, shared_from_this(), 2.f));
		RotateMoveSequencer->AddNode(make_unique<Move_CenterPoint>(BlackBoard, shared_from_this(), static_cast<uint32_t>(MONSTER_KNIGHT::WALK_FRONT)));
	
		RevertSequencer->AddNode(move(RotateMoveSequencer));
	}
	RevertSequencer->AddNode(make_unique<Play_StandByAnimation>(BlackBoard, shared_from_this()));

	RootNode->AddNode(move(DefaultStateSelector));
	RootNode->AddNode(move(DetectSelector));
	RootNode->AddNode(move(ComBatSequencer));
	RootNode->AddNode(move(RevertSequencer));

	BehaviorTree->Set_RootNode(move(RootNode));
}

VOID Monster_Knight::Update_StateUI(CONST _float& _DT) {
	_float DistanceFromPlayer = XMVectorGetX(XMVector3Length(XMVectorSubtract(Component_Transform->Get_WorldPosition(),
		static_pointer_cast<Transform>(GamePlayer->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_WorldPosition())));

	XMFLOAT2 HPBarPos = GameInstance::GetInstance().Get_Utility()->Convert_WorldPositionToScreen(Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 5.5f, 0.f, 0.f));
	_float	 HPBarScale = 0.15f, HPFontScale = 0.5f;

	HPBar->Set_FontPosition(HPBarPos - XMVectorSet(0.f, 20.f, 0.f, 0.f));
	HPBar->Set_FontScale(HPFontScale - HPFontScale / 50.f * DistanceFromPlayer);

	HPBar->Set_Position(HPBarPos);
	HPBar->Set_Scale(HPBarScale - HPBarScale / 50.f * DistanceFromPlayer);

	HPBar->Get_FontScale() >= 0.25f && HPBar->Get_FontScale() <= 0.5f ? Visualize_StateUI = true : Visualize_StateUI = false;

	if (ObjectHPPercentage > OBJHP / MaxObjectHP) ObjectHPPercentage -= _DT;
}

unique_ptr<Monster_Knight>	Monster_Knight::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Monster_Knight>(new Monster_Knight(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Monster Knight.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>		Monster_Knight::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Monster_Knight>(new Monster_Knight(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Monster Knight.");
		return nullptr;
	}
	return Instance;
}