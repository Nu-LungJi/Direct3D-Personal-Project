#include "Animation.h"
#include "Bone.h"
#include "MeshLoader.h"
#include "Channel.h"

Animation::Animation() {}
Animation::~Animation() {}

// aiAnimation : Run, Walk, Attack 같은 하나의 애니메이션 파일이, 하나의 aiAnimation와 대응된다.
// aiNodeAnim의 배열을 갖고 있다. 즉, 한 캐릭터가 애니메이션이 10개가 있다면 7번째는 Scene->mAnimations[6]이다.

HRESULT		Animation::Initialize() {
	return S_OK;
}
HRESULT Animation::Update_Animation(const _float& _DT, ANIMATION_LOOP& _AnimationLoop, Animator* _This) {
	CurrentTime += TicksPerSecond * _DT * PlaySpeed;

	if (CurrentTime >= Duration) {
		if (_AnimationLoop == ANIMATION_LOOP::PLAY_ONCE) {
			CurrentTime = Duration;
			_AnimationLoop = ANIMATION_LOOP::PAUSED;
			AnimationState = ANIMATION_STATE::FINISHED;
			return S_OK;
		}
		else if (_AnimationLoop == ANIMATION_LOOP::PLAY_LOOP) {
			CurrentTime = 0.f;
			AnimationState = ANIMATION_STATE::FINISHED;

			return S_OK;
		}
	}
	AnimationState = ANIMATION_STATE::ONRUNNING;

	return S_OK;
}
HRESULT Animation::Decompose_Animation(const aiAnimation* _Animation, shared_ptr<MeshLoader>& _Loader) {

	AnimationName = _Animation->mName.C_Str();

	Duration = static_cast<_float>(_Animation->mDuration);
	TicksPerSecond = static_cast<_float>(_Animation->mTicksPerSecond);
	CurrentTime = 0.f;
	PlaySpeed = 1.f;

	ChannelCount = _Animation->mNumChannels;

	for (uint32_t IDX = 0; IDX < ChannelCount; ++IDX) {
		auto Channel = Channel::Create();
		Channel->Decompose_Channel(_Animation->mChannels[IDX], _Loader);

		ChannelList.push_back(Channel);
	}

	Generate_BoneMappingTable(_Loader->Get_ModelBoneList());
	return S_OK;
}

HRESULT Animation::Decompose_Animation(const FB_Animation::Animation* _Animation, shared_ptr<MeshLoader>& _Loader) {

	auto FB_AnimationData	= _Animation->AnimationData();
	auto FB_ChannelList		= _Animation->ChannelList();

	Duration		= FB_AnimationData->Duration();
	TicksPerSecond	= FB_AnimationData->TicksPerSecond();
	AnimationName	= _Animation->AnimationName()->c_str();

	ChannelCount = FB_ChannelList->size();

	for (uint32_t ChannelIndex = 0; ChannelIndex < ChannelCount; ++ChannelIndex) {
		auto Channel = Channel::Create();
		Channel->Decompose_Channel(FB_ChannelList->Get(ChannelIndex));

		ChannelList.push_back(Channel);
	}

	Generate_BoneMappingTable(_Loader->Get_ModelBoneList());
	return S_OK;
}

VOID Animation::Generate_BoneMappingTable(vector<shared_ptr<Bone>>& _BoneList) {
	BoneMappingTable.resize(_BoneList.size(), -1);

	for (uint32_t IDX = 0; IDX < ChannelList.size(); ++IDX) {
		int32_t BoneIndex = ChannelList[IDX]->Get_ChannelBoneIndex();
		if(BoneIndex != -1)
			BoneMappingTable[BoneIndex] = IDX;
	}
}

BoneTransform Animation::Get_BoneTransform(int32_t _BoneIndex, _float _CurrentTime, vector<shared_ptr<Bone>>& _BoneList) {
	int ChannelIndex = BoneMappingTable[_BoneIndex];
	if (_CurrentTime > Duration) { _CurrentTime = round(_CurrentTime); }
	return ChannelIndex != -1 ? ChannelList[ChannelIndex]->Evaluate(_CurrentTime) : _BoneList[_BoneIndex]->Get_BindPoseTRS();
}

flatbuffers::Offset<FB_Animation::Animation>  Animation::Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder) {

	vector<flatbuffers::Offset<FB_Channel::Channel>> ChannelVector;

	FB_Animation::AnimationInfo AnimInfo(Duration, TicksPerSecond);
	for (auto& CHN : ChannelList) {
		ChannelVector.push_back(CHN->Compose_BinaryData(_Builder));
	}
	auto ChannelList = _Builder.CreateVector(ChannelVector.data(), ChannelVector.size());

	return FB_Animation::CreateAnimation(_Builder, &AnimInfo, _Builder.CreateString(AnimationName), ChannelList);
}

shared_ptr<Animation> Animation::Create(){
	auto Instance = shared_ptr<Animation>(new Animation());
	if (FAILED(Instance->Initialize())) {
		MSG_BOX("Cannot Create Bone");
		return nullptr;
	}
	return Instance;
}
