#pragma once
#include "Component.h"
#include "NavigationCell.h"
#include "Shader.h"

BEGIN(Engine)
class ENGINE_DLL Navigation : public Component {
private:
	Navigation(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Navigation(CONST Navigation& _PRTOBJ);
public:
	virtual ~Navigation() = default;

public:
	virtual HRESULT	Initialize_ProtoType() override;
	virtual HRESULT	Initialize(VOID* _ARG) override;

public:
	virtual shared_ptr<Component>	Clone(VOID* _ARG);
	static  unique_ptr<Navigation>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	vector<shared_ptr<NavigationCell>>	CellList;

#ifdef _DEBUG
public:
	HRESULT Render_Navigation_CellList();
private:
	shared_ptr<Shader>	Component_Shader = { nullptr };
#endif
};
END