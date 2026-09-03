#include "EffectManager.h"
#include "GameInstance.h"

EffectManager::EffectManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX){}
EffectManager::~EffectManager() { Release_EffectManager(); }

HRESULT EffectManager::Ready_EffectManager() {
	EffekSeerRenderer = EffekseerRendererDX11::Renderer::Create(GRPDEV.Get(), DEVCTX.Get(), 1000);
	EffekSeerManager  = Effekseer::Manager::Create(1000);

	EffekSeerManager->SetSpriteRenderer(EffekSeerRenderer->CreateSpriteRenderer());
	EffekSeerManager->SetRibbonRenderer(EffekSeerRenderer->CreateRibbonRenderer());
	EffekSeerManager->SetRingRenderer(EffekSeerRenderer->CreateRingRenderer());
	EffekSeerManager->SetModelRenderer(EffekSeerRenderer->CreateModelRenderer());

	EffekSeerManager->SetTextureLoader(EffekSeerRenderer->CreateTextureLoader());
	EffekSeerManager->SetModelLoader(EffekSeerRenderer->CreateModelLoader());
	EffekSeerManager->SetMaterialLoader(EffekSeerRenderer->CreateMaterialLoader());

	EffekSeerManager->LaunchWorkerThreads(4);

	return S_OK;
}

VOID EffectManager::Update_EffectManager(const _float& _DT)
{
	EffekSeerManager->GetTotalInstanceCount() > 0 ? Render_Flag = true : Render_Flag = false;
	//if (Render_Flag || EffectPlay_Flag) {
		EffekSeerManager->Update(_DT);
	//}
	//if (!EffekSeerManager->Exists(EffectHandle)) {
	//	EffectHandle = EffekSeerManager->Play(EffectProtoList[0], 1.58f, 46.5f, -61.45f);
	//}
}

HRESULT EffectManager::Load_EffectBundle(const filesystem::path& _FolderPath) {
	for (const auto& File_iterator : filesystem::recursive_directory_iterator(_FolderPath)) {
		if (File_iterator.is_regular_file() == FALSE)    continue;

		if (File_iterator.path().extension().string() == ".efkefc") {
			string FileName = File_iterator.path().stem().string();
			Effekseer::EffectRef Effect = Effekseer::Effect::Create(EffekSeerManager, File_iterator.path().u16string().c_str());
			EffectProtoList.insert({ FileName, Effect });
		}
	}

	return S_OK;
}

void EffectManager::Render_EffectManager() {
	//if (Render_Flag || EffectPlay_Flag) {
		const XMMATRIX VIEW = GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix();
		const XMMATRIX PROJ = GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix();

		EffekSeerRenderer->SetCameraMatrix(*(Effekseer::Matrix44*)&VIEW);
		EffekSeerRenderer->SetProjectionMatrix(*(Effekseer::Matrix44*)&PROJ);

		EffekSeerRenderer->BeginRendering();
		EffekSeerManager->Draw();
		EffekSeerRenderer->EndRendering();
	//}
}

void EffectManager::Release_EffectManager() {
	for (auto& Effect : EffectProtoList) {
		Effect.second->Release();
	}
	EffekSeerManager->Release();
	EffekSeerRenderer->Release();
}

void EffectManager::Play_Effect(string _EffectName, XMFLOAT3 _Position, XMFLOAT3 _Rotation, XMFLOAT3 _Scale, _float _Speed) {
	Effekseer::EffectRef ERef = nullptr;
	for (auto& Effect : EffectProtoList) {
		if (Effect.first == _EffectName) {
			ERef = Effect.second;
			break;
		}
	}
	EffectHandle = EffekSeerManager->Play(ERef, _Position.x, _Position.y, _Position.z);
	EffekSeerManager->SetRotation(EffectHandle, _Rotation.x, _Rotation.y, _Rotation.z);
	EffekSeerManager->SetScale(EffectHandle, _Scale.x, _Scale.y, _Scale.z);
	EffekSeerManager->SetSpeed(EffectHandle, _Speed);

	EffectPlay_Flag = true;
}

unique_ptr<EffectManager>	EffectManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto	EM = unique_ptr<EffectManager>(new EffectManager(_GRPDEV, _DEVCTX));
	if (FAILED(EM->Ready_EffectManager())) {
		MSG_BOX("Cannot Create EffectManager.");
		return nullptr;
	}
	return EM;
}