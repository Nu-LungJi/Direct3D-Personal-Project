#pragma once
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL Entity {
protected:
	Entity() = default;
public:
	virtual ~Entity() = default;

public:
	virtual shared_ptr<Entity> Clone(void* _ARG) = 0;
};
END