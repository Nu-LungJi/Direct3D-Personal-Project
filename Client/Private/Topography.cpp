#include "../Default/PCH.h"
#include "GameInstance.h"

Topography::Topography(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
Topography::Topography(CONST Topography& _PRTOBJ) : GameObject(_PRTOBJ) {}

HRESULT		Topography::Initialize_ProtoType() {

	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	COLLIDER_TYPE	ColliderType = COLLIDER_TYPE::COLLIDER_OBB;

	Component_Transform  = Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Shader	 = Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TERRAIN_SHADER);
	Component_Navigation = Add_Component<NavMeshAgent>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_NAVIGATION);
	Component_Terrain	 = Add_Component<Terrain>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TERRAIN);

	return S_OK;
}
HRESULT		Topography::Initialize(VOID* _ARG) {
	Component_Transform  = static_pointer_cast<Transform>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);
	Component_Terrain	 = static_pointer_cast<Terrain>	(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TERRAIN]	 );
	Component_Shader	 = static_pointer_cast<Shader>	(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TERRAIN_SHADER]);
	Component_Navigation = static_pointer_cast<NavMeshAgent>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_NAVIGATION]);

	Component_Transform->Set_WorldPosition(-256.f, -1.5f, -128.f);
	Component_Transform->Set_WorldScale(1.f, 1.f, 1.f);

	Texture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(L"../../Resource/Asset/Texture/Terrain_Texture.png");

	return S_OK;
}
VOID		Topography::Update(CONST _float& _DT) {

}
VOID		Topography::Late_Update(CONST _float& _DT) {
	//GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
}
HRESULT		Topography::Render() {
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMFLOAT4 Position;

	XMStoreFloat4x4 (&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4 (&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());
	XMStoreFloat4	(&Position	, GameInstance::GetInstance().Get_MainCamera()->Get_CameraPosition());

	if (FAILED(Component_Transform->Bine_ShaderResource(Component_Shader, "g_WorldMatrix")))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))							return E_FAIL;

	if (FAILED(Component_Shader->Bind_ShaderResourceView("g_DiffuseTexture", Texture)))				return E_FAIL;

	if	(FAILED(Component_Shader->Shader_Begin(0)))													return E_FAIL;
	if	(FAILED(Component_Terrain->Bind_Resources()))												return E_FAIL;
	if	(FAILED(Component_Terrain->Render_Buffer()))												return E_FAIL;

	return S_OK;
}

unique_ptr<Topography>	Topography::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Topography>(new Topography(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Topography.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	Topography::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Topography>(new Topography(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Topography");
		return nullptr;
	}
	return Instance;
}