#pragma once
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL Layer {
private:
	Layer() = default;
public:
	virtual ~Layer();

public:
	VOID		Priority_Update(_float	_DT);
	VOID		Update(_float _DT);
	VOID		Late_Update(_float _DT);

	HRESULT		Add_GameObject(shared_ptr<GameObject> _GOBJ);
	HRESULT		Clear_GameObjectList();

	shared_ptr<GameObject>			Get_GameObject(string _OBJTAG);
	list<shared_ptr<GameObject>>	Get_GameObjectList() { return GameObjectList; }

	HRESULT		Remove_GameObject(string _OBJTAG);
	HRESULT		Remove_GameObject(shared_ptr<GameObject> _OBJ);
private:
	list<shared_ptr<GameObject>>	GameObjectList;

public:
	static shared_ptr<Layer>	Create();
};
END