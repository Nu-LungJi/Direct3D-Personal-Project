#include "GameObject.h"

GameObject::GameObject(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX): GRPDEV(_GRPDEV), DEVCTX(_DEVCTX), OBJTAG("") {
	ComponentList.resize((uint32_t)COMPONENT_TYPE::COMPONENT_END);
}
GameObject::GameObject(CONST GameObject& _PRTOBJ)
	: GRPDEV(_PRTOBJ.GRPDEV), DEVCTX(_PRTOBJ.DEVCTX), OBJTAG(_PRTOBJ.OBJTAG), AssetPath(_PRTOBJ.AssetPath){ 
	ComponentList.resize((uint32_t)COMPONENT_TYPE::COMPONENT_END);
	for (uint32_t IDX = 0; IDX < ComponentList.size();++IDX) {
		if (nullptr == _PRTOBJ.ComponentList[IDX])	continue;
		ComponentList[IDX] = _PRTOBJ.ComponentList[IDX]->Clone(nullptr);
	}
}

shared_ptr<Component> GameObject::Find_Component(COMPONENT_TYPE _CTYPE) {
	if (nullptr != ComponentList[(uint32_t)_CTYPE]) return ComponentList[(uint32_t)_CTYPE];
	return nullptr;
}
