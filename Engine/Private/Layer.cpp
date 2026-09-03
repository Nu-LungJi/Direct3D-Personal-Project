#include "Layer.h"
#include "Scene.h"

Layer::~Layer() {}


VOID	Layer::Priority_Update(_float	_DT) {
	for (auto& GOBJ : GameObjectList) {
		if (GOBJ != nullptr) { GOBJ->Priority_Update(_DT); }
	}
}
VOID	Layer::Update(_float _DT) {
	for (auto& GOBJ : GameObjectList) 
		if (GOBJ != nullptr) { GOBJ->Update(_DT);			}
}
VOID	Layer::Late_Update(_float _DT) {
	for (auto& GOBJ : GameObjectList)
		if (GOBJ != nullptr) { GOBJ->Late_Update(_DT); }
}

HRESULT Layer::Add_GameObject(shared_ptr<GameObject> _GOBJ){
	if (nullptr == _GOBJ)	return E_FAIL;

	GameObjectList.push_back(_GOBJ);
	return S_OK;
}

HRESULT Layer::Clear_GameObjectList() {
	for (auto& GOBJ : GameObjectList) 
		GOBJ.reset();
	GameObjectList.clear();

	return S_OK;
}
shared_ptr<GameObject>	Layer::Get_GameObject(string _OBJTAG) {
	for (auto& GOBJ : GameObjectList) {
		if (GOBJ->Get_ObjectTag() == _OBJTAG)	return GOBJ;
	}
	return nullptr;
}

HRESULT Layer::Remove_GameObject(string _OBJTAG) {
	for (auto& OBJ : GameObjectList) {
		if (OBJ->Get_ObjectTag() == _OBJTAG) {
			GameObjectList.remove(OBJ);
			return S_OK;
		}
	}
	return E_FAIL;
}
HRESULT Layer::Remove_GameObject(shared_ptr<GameObject> _OBJ) {
	for (auto& OBJ : GameObjectList) {
		if (OBJ == _OBJ) {
			GameObjectList.remove(OBJ);
			return S_OK;
		}
	}
	return E_FAIL;
}


shared_ptr<Layer>	Layer::Create() {
	return unique_ptr<Layer>(new Layer());
}