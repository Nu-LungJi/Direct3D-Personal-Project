#pragma once
#include "Component.h"
#include "Bone.h"
#include "Animation.h"
#include "Transform.h"
#include "FB_Animator_generated.h"

BEGIN(Engine)
class Animation;
class Transform;
class MeshLoader;

class ENGINE_DLL Animator : public Component {
private:
	Animator(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Animator(CONST Animator& _PRTOBJ);
public:
	~Animator() = default;
	
public:
	virtual HRESULT	Initialize_ProtoType();
	virtual HRESULT	Initialize(VOID* _ARG);

	VOID			Update_Animation(const _float& _DT);
	VOID			Update_SingleAnimation(const _float& _DT);
	VOID			Update_BlendAnimation(const _float& _DT);
	VOID			Update_RootMotion(const _float& _DT);

	VOID			Play_Animation(uint32_t _Index, _bool _AnimationLoop = false, _float _AnimSpeed = 1.f);
	VOID			Play_Animation(shared_ptr<Animation> _Animation, _bool _AnimationLoop = false, _float _AnimSpeed = 1.f);

	VOID			Register_TransformComponent(shared_ptr<Transform>& _CMPTRS)			{ Component_Transform = _CMPTRS;	}
	VOID			Register_NavMeshAgentComponent(shared_ptr<NavMeshAgent>& _CMPNAV)	{ Component_NavMeshAgent = _CMPNAV; }

public:
	vector<shared_ptr<Animation>>&	Get_AnimationList()						{ return AnimationList;			}
	shared_ptr<Animation>&			Get_AnimationByIndex(uint32_t _Index)	{ return AnimationList[_Index]; }

	shared_ptr<Animation>			Get_CurrentAnimation()					{ return CurrentAnimation;		}
	shared_ptr<Animation>			Get_PreviousAnimation()					{ return PreviousAnimation;		}

	uint32_t						Get_CurrentAnimationIndex()				{ return CurrentAnimationIndex; }

public:
	VOID							Set_AnimationLoop(ANIMATION_LOOP _Value){ Animation_Loop = _Value;		}
	ANIMATION_LOOP					Get_AnimationLoop()						{ return Animation_Loop;		}

	VOID							Set_AnimationBlending(_bool _Value)		{ Animation_Blending = _Value;	}
	_bool							Get_AnimationBlending()					{ return Animation_Blending;	}
	
	VOID							Set_AnimationBlendTime(_float _Value)	{ Animation_BlendTime = _Value;	}
	_float							Get_AnimationBlendTime()				{ return Animation_BlendTime;	}

	ANIMATION_STATE					Get_AnimationState(uint32_t _AnimIndex)								{ return AnimationList[_AnimIndex]->Get_AnimationState();	 }
	VOID							Set_AnimationState(uint32_t _AnimIndex, ANIMATION_STATE _AnimState) { AnimationList[_AnimIndex]->Set_AnimationState(_AnimState); }
	
	VOID							Set_ComponentOwner(shared_ptr<GameObject> _Owner)	{ Component_Owner = _Owner; }
	shared_ptr<GameObject>			Get_ComponentOwner()								{ return Component_Owner;	}

	shared_ptr<Bone>				Get_ModelRootBone()						{ return ModelRootBone;			}

	VOID							Set_MovementAmount(_float _Amount)		{ MovementAmount = { _Amount, _Amount, _Amount }; }
	VOID							Set_MovementAmount(_float _XAmount, _float _YAmount, _float _ZAmount) { MovementAmount = { _XAmount, _YAmount, _ZAmount }; }
	XMFLOAT3						Get_MovementAmount()					{ return MovementAmount;		}

	VOID				Stop_Animation() { Animation_Loop = ANIMATION_LOOP::PAUSED; }
	VOID				Resume_PlayBack(ANIMATION_LOOP _Loop = ANIMATION_LOOP::PLAY_ONCE) { Animation_Loop = _Loop; }

public:
	flatbuffers::Offset<FB_Animator::Animator> Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder);
	
	HRESULT		Decompose_Animator(shared_ptr<MeshLoader> _Loader);
	HRESULT		Decompose_BinaryData(const flatbuffers::Vector<flatbuffers::Offset<FB_Animation::Animation>>* _AnimationList, shared_ptr<MeshLoader> _Loader);
	
public:
	virtual shared_ptr<Component>	Clone(VOID* _ARG);
	static  unique_ptr<Animator>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	vector<shared_ptr<Animation>>	AnimationList;
	shared_ptr<Animation>			CurrentAnimation		= { nullptr };
	shared_ptr<Animation>			PreviousAnimation		= { nullptr };
	uint32_t						CurrentAnimationIndex	= { 0 };

	string							AnimationName;
	ANIMATION_LOOP					Animation_Loop			= { ANIMATION_LOOP::PLAY_LOOP };
	_bool							Animation_Blending		= { false	};
	_float							Animation_BlendWeight	= {	  0.f	};
	_float							Animation_BlendTime		= {   0.f   };
	_float							Animation_PrevAnimTime	= {	  0.f	};
	vector<shared_ptr<Bone>>		ModelBoneList;
	shared_ptr<Bone>				ModelRootBone			= { nullptr };

	shared_ptr<Transform>			Component_Transform		= { nullptr };
	shared_ptr<NavMeshAgent>		Component_NavMeshAgent  = { nullptr };
	_bool							FirstFrameCheck			= { false	};
	XMFLOAT3						MovementAmount			= {			};
	shared_ptr<GameObject>			Component_Owner			= { nullptr };
};
END