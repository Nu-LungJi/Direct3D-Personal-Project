#pragma once
#include "Engine_Define.h"
#include "GameObject.h"
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL ProtoManager {
private:
	ProtoManager();
public:
	virtual ~ProtoManager();

public:
	HRESULT			Ready_ProtoManager(uint32_t _Level);
	HRESULT			Add_ProtoType(uint32_t _Level, const string& _PRTTAG, unique_ptr<GameObject> _PRTOBJ);
	HRESULT			Add_ProtoType(uint32_t _Level, COMPONENT_TYPE _CMTYPE, unique_ptr<Component>  _PRTCOM);
	HRESULT			Clear_ProtoType(uint32_t _Level);
	
	shared_ptr<GameObject>	Clone_ProtoType(uint32_t _Level, const string& _PRTTAG, const string& _NEWTAG, VOID* _ARG);
	shared_ptr<Component >	Clone_ProtoType(uint32_t _Level, COMPONENT_TYPE _CMTYPE, VOID* _ARG);

	static			unique_ptr<ProtoManager>	Create(uint32_t _Level);
private:
	GameObject* Find_ProtoType(uint32_t _Level, CONST string& _PRTTAG);
	Component*	Find_ProtoType(uint32_t _Level, COMPONENT_TYPE _CMTYPE);
	
private:
	unique_ptr<unordered_map<string, unique_ptr<GameObject>>[]>			GameObjectList = { nullptr };
	unique_ptr<unordered_map<COMPONENT_TYPE, unique_ptr<Component >>[]>	ComponentList  = { nullptr };
};
END