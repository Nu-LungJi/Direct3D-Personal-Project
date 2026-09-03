#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "MeshLoader.h"
#include "Shader.h"
#include "Collider.h"
#include "NavMeshAgent.h"

BEGIN(Engine)
class ENGINE_DLL MapObject : public GameObject {
private:
	MapObject(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	MapObject(CONST MapObject& _PRTOBJ);
public:
	virtual ~MapObject() = default;

public:
	HRESULT		Initialize_ProtoType(const wstring& _FilePath, uint32_t _LevelIndex);
	virtual		HRESULT		Initialize(VOID* _ARG);
	virtual		VOID		Update(CONST _float& _DT);
	virtual		VOID		Late_Update(CONST _float& _DT);
	virtual		HRESULT		Render();

	shared_ptr<MeshLoader>	Get_ModelComponent()	 { return Component_Model;		}
	shared_ptr<Transform>	Get_TransformComponent() { return Component_Transform;	}
	shared_ptr<Shader>		Get_ShaderComponent()	 { return Component_Shader; }

public:
	static		unique_ptr<MapObject>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const wstring& _FilePath, uint32_t _LevelIndex);
	virtual		shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<MeshLoader>		Component_Model;
	shared_ptr<Transform>		Component_Transform;
	shared_ptr<Collider>		Component_Collider;
	shared_ptr<Shader>			Component_Shader;
	shared_ptr<NavMeshAgent>	Component_Navigation;
};
END