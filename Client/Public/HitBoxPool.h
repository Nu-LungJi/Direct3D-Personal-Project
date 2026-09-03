#pragma once
#include "GameObject.h"
#include "PlayerHitBox.h"

#define MAX_HITBOXPOOL_SIZE 50

class HitBoxPool : public GameObject {
private:
	HitBoxPool(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	HitBoxPool(CONST HitBoxPool& _PRTOBJ);
public:
	virtual ~HitBoxPool();

public:
	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);
	virtual VOID		Update(CONST _float& _DT);
	virtual	VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

public:
	VOID Generate_HitBox(XMVECTOR _Position, XMVECTOR _Rotation, XMVECTOR _StartScale, XMVECTOR _EndScale, _float _Duration, _float _AppearTimeOffset, _float _AfterDurationTime, _float _Damage, _float _DFTTimeOffset, FLOWTYPE _FTYPE = FLOWTYPE::LINEAR);

public:
	static	 unique_ptr<HitBoxPool>	Create(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual  shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	vector<shared_ptr<PlayerHitBox>>		HitBoxList;
};

