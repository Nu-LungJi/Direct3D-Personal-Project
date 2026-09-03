#include "../Default/PCH.h"
#include "Monster_FinalBoss.h"

Monster_FinalBoss::Monster_FinalBoss(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
Monster_FinalBoss::~Monster_FinalBoss() {}

HRESULT Monster_FinalBoss::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform		= Add_Component<Transform>		(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Model			= Add_Component<MeshLoader>		(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Animator		= Add_Component<Animator>		(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_ANIMATOR);
	Component_Shader		= Add_Component<Shader>			(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER);
	Component_NavMeshAgent	= Add_Component<NavMeshAgent>	(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_NAVIGATION);
	Component_Collider		= Add_Component<Collider>		(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_COLLIDER);

	Component_Model->Load_FBXModel(L"../../Resource/Asset/Character/Scar/Scar.fbx", ANIMATION_TYPE::ANIMATION, Component_Animator);

	return S_OK;
}
HRESULT Monster_FinalBoss::Initialize(VOID* _ARG) {
	Component_Transform		= static_pointer_cast<Transform>	( ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]	    );
	Component_Model			= static_pointer_cast<MeshLoader>	( ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]		    );
	Component_Animator		= static_pointer_cast<Animator>		( ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_ANIMATOR]	    );
	Component_Collider		= static_pointer_cast<Collider>		( ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_COLLIDER]		);
	Component_Shader		= static_pointer_cast<Shader>		( ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER] );

	Component_Collider->Set_AutomaticTransform(Component_Model, Component_Transform, COLLIDER_TYPE::COLLIDER_OBB);
	Component_Collider->Set_ColliderOwner(shared_from_this());
	GameInstance::GetInstance().Get_CollisionManager()->Register_Collider(Component_Collider);

	Component_Animator->Set_ComponentOwner(shared_from_this());
	Component_Animator->Register_TransformComponent(Component_Transform);
	Component_Animator->Register_NavMeshAgentComponent(Component_NavMeshAgent);

	Component_Animator->Play_Animation(static_cast<uint32_t>(MONSTER_KNIGHT::ATTACK01), true);
	Component_Animator->Stop_Animation();
	Component_Animator->Get_CurrentAnimation()->Set_AnimationState(ANIMATION_STATE::ONREADY);

	GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(shared_from_this());

	OBJHP = MaxObjectHP = 100000.f;

	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());
	GamePlayer->Get_NavMeshAgentComponent()->Copy_CellList(Component_NavMeshAgent);

	Component_Transform->Set_WorldScale(0.75f, 0.75f, 0.75f);
	Component_NavMeshAgent->Register_TransformComponent(Component_Transform);

	Component_Transform->Set_WorldPosition(-8.f, 18.f, -245.f);
	NoiseTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(L"../../Resource/Asset/Effect/BossEffect/T_Tile_300192.png");

	Generate_BehaviorTree();
	Generate_EffectList();

	return S_OK;
}
VOID	Monster_FinalBoss::Update(CONST _float& _DT) {
	if (nullptr == MonsterDamageFont)
		MonsterDamageFont = static_pointer_cast<DamageFontUI>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("DamageFontUI"));
	if (nullptr == HPBar) 
		HPBar = static_pointer_cast<BossUI>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("BossUI"));
	if (Activation == false)	return;

	Component_Collider->Update(_DT);
	BehaviorTree->BehaviorTree_Update(_DT);
	Component_Animator->Update_Animation(_DT);

	if (KEY_DOWN(DIK_F7)) {
		Component_Transform->Set_WorldRotation(0.f, 0.f, 0.f);
		Component_Animator->Play_Animation(static_cast<uint32_t>(MONSTER_SCAR::ATTACK01), true);
		Component_Animator->Set_AnimationBlending(true);
		GameInstance::GetInstance().Set_MainCamera(3);
		static_pointer_cast<ActionCamera_Boss>(GameInstance::GetInstance().Get_CameraByIndex(1))->Activate_CutScene();
	}
	
	if		(DissolveEnable == PLAY_TYPE::ONSTART) {
		DissolveEnable = PLAY_TYPE::ONPROGRESS;
	}
	else if (DissolveEnable == PLAY_TYPE::FINISHED) {
		DissolveEnable = PLAY_TYPE::ONSTART;
	}
	else if (DissolveEnable == PLAY_TYPE::ONPROGRESS) {
		if (DelayTime > 0.f) {
			DelayTime -= _DT;
			return;
		}

		if (DissolvePlayInverse) {
			DissolveAmount -= _DT * DissolveSpeed;
			if (DissolveAmount <= 0.f) {
				DissolveAmount = 0.f;
				DissolveEnable = PLAY_TYPE::FINISHED;
			}
		}
		else {
			DissolveAmount += _DT * DissolveSpeed;
			if (DissolveAmount >= 1.f) {
				DissolveAmount = 1.f;
				DissolveEnable = PLAY_TYPE::FINISHED;
			}
		}
	}
	if (KEY_DOWN(DIK_5)) {
		auto SwordTrail = this->Get_EffectProto("EFF_CircleTrail");

		SwordTrail->Set_EffectOwner(static_pointer_cast<Monster_FinalBoss>(shared_from_this()));
		SwordTrail->Set_EffectOrbitRotation(0.f, 0.f);
		SwordTrail->Set_EffectRotation({ 0.f, -20.f, 0.f });

		SwordTrail->Set_EmissiveOption({ 50.f / 255.f , 35.f / 255.f, 100.f / 255.f }, 1.f);
		SwordTrail->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		SwordTrail->Set_EffectScrollSpeed({ -1.5f, 0.f });

		SwordTrail->Set_DissolveTextureOffset({ 0.5f, 0.f });
		SwordTrail->Set_EffectTextureOffset({ 0.5f, 0.f });

		SwordTrail->Set_DissolveStrength(0.2f);
		SwordTrail->Set_EffectScale({ 15.f, 15.f, 15.f });
		SwordTrail->Set_EffectLifeTime(1.f);

		SwordTrail->Set_EffectFadeOutValue(0.1f);
		SwordTrail->Set_DistanceFromPlayer(-3.f);
		SwordTrail->Set_EffectOffset({ 0.f, 4.f, 0.f });
		SwordTrail->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
		SwordTrail->Set_TimeAccumulation(0.f);
		SwordTrail->Set_TimeProgress(0.f);

		this->Play_Effect(SwordTrail);
	}
	if (KEY_DOWN(DIK_6)) {
		auto SwordTrail = this->Get_EffectProto("EFF_CircleTrail");

		SwordTrail->Set_EffectOwner(static_pointer_cast<Monster_FinalBoss>(shared_from_this()));
		SwordTrail->Set_EffectOrbitRotation(0.f, 0.f);
		SwordTrail->Set_EffectRotation({ 0.f, -20.f, 0.f });

		SwordTrail->Set_EmissiveOption({ 50.f / 255.f , 35.f / 255.f, 100.f / 255.f }, 1.f);
		SwordTrail->Set_DissolveScrollSpeed({ 0.75f, 0.f });
		SwordTrail->Set_EffectScrollSpeed({ 0.75f, 0.f });

		SwordTrail->Set_DissolveTextureOffset({ -0.75f, 0.f });
		SwordTrail->Set_EffectTextureOffset({ -0.75f, 0.f });

		SwordTrail->Set_DissolveStrength(0.2f);
		SwordTrail->Set_EffectScale({ 12.f, 12.f, 12.f });
		SwordTrail->Set_EffectLifeTime(1.f);

		SwordTrail->Set_EffectFadeOutValue(0.1f);
		SwordTrail->Set_DistanceFromPlayer(-6.f);
		SwordTrail->Set_EffectOffset({ 0.f, 4.f, 0.f });
		SwordTrail->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
		SwordTrail->Set_TimeAccumulation(0.f);
		SwordTrail->Set_TimeProgress(0.f);

		this->Play_Effect(SwordTrail);
	}
}
VOID		Monster_FinalBoss::Late_Update(CONST _float& _DT) {
	if (Activation == false)	return;
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
	if (Visualize_StateUI && OBJHP > 0.f) {
		HPBar->Set_CurrentHPPercentage(OBJHP / MaxObjectHP);
	}
}
HRESULT		Monster_FinalBoss::Render() {
	if (Activation == false)	return S_OK;
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMFLOAT4 DissolveColor = { 57.f / 255.f, 16.f / 255.f, 123.f / 255.f, 1.f };

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

VOID Monster_FinalBoss::On_CollisionEnter(shared_ptr<GameObject> _ColliderOwner) {
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
VOID Monster_FinalBoss::On_CollisionStay(shared_ptr<GameObject> _ColliderOwner) {

}
VOID Monster_FinalBoss::On_CollisionExit(shared_ptr<GameObject> _ColliderOwner) {

}
VOID Monster_FinalBoss::Enable_Dissolve(PLAY_TYPE _Enable, _bool _Inverse, _float _DelayTime, _float _DissolveSpeed) {
	DissolveEnable = _Enable;
	DelayTime = _DelayTime;
	DissolveSpeed = _DissolveSpeed;
	DissolvePlayInverse = _Inverse;
}

VOID Monster_FinalBoss::Generate_BehaviorTree() {
	BlackBoard	 = BTBlackBoard::Create();
	BehaviorTree = BehaviorTree_FinalBoss::Create(BlackBoard);

	shared_ptr<Transform> PlayerTransform = static_pointer_cast<Transform>(GamePlayer->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

	BlackBoard->Set_Value<shared_ptr<Transform>>("OwnerTransform", Component_Transform);
	BlackBoard->Set_Value<shared_ptr<Transform>>("PlayerTransform", PlayerTransform );
	BlackBoard->Set_Value<shared_ptr<Animator>> ("OwnerAnimator", Component_Animator);

	BlackBoard->Set_Value<_bool>("Target Engaged", true);
	BlackBoard->Set_Value<_float>("MovementAmount", 1.f);

	auto RootNode = make_unique<Selector>(BlackBoard, shared_from_this());

	auto DefaultStateSelector = make_unique<Selector>(BlackBoard, shared_from_this());
	{
		auto DeathStateSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
		DeathStateSequencer->AddNode(make_unique<Check_DeathCondition>(BlackBoard, shared_from_this()));
		DeathStateSequencer->AddNode(make_unique<BS_Play_DeathAnimation>(BlackBoard, shared_from_this(), Component_Transform, Component_Animator, static_cast<uint32_t>(MONSTER_SCAR::DEATH)));
		DeathStateSequencer->AddNode(make_unique<Destroy_OwnerActor>(BlackBoard, shared_from_this()));
		DefaultStateSelector->AddNode(move(DeathStateSequencer));
	}
	{
		auto GroggyStateSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
		GroggyStateSequencer->AddNode(make_unique<Check_GroggyCondition>(BlackBoard, shared_from_this(), &GroggyCall));
		GroggyStateSequencer->AddNode(make_unique<Play_GroggyAnimation>(BlackBoard, shared_from_this(), Component_Animator, static_cast<uint32_t>(MONSTER_SCAR::GROGGY)));
		GroggyStateSequencer->AddNode(make_unique<AlwaysFailureNode>(BlackBoard, shared_from_this()));
		DefaultStateSelector->AddNode(move(GroggyStateSequencer));
	}
	{
		auto DamageSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
		DamageSequencer->AddNode(make_unique<Execute_HitStop>(BlackBoard, shared_from_this(), &HitCall));
		DamageSequencer->AddNode(make_unique<AlwaysFailureNode>(BlackBoard, shared_from_this()));
		DefaultStateSelector->AddNode(move(DamageSequencer));
	}
	auto TransformSelector = make_unique<Selector>(BlackBoard, shared_from_this());
	TransformSelector->AddNode(make_unique<LookAt_TargetPosition>(BlackBoard, shared_from_this(), Component_Transform, 2.f));
	TransformSelector->AddNode(make_unique<AlwaysFailureNode>(BlackBoard, shared_from_this()));

	auto AttackSelector = make_unique<Selector>(BlackBoard, shared_from_this());
	AttackSelector = move(Compose_AttackNodeList(move(AttackSelector)));

	auto EffectSelector = make_unique<Selector>(BlackBoard, shared_from_this());
	EffectSelector->AddNode(make_unique<Play_AttackEffect>(BlackBoard, shared_from_this(), Component_Animator));

	RootNode->AddNode(move(DefaultStateSelector));
	RootNode->AddNode(move(TransformSelector));
	RootNode->AddNode(move(EffectSelector));
	RootNode->AddNode(move(AttackSelector));
	BehaviorTree->Set_RootNode(move(RootNode));
}
unique_ptr<Selector> Monster_FinalBoss::Compose_AttackNodeList(unique_ptr<Selector> _CompositeNode) {
	auto AnimationList = Component_Animator->Get_AnimationList();
	vector<AttackConfig>	AttackConfigList = {
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK01), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK01)], 200 }, // 등장 후 도약 공격 : MONSTER_SCAR::ATTACK01
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK02), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK02)], 200 }, // 손바닥 원기옥 내려찍기 공격 : MONSTER_SCAR::ATTACK02
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK03), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK03)], 200 }, // 점프 후, 540도 회전 휘두르기 : MONSTER_SCAR::ATTACK03
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK04), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK04)], 200 }, // 공중부양 후, 찍어 누르기 : MONSTER_SCAR::ATTACK04 - 빼자
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK05), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK05)], 200 }, // 플레이어 캐치 후, 슬램 : MONSTER_SCAR::ATTACK05
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK06), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK06)], 200 }, // 기침 후, 표효 : MONSTER_SCAR::ATTACK06 - 빼자
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK07), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK07)], 200 }, // 양손 두 번 휘두르기 : MONSTER_SCAR::ATTACK07
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK08), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK08)], 200 }, // 옆으로 도약, 다시 제자리 오면서 휘두르기 : MONSTER_SCAR::ATTACK08
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK09), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK09)], 200 }, // 궁극기 : 땅에 칼을 꽂고 올려치기 : MONSTER_SCAR::ATTACK09
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK10), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK10)], 200 }, //    -> (플레이어 에어본 된 경우)MONSTER_SCAR::ATTACK10
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK11), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK11)], 200 }, // 외발감기 3연타 : MONSTER_SCAR::ATTACK11
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK12), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK12)], 200 }, // 3연타 후, 칼 손잡이 찍기 : MONSTER_SCAR::ATTACK12 - 빼자
		{ static_cast<uint32_t>(MONSTER_SCAR::ATTACK13), AnimationList[static_cast<uint32_t>(MONSTER_SCAR::ATTACK13)], 200 }  // 순간이동: MONSTER_SCAR::ATTACK13
	};

	auto AttackSequencer = make_unique<Sequencer>(BlackBoard, shared_from_this());
	AttackSequencer->AddNode(make_unique<Check_AttackCondition>(BlackBoard, shared_from_this(), Component_Animator));
	AttackSequencer->AddNode(make_unique<Play_AttackAnimation>(BlackBoard, shared_from_this(), Component_Animator, &AnimIndex));
	AttackSequencer->AddNode(make_unique<AlwaysFailureNode>(BlackBoard, shared_from_this()));

	_CompositeNode->AddNode(move(AttackSequencer));

	return _CompositeNode;
}

shared_ptr<BossEffect> Monster_FinalBoss::Get_EffectProto(const string& _EffectTag) {
	shared_ptr<GameObject>	GOBJ = EffectProtoList.find(_EffectTag)->second->Clone(nullptr);
	GOBJ->Set_ObjectTag(GOBJ->Get_ObjectTag() + "_Clone");
	GOBJ->Set_AssetPath("");
	return static_pointer_cast<BossEffect>(GOBJ);
}
VOID Monster_FinalBoss::Play_Effect(shared_ptr<BossEffect> _ProtoEffect) {
	_ProtoEffect->Update_EffectTransform();
	GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Add_GameObject(8, _ProtoEffect);
}
VOID Monster_FinalBoss::Generate_EffectList() {
	shared_ptr<BossEffect>	EffectPrototype = { nullptr };
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();

	wstring UsualDissolveTexFilePath = L"../../Resource/Asset/Effect/BossEffect/T_Tile_20003.png";
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_HalfTrail");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_HalfTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EffectScrollSpeed({ -1.5f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.2f);
		EffectPrototype->Set_DissolveEdgeWidth(0.2f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.43f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_CirclePlane_Charge");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_CirclePlane_Charge.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EffectScrollSpeed({ -1.5f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.2f);
		EffectPrototype->Set_DissolveEdgeWidth(0.2f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_CirclePlane_Charge_v2");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_CirclePlane_Charge_v2.fbx");
		EffectPrototype->Allocate_DissolveTexture(UsualDissolveTexFilePath);
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EffectScrollSpeed({ -1.5f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.2f);
		EffectPrototype->Set_DissolveEdgeWidth(0.2f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_CircleTrail");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_CircleTrail.fbx");
		EffectPrototype->Allocate_DissolveTexture(L"../../Resource/Asset/Effect/BossEffect/T_Dissovlve_30005.png");
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EffectScrollSpeed({ -1.5f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.2f);
		EffectPrototype->Set_DissolveEdgeWidth(0.2f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_CardBomb");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_CardBomb.fbx");
		EffectPrototype->Allocate_DissolveTexture(L"../../Resource/Asset/Effect/BossEffect/T_Tile_230006_2.png");
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EffectScrollSpeed({ -1.5f, 0.3f });
		EffectPrototype->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.2f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.0f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.2f);
		EffectPrototype->Set_DissolveEdgeWidth(0.2f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_CatchSlam_UpWave");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_CatchSlam_UpWave.fbx");
		//EffectPrototype->Allocate_EffectSourceTexture(L"../../Resource/Asset/Effect/T_Changli_25004_wings.png");
		EffectPrototype->Allocate_DissolveTexture(L"../../Resource/Asset/Effect/BossEffect/T_Tile_50003.png");
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EffectScrollSpeed({ -10.5f, 0.f });
		EffectPrototype->Set_DissolveScrollSpeed({ -10.5f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ -0.5f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ -0.5f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.02f);
		EffectPrototype->Set_DissolveEdgeWidth(0.02f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_CatchSlam_DecalWave");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_CatchSlam_DecalWave.fbx");
		//EffectPrototype->Allocate_EffectSourceTexture(L"../../Resource/Asset/Effect/T_Changli_25004_wings.png");
		EffectPrototype->Allocate_DissolveTexture(L"../../Resource/Asset/Effect/BossEffect/T_Ramp_30021.png");
		EffectPrototype->Set_EffectLifeTime(1.f);

		EffectPrototype->Set_EffectScrollSpeed({ 0.f, 0.f });
		EffectPrototype->Set_DissolveScrollSpeed({ 0.f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.f);

		EffectPrototype->Set_DissolveStrength(0.02f);
		EffectPrototype->Set_DissolveEdgeWidth(0.02f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_CircleTrail_V2");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_CircleTrail_V2.fbx");
		//EffectPrototype->Allocate_EffectSourceTexture(L"../../Resource/Asset/Effect/T_Changli_25004_wings.png");
		EffectPrototype->Allocate_NoiseTexture(L"../../Resource/Asset/Effect/BossEffect/T_Tile_300192.png");
		EffectPrototype->Set_EffectLifeTime(1.f);

		EffectPrototype->Set_EffectScrollSpeed({ 0.f, 0.f });
		EffectPrototype->Set_DissolveScrollSpeed({ 0.f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.f);

		EffectPrototype->Set_DissolveStrength(0.02f);
		EffectPrototype->Set_DissolveEdgeWidth(0.02f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_QuadRoar");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_QuadRoar.fbx");
		//EffectPrototype->Allocate_EffectSourceTexture(L"../../Resource/Asset/Effect/T_Changli_25004_wings.png");
		EffectPrototype->Allocate_NoiseTexture(L"../../Resource/Asset/Effect/BossEffect/T_Ring_30005.png");
		EffectPrototype->Set_EffectLifeTime(1.f);
		EffectPrototype->Set_ShaderPassNumber(1);

		EffectPrototype->Set_EmissiveOption({ 0.255f, 0.157f, 0.941f }, 0.5f);
		EffectPrototype->Set_EffectScrollSpeed({ 0.f, 0.f });
		EffectPrototype->Set_DissolveScrollSpeed({ 0.f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ 0.f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ 0.f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.f);

		EffectPrototype->Set_DissolveStrength(0.f);
		EffectPrototype->Set_DissolveEdgeWidth(0.f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_SwordAura");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_SwordAura.fbx");
		//EffectPrototype->Allocate_EffectSourceTexture(L"../../Resource/Asset/Effect/T_Changli_25004_wings.png");
		EffectPrototype->Allocate_DissolveTexture(L"../../Resource/Asset/Effect/BossEffect/T_Trail_Dny_360002.png");
		EffectPrototype->Set_EffectLifeTime(0.5f);

		EffectPrototype->Set_EffectScrollSpeed({ -10.5f, 0.f });
		EffectPrototype->Set_DissolveScrollSpeed({ -10.5f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ -0.5f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ -0.5f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.02f);
		EffectPrototype->Set_DissolveEdgeWidth(0.02f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_TrailUp_V2");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_TrailUp_V2.fbx");
		//EffectPrototype->Allocate_EffectSourceTexture(L"../../Resource/Asset/Effect/T_Changli_25004_wings.png");
		EffectPrototype->Allocate_DissolveTexture(L"../../Resource/Asset/Effect/BossEffect/T_Trail_160005_2.png");
		EffectPrototype->Set_EffectLifeTime(0.5f);
		EffectPrototype->Set_EmissiveOption({ 50.f / 255.f / 2.f , 35.f / 255.f / 2.f, 100.f / 255.f / 2.f }, 1.f);

		EffectPrototype->Set_EffectScrollSpeed({ -10.5f, 0.f });
		EffectPrototype->Set_DissolveScrollSpeed({ -10.5f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ -0.5f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ -0.5f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.02f);
		EffectPrototype->Set_DissolveEdgeWidth(0.02f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
	{
		EffectPrototype = BossEffect::Create(GRPDEV, DEVCTX);
		EffectPrototype->Set_ObjectTag("EFF_UltimateTrail");

		EffectPrototype->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/EFF_UltimateTrail.fbx");
		//EffectPrototype->Allocate_EffectSourceTexture(L"../../Resource/Asset/Effect/T_Changli_25004_wings.png");
		EffectPrototype->Allocate_DissolveTexture(L"../../Resource/Asset/Effect/BossEffect/T_Trail_30038_2.png");
		EffectPrototype->Set_EffectLifeTime(0.5f);
		EffectPrototype->Set_EmissiveOption({ 50.f / 255.f / 2.f , 35.f / 255.f / 2.f, 100.f / 255.f / 2.f }, 1.f);

		EffectPrototype->Set_EffectScrollSpeed({ -10.5f, 0.f });
		EffectPrototype->Set_DissolveScrollSpeed({ -10.5f, 0.f });
		EffectPrototype->Set_EffectTextureOffset({ -0.5f, 0.f });
		EffectPrototype->Set_DissolveTextureOffset({ -0.5f, 0.f });
		EffectPrototype->Set_EffectFadeOutValue(0.2f);

		EffectPrototype->Set_DissolveStrength(0.02f);
		EffectPrototype->Set_DissolveEdgeWidth(0.02f);
		EffectPrototype->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

		EffectProtoList.insert({ EffectPrototype->Get_ObjectTag(), EffectPrototype });
	}
}
unique_ptr<Monster_FinalBoss>	Monster_FinalBoss::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Monster_FinalBoss>(new Monster_FinalBoss(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Monster FinalBoss.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>			Monster_FinalBoss::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Monster_FinalBoss>(new Monster_FinalBoss(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Monster FinalBoss.");
		return nullptr;
	}
	return Instance;
}