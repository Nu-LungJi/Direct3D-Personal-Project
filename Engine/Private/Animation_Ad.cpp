#include "Animation_Ad.h"
Animation_Ad::Animation_Ad()    {}
Animation_Ad::Animation_Ad(const Animation_Ad& _PRTOBJ) : Duration(_PRTOBJ.Duration), TicksPerSecond(_PRTOBJ.TicksPerSecond), 
AnimationTime(_PRTOBJ.AnimationTime), PlaySpeed(_PRTOBJ.PlaySpeed), CurrentTime(_PRTOBJ.CurrentTime), ChannelCount(_PRTOBJ.ChannelCount),
ChannelList(_PRTOBJ.ChannelList), CurrentKeyFrameIndexList(_PRTOBJ.CurrentKeyFrameIndexList) {}
Animation_Ad::~Animation_Ad()   {}

HRESULT Animation_Ad::Initialize(const aiAnimation* _Animation, MeshLoader_Ad* _ModelLoader) {
	Duration		= _Animation->mDuration;
	TicksPerSecond	= _Animation->mTicksPerSecond;

	ChannelCount	= _Animation->mNumChannels;
	CurrentKeyFrameIndexList.resize(ChannelCount);

	for (uint32_t IDX = 0; IDX < ChannelCount; ++IDX) {
		auto Channel = Channel_Ad::Create(_Animation->mChannels[IDX], _ModelLoader);
		if (nullptr == Channel) return E_FAIL;

		ChannelList.push_back(Channel);
	}

    return S_OK;
}

HRESULT Animation_Ad::Update_Animation(_float _DT, const vector<shared_ptr<Bone_Ad>>& _BoneList, _bool _Looping) {
	CurrentTime += TicksPerSecond * _DT;
	if (CurrentTime >= Duration) {
		if (!_Looping)	return S_OK;

		CurrentTime = 0.f;
	}

	for (uint32_t Channel = 0; Channel < ChannelCount; ++Channel) {
		ChannelList[Channel]->Update_TransformMatrix(_BoneList, &CurrentKeyFrameIndexList[Channel], CurrentTime);
	}
	
	return E_FAIL;
}

shared_ptr<Animation_Ad> Animation_Ad::Create(const aiAnimation* _Animation, MeshLoader_Ad* _ModelLoader) {
	auto Instance = shared_ptr<Animation_Ad>(new Animation_Ad());
	if (FAILED(Instance->Initialize(_Animation, _ModelLoader))) {
		MSG_BOX("Cannot Create Mesh");
		return nullptr;
	}
	return Instance;
}
