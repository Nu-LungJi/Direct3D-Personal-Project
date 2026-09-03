#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ObjectManager {
private:
	ObjectManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~ObjectManager();

public:
	HRESULT		Initialize_ObjectManager();
	void		Priority_Update(_float fTimeDelta);
	void		Update(_float fTimeDelta);
	void		Late_Update(_float fTimeDelta);
	void		Clear_ObjectManager(uint32_t iClearLevelIndex);

public:
	HRESULT Add_GameObject_toLayer(uint32_t iPrototypeLevelIndex, const wstring& strPrototypeTag,
		uint32_t iLayerLevelIndex, const wstring& strLayerTag, void* pArg);

private:
	uint32_t		m_iNumLevels = {};
private:
	unique_ptr<map<const wstring, unique_ptr<class Layer>>[]>		m_pLayers = { nullptr };
	typedef map<const wstring, unique_ptr<class Layer>>					LAYERS;

private:
	class CLayer* Find_Layer(uint32_t iLayerLevelIndex, const wstring& strLayerTag);
private:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };
public:
	static unique_ptr<ObjectManager> Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
};

END