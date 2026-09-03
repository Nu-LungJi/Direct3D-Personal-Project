#pragma once
#include "Engine_Define.h"
#include "FB_Channel_generated.h"
#include "MeshLoader.h"

BEGIN(Engine)
class ENGINE_DLL Channel {
private:
	Channel();
public:
	~Channel();

public:
	HRESULT		Initialize();
	
	flatbuffers::Offset<FB_Channel::Channel> Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder);
	HRESULT		Decompose_Channel(const aiNodeAnim* _AnimNode, shared_ptr<MeshLoader>& _Loader);
	HRESULT		Decompose_Channel(const FB_Channel::Channel* _Channel);
	
	BoneTransform	Evaluate(const _float& _AnimTime);

	uint32_t		Get_ChannelBoneIndex() { return ChannelBoneIndex; }

	static shared_ptr<Channel> Create();

private:
	HRESULT		Set_AnimationKeyFrame(const aiNodeAnim* _Channel);

	uint32_t	Get_PositionIndex(const _float& _AnimTime);
	uint32_t	Get_RotationIndex(const _float& _AnimTime);
	uint32_t	Get_ScaleIndex(const _float& _AnimTime);

	XMVECTOR	InterpolatePosition(const _float& _AnimTime);
	XMVECTOR	InterpolateRotation(const _float& _AnimTime);
	XMVECTOR	InterpolateScale(const _float& _AnimTime);

	_float		Get_ScaleFactor(_float _LastTimeStamp, _float _NextTimeStamp, _float _AnimTime);

	uint32_t	FB_Get_PositionIndex(const _float& _AnimTime);
	uint32_t	FB_Get_RotationIndex(const _float& _AnimTime);
	uint32_t	FB_Get_ScaleIndex(const _float& _AnimTime);

	XMVECTOR	FB_InterpolatePosition(const _float& _AnimTime);
	XMVECTOR	FB_InterpolateRotation(const _float& _AnimTime);
	XMVECTOR	FB_InterpolateScale(const _float& _AnimTime);

private:
	vector<KeyPosition>			PositionKeyFrameList;
	vector<KeyRotation>			RotationKeyFrameList;
	vector<KeyScale>			ScaleKeyFrameList;

	uint32_t					PositionKeyFrameCount;
	uint32_t					RotationKeyFrameCount;
	uint32_t					ScaleKeyFrameCount;

	uint32_t					ChannelBoneIndex;

	////////// FlatBuffer //////////
	vector<FB_Channel::PositionKeyFrame>	FB_PositionKeyFrameList;
	vector<FB_Channel::RotationKeyFrame>	FB_RotationKeyFrameList;
	vector<FB_Channel::ScaleKeyFrame>		FB_ScaleKeyFrameList;

	 _bool	ReferencedByBinary = false;
};
END