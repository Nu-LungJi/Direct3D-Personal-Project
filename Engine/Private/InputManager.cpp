#include "InputManager.h"
#include "GameInstance.h"

InputManager::InputManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX)	: GRPDEV(_GRPDEV), DEVCTX(_DEVCTX), GamePad_State() {}
InputManager::~InputManager()	{}

HRESULT InputManager::Ready_InputManager() {
	ENGINE_DESC EngineOption = GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription();

	GamePad		= make_unique<DirectX::GamePad>	();

	ZeroMemory(KEYBOARDKEYSTATE, sizeof(KEYBOARDKEYSTATE));
	ZeroMemory(KeyState, sizeof(KeyState));
	
	if(FAILED(DirectInput8Create(EngineOption.hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&InputSDK, NULL)))	return E_FAIL;
	
	if (FAILED(InputSDK->CreateDevice(GUID_SysKeyboard, KeyBoardDEV.GetAddressOf(), nullptr)))	return E_FAIL;
	if (FAILED(InputSDK->CreateDevice(GUID_SysMouse, MouseDEV.GetAddressOf(), nullptr)))		return E_FAIL;
	
	KeyBoardDEV->SetDataFormat(&c_dfDIKeyboard);
	KeyBoardDEV->SetCooperativeLevel(EngineOption.hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	KeyBoardDEV->Acquire();
	
	MouseDEV->SetDataFormat(&c_dfDIMouse);
	MouseDEV->SetCooperativeLevel(EngineOption.hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	MouseDEV->Acquire();
	
	MouseClickedR = FALSE;
	MouseClickedL = FALSE;

	return S_OK;
}

VOID InputManager::Update_InputManager() {

	GamePad_Update();
	KeyBoard_Update();
	Mouse_Update();
}

_bool InputManager::KEY_STATE_DOWN(UCHAR _KEY) {
	if (!KEYBOARDKEYSTATE[_KEY] && (KeyState[_KEY] & 0x80)) {
		KEYBOARDKEYSTATE[_KEY] = !KEYBOARDKEYSTATE[_KEY];
		return TRUE;
	}
	return FALSE;
}
_bool InputManager::KEY_STATE_UP(UCHAR _KEY) {
	if (KEYBOARDKEYSTATE[_KEY] && !(KeyState[_KEY] & 0x80)) {
		KEYBOARDKEYSTATE[_KEY] = !KEYBOARDKEYSTATE[_KEY];
		return TRUE;
	}
	return FALSE;
}
_bool InputManager::MOUSE_LB_DOWN() {
	if (MouseClickedL == FALSE && Get_MouseState(MOUSEKEYSTATE::MKS_LB) & 0x80) {
		MouseClickedL = true;
		return true;
	}
	else if ((Get_MouseState(MOUSEKEYSTATE::MKS_LB) & 0x80) == false) {
		MouseClickedL = false;
	}

	return false;
}
_bool InputManager::MOUSE_RB_DOWN() {
	if (MouseClickedR == FALSE && Get_MouseState(MOUSEKEYSTATE::MKS_RB) & 0x80) {
		MouseClickedR = TRUE;
		return TRUE;
	}
	else if ((Get_MouseState(MOUSEKEYSTATE::MKS_RB) & 0x80) == FALSE) {
		MouseClickedR = false;
	}
	return FALSE;
}

_bool InputManager::MOUSE_LB_PRESSED() {
	return (Get_MouseState(MOUSEKEYSTATE::MKS_LB) & 0x80) != 0;
}
_bool InputManager::MOUSE_RB_PRESSED() {
	return (Get_MouseState(MOUSEKEYSTATE::MKS_RB) & 0x80) != 0;
}

VOID InputManager::GamePad_Update()		{
	GamePad_State = GamePad->GetState(0);

	if (GamePad_State.IsConnected() == TRUE)	{ GamePad_Tracker.Update(GamePad_State);	}
	else										{ GamePad_Tracker.Reset();					}
}
VOID InputManager::KeyBoard_Update()	{
	for (int i = 0; i < 256; ++i) {
		if (KEYBOARDKEYSTATE[i] && !(KeyState[i] & 0x80))
			KEYBOARDKEYSTATE[i] = !KEYBOARDKEYSTATE[i];
	
		if (!KEYBOARDKEYSTATE[i] && (KeyState[i] & 0x80))
			KEYBOARDKEYSTATE[i] = !KEYBOARDKEYSTATE[i];
	}
	
	KeyBoardDEV->GetDeviceState(256, KeyState);
	
}
VOID InputManager::Mouse_Update()		{
	if (!(Get_MouseState(MOUSEKEYSTATE::MKS_LB) & 0x80)) MouseClickedL = FALSE;
	
	if (!(Get_MouseState(MOUSEKEYSTATE::MKS_RB) & 0x80)) MouseClickedR = FALSE;
	
	MouseDEV->GetDeviceState(sizeof(MouseState), &MouseState);
}
unique_ptr<InputManager>	InputManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<InputManager>(new InputManager(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Ready_InputManager())) {
		MSG_BOX("Cannot Create InputManager.");
		return nullptr;
	}
	return Instance;
}