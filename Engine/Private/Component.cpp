#include "Component.h"

Component::Component(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX)					{ }
Component::Component(CONST Component& _PRTOBJ)											: GRPDEV(_PRTOBJ.GRPDEV), DEVCTX(_PRTOBJ.DEVCTX)	{ }
Component::~Component() {}