#pragma once
#include "Engine_Define.h"
#include "Channel_Ad.h"

BEGIN(Engine)
class ENGINE_DLL Animation_Ad {
private:
	Animation_Ad();
	Animation_Ad(const Animation_Ad& _PRTOBJ);
public:
	~Animation_Ad();

public:
	HRESULT Initialize(const aiAnimation* _Animation, class MeshLoader_Ad* _ModelLoader);
	HRESULT Update_Animation(_float _DT, const vector<shared_ptr<Bone_Ad>>& _BoneList, _bool _Looping);

	static	shared_ptr<Animation_Ad> Create(const aiAnimation* _Animation, class MeshLoader_Ad* _ModelLoader);
	shared_ptr<Animation_Ad> Clone() { return shared_ptr<Animation_Ad>(new Animation_Ad (*this)); }
private:
	_float		Duration;
	_float		TicksPerSecond;
	_float		AnimationTime;
	_float		PlaySpeed;
	_float		CurrentTime;

	uint32_t			ChannelCount;
	vector<shared_ptr<Channel_Ad>>	ChannelList;
	vector<uint32_t>	CurrentKeyFrameIndexList;
};
END