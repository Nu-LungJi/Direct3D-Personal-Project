#pragma once
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL FileManager {
private:
	FileManager();
public:
	virtual ~FileManager();

public:
	static unique_ptr<FileManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
};
END
