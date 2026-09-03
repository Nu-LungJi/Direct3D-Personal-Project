#pragma once

class GameManager {
private:
	GameManager();
public:
	virtual ~GameManager();

public:
	HRESULT		Ready_GameManager();
	VOID		Priority_Update_GameManager(CONST FLOAT& _DT);
	VOID		Update_GameManager(CONST FLOAT& _DT);
	VOID		LateUpdate_GameManager(CONST FLOAT& _DT);
	VOID		Render_GameManager();

	HRESULT			Loading_GameManger();
	LOADING_STATE	Check_LoadingState();

private:
	HRESULT		Ready_DefaultSetting();
	HRESULT		Ready_LoadingScreen(uint32_t _StartScene);
	HRESULT		Ready_SoundList();
	HRESULT		Register_AllScene();

	HRESULT		Loading_MainScene();
	HRESULT		Loading_BossScene();



	shared_ptr<FontObject>			Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha);
	shared_ptr<UIObject>			Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation = 0.f);
	shared_ptr<UIObject>			Create_FilterObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation = 0.f);
	
public:
	static unique_ptr<GameManager> Create();
	
private:
	ComPtr<ID3D11Device>			GRPDEV;
	ComPtr<ID3D11DeviceContext>		DEVCTX;

	HANDLE							Handle;
	CRITICAL_SECTION				CRTSCT;
	uint32_t						SCENETYPE = { 0 };

	shared_ptr<UIObject>			Loading_Screen = { nullptr };
	shared_ptr<UIObject>			ScreenFilter = { nullptr };
	shared_ptr<FontObject>			Loading_Text = { nullptr };

	vector<shared_ptr<UIObject>>	UIObjectList;
	vector<shared_ptr<FontObject>>	FontObjectList;
	vector<shared_ptr<UIObject>>	FilterObjectList;
};