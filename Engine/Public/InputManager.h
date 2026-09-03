#pragma once
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL InputManager {
private:
	InputManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~InputManager();

public:
	HRESULT Ready_InputManager();
	VOID	Update_InputManager();

	static unique_ptr<InputManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

public:
	//////////////////////////////////////////////////////////////////// GamePad	Input
	
	// 패드 입력 (입력 시작(PRESSED), 유지(HELD), 종료(RELEASED))
	// { A, B, X, Y } + Trigger(LEFT, RIGHT) + DPad(UP, DOWN, LEFT, RIGHT) + Shoulder(LEFT, RIGHT) + StickBTN(UP, DOWN, LEFT, RIGHT)
	_bool	GPBTN_Pressed	(GamePad::ButtonStateTracker::ButtonState _BTN) { return _BTN == GamePad::ButtonStateTracker::PRESSED;	}
	_bool	GPBTN_Held		(GamePad::ButtonStateTracker::ButtonState _BTN) { return _BTN == GamePad::ButtonStateTracker::HELD;		}
	_bool	GPBTN_Released	(GamePad::ButtonStateTracker::ButtonState _BTN) { return _BTN == GamePad::ButtonStateTracker::RELEASED; }

	// ThumbStick 기울기 X, Y ( -1.f ~ 1.f )
	_float	Get_LThumbStickAngleX()		{ return  GamePad_State.thumbSticks.leftX;  }
	_float	Get_LThumbStickAngleY()		{ return  GamePad_State.thumbSticks.leftY;  }
															   
	_float	Get_RThumbStickAngleX()		{ return  GamePad_State.thumbSticks.rightX; }
	_float	Get_RThumbStickAngleY()		{ return  GamePad_State.thumbSticks.rightY; }

	// 패드 연결 확인
	_bool	Check_GamePadConnection()	{ return GamePad_State.IsConnected();		 }

	//////////////////////////////////////////////////////////////////// KeyBoard Input
	
	// 키보드 입력 (입력 시작(PRESSED), 유지(HELD), 종료(RELEASED))
	_bool	KEY_STATE_HOLD(uint8_t _KEY) { return KeyState[_KEY] & 0x80 ? true : false; }
	_bool	KEY_STATE_DOWN(uint8_t _KEY);
	_bool	KEY_STATE_UP  (uint8_t _KEY);

	//////////////////////////////////////////////////////////////////// Mouse	Input
	
	// 마우스 입력 (입력 시작(PRESSED), 유지(HELD), 종료(RELEASED))
	_bool	MOUSE_LB_DOWN();
	_bool	MOUSE_RB_DOWN();

	_bool	MOUSE_LB_PRESSED();
	_bool	MOUSE_RB_PRESSED();

	int8_t	Get_KeyState(uint8_t _KID)			{ return KeyState[_KID]; }
	int8_t	Get_MouseState(MOUSEKEYSTATE _MID)	{ return MouseState.rgbButtons[(uint32_t)_MID]; }
	LONG	Get_MouseMove(MOUSEMOVESTATE _MID)	{ return *(((long*)&MouseState) + (uint32_t)_MID); }

private:
	VOID	GamePad_Update();		// 게임 패드 상태 Update
	VOID	KeyBoard_Update();		// 키보드 상태 Update
	VOID	Mouse_Update();			// 마우스 상태 Update

private:
	ComPtr<ID3D11Device>			GRPDEV		= { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX		= { nullptr };

	ComPtr<IDirectInput8>			InputSDK	= { nullptr };
	ComPtr<IDirectInputDevice8>		KeyBoardDEV = { nullptr };
	ComPtr<IDirectInputDevice8>		MouseDEV	= { nullptr };

	unique_ptr<DirectX::GamePad	>	GamePad			= { nullptr };
	GamePad::ButtonStateTracker		GamePad_Tracker;
	GamePad::State					GamePad_State;

	_bool							KEYBOARDKEYSTATE[256];
	CHAR							KeyState[256];
	vector<KeyINFO>					KeySet;

	DIMOUSESTATE					MouseState;
	_bool							MouseClickedR;
	_bool							MouseClickedL;
};

END