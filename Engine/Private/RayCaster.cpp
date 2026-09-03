#include "RayCaster.h"
#include "GameInstance.h"

RayCaster::RayCaster(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) { }

HRESULT RayCaster::Register_Recipient(shared_ptr<GameObject> _GOBJ) {
	RecipientList.push_back(_GOBJ); 
	return S_OK;
}
HRESULT RayCaster::UnRegister_Recipient(shared_ptr<GameObject> _GOBJ) {
	auto iter = find(RecipientList.begin(), RecipientList.end(), _GOBJ);
	if (iter == RecipientList.end()) return E_FAIL;

	iter = RecipientList.erase(iter);
	return S_OK;
}
shared_ptr<GameObject> RayCaster::RayCast_Collider() {
	if (nullptr == MainCamera)	MainCamera = GameInstance::GetInstance().Get_MainCamera();

	CastedObjectList.clear();
	const ENGINE_DESC& EngineOption = GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription();

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(EngineOption.hWnd, &ptMouse);

	XMMATRIX ProjectionMatrix	= MainCamera->Get_ProjMatrix();
	XMMATRIX ViewMatrix			= MainCamera->Get_ViewMatrix();
	XMMATRIX InvViewMatrix		= MainCamera->Get_InvViewMatrix();

	// 스크린 좌표 (0, 0) ~ (1280, 720) >> NDC 좌표 (-1, -1) ~ (1, 1) 변환 >> ViewSpace 좌표 변환
	// ViewSpace : 카메라는 원점(0, 0, 0), Z축을 바라보게 만드는 과정
	RECT rc{};
	HWND hWnd = GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription().hWnd;
	GetWindowRect(hWnd, &rc);

	_float NDCX = (( 2.f * ptMouse.x) / EngineOption.WindowResolutionX- 1.f) / ProjectionMatrix.r[0].m128_f32[0];
	_float NDCY = ((-2.f * ptMouse.y) / EngineOption.WindowResolutionY + 1.f) / ProjectionMatrix.r[1].m128_f32[1];

	// View Space 카메라(원점) 기준에서, 화면에 찍은 X, Y 좌표를 Z=1 평면에 찍어주어 방향 생성
	XMVECTOR RayOrigin		= XMVectorSet(0.f, 0.f, 0.f, 1.f);
	XMVECTOR RayDirection	= XMVectorSet(NDCX, NDCY, 1.f, 0.f);

	// 뷰 역행렬 곱해주어 월드 공간으로 변환
	WorldRayOrigin		= XMVector3TransformCoord(RayOrigin, InvViewMatrix);
	WorldRayDirection	= XMVector3TransformNormal(RayDirection, InvViewMatrix);
	WorldRayDirection	= XMVector3Normalize(WorldRayDirection);

	return Compute_RayCastObject();
}
shared_ptr<GameObject> RayCaster::Compute_RayCastObject() {
	_float MinimumDistance = FLT_MAX;

	uint32_t SceneNumb = GameInstance::GetInstance().Get_CurrentSceneIndex();

	for (auto& Recipient : RecipientList) {
		// 대상자들의 Transform 컴포넌트를 가져와서 WorldMatrix를 Inverse하여 "월드 좌표 역행렬"을 만듦.
		shared_ptr<Transform>	TransformCMP = static_pointer_cast<Transform>(Recipient->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		if (nullptr == TransformCMP)	continue;
		XMMATRIX InvWorld = XMMatrixInverse(0, TransformCMP->Get_WorldMatrix());
	
		// 월드 공간에 있는 RayOrigin, RayDirection을 월드 좌표 역행렬을 행렬곱 해주어, 로컬좌표로 Unproject
		LocalRayOrigin		= XMVector3TransformCoord(WorldRayOrigin, InvWorld);		// 벡터X행렬 연산을 해주고, 자동으로 W나누기 연산을 해준다.
		LocalRayDirection	= XMVector3TransformNormal(WorldRayDirection, InvWorld);
		LocalRayDirection	= XMVector3Normalize(LocalRayDirection);
	
		if (Control_Lock) {
			if (CastedObject == Recipient) return CastedObject;
			else continue;
		}

		_float Distance = 0.f;		// Ray에 맞은 물체와의 거리를 저장하는 변수
		shared_ptr<ParentCollider> ColliderBox = static_pointer_cast<Collider>(Recipient->Find_Component(COMPONENT_TYPE::COMPONENT_COLLIDER))->Get_ColliderBox();

		BoundingBox BBox = *static_pointer_cast<AABBCollider>(ColliderBox)->Get_ColliderBox().get();
		XMStoreFloat3(&BBox.Center, XMVector3TransformCoord(XMLoadFloat3(&BBox.Center), InvWorld));
		XMStoreFloat3(&BBox.Extents, XMVector3TransformNormal(XMLoadFloat3(&BBox.Extents), InvWorld));
		if (BBox.Intersects(LocalRayOrigin, LocalRayDirection, Distance)) {
			CastedObjectList.insert({ Distance, Recipient });
			if (Distance < MinimumDistance) {
				// 지금까지 맞은 오브젝트들 중 가장 가까운 오브젝트를 PickedObject로 선별하고, 거리를 갱신
				MinimumDistance = Distance;
				CastedObject = Recipient;
			}
		}
	}
	if (nullptr == CastedObject) return nullptr;

	shared_ptr<Transform>	TransformCMP = static_pointer_cast<Transform>(CastedObject->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	XMMATRIX InvWorld = XMMatrixInverse(0, TransformCMP->Get_WorldMatrix());
	LocalRayOrigin		= XMVector3TransformCoord(WorldRayOrigin, InvWorld);
	LocalRayDirection	= XMVector3TransformNormal(WorldRayDirection, InvWorld);
	LocalRayDirection	= XMVector3Normalize(LocalRayDirection); 
	
	return CastedObject;
}
shared_ptr<GameObject> RayCaster::Find_Recipient(const string& _OBJTAG) {
	for (auto& RCP : RecipientList)
		if (RCP->Get_ObjectTag() == _OBJTAG)	return RCP;

	return nullptr;
}
HRESULT RayCaster::Set_CastedObject(shared_ptr<GameObject> _GOBJ) {

	for (auto& OBJ : RecipientList) {
		if (OBJ == _GOBJ) {
			CastedObject = _GOBJ;
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT RayCaster::Delete_RayCastObject(){
	//RayCast_Collider();

	if (nullptr == CastedObject)		return E_FAIL;

	auto iter = find(RecipientList.begin(), RecipientList.end(), CastedObject);

	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Remove_GameObject(*iter)))		return E_FAIL;

	if (iter == RecipientList.end())	return E_FAIL;

	iter = RecipientList.erase(iter);

	CastedObject = nullptr;

	return S_OK;
}

HRESULT RayCaster::Select_NextRayCastedObject() {
	
	auto iterator = CastedObjectList.begin();
	for (; iterator != CastedObjectList.end(); ++iterator) {
		if (iterator->second == CastedObject) break;
	}

	if (iterator == CastedObjectList.end()) return E_FAIL;

	iterator = next(iterator);
	if(iterator == CastedObjectList.end())	iterator = CastedObjectList.begin();
	CastedObject = iterator->second;

	return S_OK;
}
VOID RayCaster::Reset_RayCasterRecipient() {
	RecipientList.clear();
}
#ifdef _DEBUG

VOID RayCaster::Render() {
	if (KEY_DOWN(DIK_F2)) {
		Render_RayCastLine == true ? Render_RayCastLine = false : Render_RayCastLine = true;
	}
	if (Render_RayCastLine) {
		GameInstance::GetInstance().Get_CollisionManager()->DebugRender_Begin();

		auto PrimitiveBatch = GameInstance::GetInstance().Get_CollisionManager()->Get_PrimitiveBatch();

		for (auto& Recipient : RecipientList) {

			XMVECTOR	 AreaColor = (Recipient == CastedObject ? DirectX::Colors::OrangeRed : DirectX::Colors::LightGreen);
			// 선택된 오브젝트 : 붉은색, 그 외 : 초록색
			static_pointer_cast<Collider>(Recipient->Find_Component(COMPONENT_TYPE::COMPONENT_COLLIDER))->Render_DebugLine(PrimitiveBatch, AreaColor);
		}
		GameInstance::GetInstance().Get_CollisionManager()->DebugRender_End();
	}
}
#endif
unique_ptr<RayCaster>	RayCaster::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	return unique_ptr<RayCaster>(new RayCaster(_GRPDEV, _DEVCTX));
}