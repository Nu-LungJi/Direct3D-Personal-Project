#include "ProtoManager.h"

ProtoManager::ProtoManager()	{}
ProtoManager::~ProtoManager()	{
	
}

HRESULT ProtoManager::Ready_ProtoManager(uint32_t _Level) {
	GameObjectList	= unique_ptr<unordered_map<string, unique_ptr<GameObject>>[]>			(new unordered_map<string		 , unique_ptr<GameObject>>[_Level]);
	ComponentList	= unique_ptr<unordered_map<COMPONENT_TYPE, unique_ptr<Component>>[]>	(new unordered_map<COMPONENT_TYPE, unique_ptr<Component >>[_Level]);

	return S_OK;
}

HRESULT ProtoManager::Add_ProtoType(uint32_t _Level, const string& _PRTTAG	, unique_ptr<GameObject> _PRTOBJ) {
	if (nullptr != Find_ProtoType(_Level, _PRTTAG))	return E_FAIL;
	GameObjectList.get()[_Level].emplace(_PRTTAG, move(_PRTOBJ));
	return S_OK;
}
HRESULT	ProtoManager::Add_ProtoType(uint32_t _Level, COMPONENT_TYPE _CMTYPE , unique_ptr<Component>  _PRTCOM) {
	if (nullptr != Find_ProtoType(_Level, _CMTYPE))	return E_FAIL;
	_PRTCOM->Set_ComponentType(_CMTYPE);
	ComponentList.get()[_Level].emplace(_CMTYPE, move(_PRTCOM));
	return S_OK;
}

HRESULT ProtoManager::Clear_ProtoType(uint32_t _Level) {

	for (auto& Pair : GameObjectList[_Level])
		Pair.second.reset();
	
	GameObjectList[_Level].clear();

	return S_OK;
}

shared_ptr<GameObject> ProtoManager::Clone_ProtoType(uint32_t _Level, const string& _PRTTAG, const string& _NEWTAG, VOID* _ARG){
	auto Instance = Find_ProtoType(_Level, _PRTTAG);
	if (nullptr == Instance)	return nullptr;
	shared_ptr<GameObject>	GOBJ = Instance->Clone(_ARG);
	GOBJ->Set_ObjectTag(_NEWTAG);
	GOBJ->Set_AssetPath(_PRTTAG);

	return GOBJ;
} 
shared_ptr<Component>  ProtoManager::Clone_ProtoType(uint32_t _Level, COMPONENT_TYPE _CMTYPE, VOID* _ARG) {
	auto Instance = Find_ProtoType(_Level, _CMTYPE);
	if (nullptr == Instance)
		return nullptr;

	return Instance->Clone(_ARG);
}

GameObject* ProtoManager::Find_ProtoType(uint32_t _Level, const string& _PRTTAG) {
	auto iter = GameObjectList.get()[_Level].find(_PRTTAG);
	if (iter == GameObjectList.get()[_Level].end())	return nullptr;

	return iter->second.get();
}
Component*  ProtoManager::Find_ProtoType(uint32_t _Level, COMPONENT_TYPE _CMTYPE) {
	auto iter = ComponentList.get()[_Level].find(_CMTYPE);
	if (iter == ComponentList.get()[_Level].end())	return nullptr;
	
	return iter->second.get();
}

unique_ptr<ProtoManager>	ProtoManager::Create(uint32_t _Level) {
	auto PM = unique_ptr<ProtoManager>(new ProtoManager);
	if (FAILED(PM->Ready_ProtoManager(_Level))) {
		MSG_BOX("Cannot Create ProtoManager");
		return nullptr;
	}
	return PM;
}