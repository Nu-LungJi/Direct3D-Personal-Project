#pragma once
#include "Engine_Define.h"
#include "UIObject.h"

BEGIN(Engine)
class ENGINE_DLL UIManager {
private:
	UIManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~UIManager();

public:
	HRESULT Ready_UIManager();
	VOID	Update_UIManager(CONST FLOAT& _DT);
	VOID	LateUpdate_UIManager(CONST FLOAT& _DT);
	VOID	Render_UIManager();

	VOID	Create_UISprite(uint32_t _ULAYER, wstring _FileName, shared_ptr<UIObject> _SPRATT);

	static unique_ptr<UIManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>		 GRPDEV				= nullptr;
	ComPtr<ID3D11DeviceContext>	 DEVCTX				= nullptr;
	HWND						 hWnd;
								 
	shared_ptr<UIObject>		 Selected_Object	= nullptr;
	shared_ptr<UIObject>		 Dump_Object		= nullptr;
	shared_ptr<UIObject>		 Cursor_Object		= nullptr;
};
END
