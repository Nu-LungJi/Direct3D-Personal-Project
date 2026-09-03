#include "Utility.h"
#include "GameInstance.h"

Utility::Utility(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {};

HRESULT Utility::Initialize_Utility() {


	return S_OK;
}

XMFLOAT2 Utility::Convert_WorldPositionToScreen(XMVECTOR _WorldPosition) {
	
	ENGINE_DESC EDESC = GameInstance::GetInstance().Get_EngineOption();

	XMVECTOR CurrentPos = XMVectorSetW(_WorldPosition, 1.f);

	XMMATRIX ViewProj = GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix() * GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix();
	XMVECTOR ClipPos = XMVector3TransformCoord(CurrentPos, ViewProj);

	_float NDC_X = XMVectorGetX(ClipPos);
	_float NDC_Y = XMVectorGetY(ClipPos);
	_float NDC_Z = XMVectorGetZ(ClipPos);

	if (NDC_Z > 1.f || NDC_Z < 0.f) return XMFLOAT2(FLT_MAX, FLT_MAX);

	return XMFLOAT2(((NDC_X + 1.f) / 2.f) * EDESC.WindowResolutionX, ((1.f - NDC_Y) / 2.f) * EDESC.WindowResolutionY);
}

unique_ptr<Utility> Utility::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto NG = unique_ptr<Utility>(new Utility(_GRPDEV, _DEVCTX));
	if (FAILED(NG->Initialize_Utility()))
		MSG_BOX("Cannot Create Utility.");

	return NG;
}