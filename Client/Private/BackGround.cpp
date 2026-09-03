#include "../Default/PCH.h"
#include "BackGround.h"
BackGround::BackGround(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
BackGround::BackGround(const BackGround& _PRTOBJ) : GameObject(_PRTOBJ) {}
BackGround::~BackGround()	{}

HRESULT BackGround::Initialize_ProtoType() {
	return S_OK;
}
HRESULT BackGround::Initialize(VOID* _ARG) {
	OBJTAG = "BackGround";
	if (FAILED(__super::Initialize(_ARG))) {
		MSG_BOX("Cannot Initialize BackGround");
		return E_FAIL;
	}
	if (FAILED(Ready_Components())) {
		MSG_BOX("Cannot Ready Components BackGround");
		return E_FAIL;
	}
	
	return S_OK;
}
VOID BackGround::Priority_Update(_float _DT) {

}
VOID BackGround::Update(_float _DT) {

}
VOID BackGround::Late_Update(_float _DT) {

}
HRESULT BackGround::Render() {
	XMFLOAT4X4 Identity = {};
	XMStoreFloat4x4(&Identity, XMMatrixIdentity());

	if (FAILED(Component_Shader->Bind_Matrix("g_WorldMatrix", &Identity)))	return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix" , &Identity)))	return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix" , &Identity)))	return E_FAIL;

	if (FAILED(Component_Shader->Shader_Begin()))							return E_FAIL;
	if (FAILED(Component_Buffer->Bind_Resources()))							return E_FAIL;
	if (FAILED(Component_Buffer->Render_Buffer()))							return E_FAIL;

	return S_OK;
}

HRESULT BackGround::Ready_Components() {
	ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_SHADER] = static_pointer_cast<Shader>(GameInstance::GetInstance().Get_ProtoManager()
		->Clone_ProtoType((uint32_t)SCENE_TYPE::SCENE_1, COMPONENT_TYPE::COMPONENT_SHADER, nullptr));
	Component_Shader = static_pointer_cast<Shader>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_SHADER]);

	ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_BUFFER] = static_pointer_cast<Buffer>(GameInstance::GetInstance().Get_ProtoManager()
		->Clone_ProtoType((uint32_t)SCENE_TYPE::SCENE_1, COMPONENT_TYPE::COMPONENT_BUFFER, nullptr));
	Component_Buffer = static_pointer_cast<Buffer>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_BUFFER]);

	return S_OK;
}
unique_ptr<BackGround>	BackGround::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX){
	auto Instance = unique_ptr<BackGround>(new BackGround(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create BackGround.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	BackGround::Clone(VOID* _ARG) {
	auto Instance = unique_ptr<BackGround>(new BackGround(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone BackGround.");
		return nullptr;
	}
	return Instance;
}