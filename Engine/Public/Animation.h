#pragma once
#include "Engine_Define.h"
#include "FB_Animation_generated.h"

BEGIN(Engine)
class MeshLoader;
class Bone;
class Channel;
class Animator;

class ENGINE_DLL Animation	{
private:
	Animation();
public:
	~Animation();


public:
	HRESULT			Initialize();
	HRESULT			Update_Animation(const _float& _DT, ANIMATION_LOOP& _AnimationLoop, Animator* _This);

	HRESULT			Decompose_Animation(const aiAnimation* _Animation, shared_ptr<MeshLoader>& _Loader);
	HRESULT			Decompose_Animation(const FB_Animation::Animation* _Animation, shared_ptr<MeshLoader>& _Loader);

	_float			Get_Duration()								{ return Duration;			}
	_float			Get_TicksPerSecond()						{ return TicksPerSecond;	}
	const string&	Get_AnimationName()							{ return AnimationName;		}
	_float			Get_AnimationProgress()						{ return CurrentTime / Duration; }

	VOID			Set_CurrentTime(_float _Time)				{ CurrentTime = _Time;		}
	_float			Get_CurrentTime()							{ return CurrentTime;		}

	VOID			Set_AnimationSpeed(_float _Speed) { PlaySpeed = _Speed; }
	_float			Get_AnimationSpeed() { return PlaySpeed; }

	VOID			Generate_BoneMappingTable(vector<shared_ptr<Bone>>& _BoneList);

	VOID			Set_AnimationState(ANIMATION_STATE _State)	{ AnimationState = _State;	}
	ANIMATION_STATE	Get_AnimationState()						{ return AnimationState;	}

	VOID			Set_AnimationIndex(uint32_t _Index)			{ AnimationIndex = _Index;	}
	uint32_t		Get_AnimationIndex()						{ return AnimationIndex;	}

	vector<shared_ptr<Channel>>& Get_ChannelList()				{ return ChannelList; }

	BoneTransform	Get_BoneTransform(int32_t _BoneIndex, _float _CurrentTime, vector<shared_ptr<Bone>>& _BoneList);
	flatbuffers::Offset<FB_Animation::Animation> Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder);

	static shared_ptr<Animation> Create();

private:
	_float		Duration		= { 0.f };
	_float		TicksPerSecond  = { 0.f };
	_float		PlaySpeed		= { 1.f };
	_float		CurrentTime		= { 0.f };

	ANIMATION_STATE	AnimationState	= ANIMATION_STATE::ONREADY;

	vector<shared_ptr<Channel>>	ChannelList;
	uint32_t					ChannelCount = { 0 };

	string						AnimationName;
	uint32_t					AnimationIndex = { 0 };
	vector<int>					BoneMappingTable;
};
END