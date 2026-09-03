#include "Channel.h"
Channel::Channel()	{}
Channel::~Channel()  {}

// aiNodeAnim : FBX를 불러올때, Assimp는 각 Bone마다 독립적인 aiNodeAnim을 생성한다.
// 내부에는 mPositionKeys, mRotationKeys, mScaleKeys가 있는데, 애니메이션이 흘러가면서 
// 어느 시간(키프레임)에 Bone이 어느 위치/회전/크기를 가지는지 배열로 갖게 된다.

HRESULT Channel::Initialize() {
	
	return S_OK;
}

HRESULT Channel::Set_AnimationKeyFrame(const aiNodeAnim* _Channel) {
	PositionKeyFrameCount = _Channel->mNumPositionKeys;
	for (size_t PosIDX = 0; PosIDX < PositionKeyFrameCount; ++PosIDX) {
		aiVector3D aiPosition = _Channel->mPositionKeys[PosIDX].mValue;	// aiPosition	: 각 키프레임(PosIDX)마다 본의 위치

		_float TimeStamp = static_cast<_float>(_Channel->mPositionKeys[PosIDX].mTime);		// TimeStamp	: 각 키프레임이 시작되는 시간
		KeyPosition Data = { { aiPosition.x, aiPosition.y, aiPosition.z },  TimeStamp };
		PositionKeyFrameList.push_back(Data);												// 이것을 구조체로 만들어 하나의 키프레임에 대한 정보를 저장한다.
	}
	RotationKeyFrameCount = _Channel->mNumRotationKeys;
	for (size_t RotIDX = 0; RotIDX < RotationKeyFrameCount; ++RotIDX) {
		aiQuaternion aiRotation = _Channel->mRotationKeys[RotIDX].mValue;
		_float TimeStamp = static_cast<_float>(_Channel->mRotationKeys[RotIDX].mTime);
		KeyRotation Data = { { aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w}, TimeStamp };
		RotationKeyFrameList.push_back(Data);
	}
	ScaleKeyFrameCount = _Channel->mNumScalingKeys;
	for (size_t ScaIDX = 0; ScaIDX < ScaleKeyFrameCount; ++ScaIDX) {
		aiVector3D aiScale = _Channel->mScalingKeys[ScaIDX].mValue;
		_float TimeStamp = static_cast<_float>(_Channel->mScalingKeys[ScaIDX].mTime);
		KeyScale Data = { { aiScale.x, aiScale.y, aiScale.z },  TimeStamp };
		ScaleKeyFrameList.push_back(Data);
	}

	return S_OK;
}

uint32_t Channel::Get_PositionIndex(const _float& _AnimTime) {
	for (uint32_t IDX = 0; IDX < PositionKeyFrameCount - 1; ++IDX) {
		if (_AnimTime < PositionKeyFrameList[IDX + 1].TimeStamp)	return IDX;
	}
	assert(0);
	return NULL;
}
uint32_t Channel::Get_RotationIndex(const _float& _AnimTime) {
	for (uint32_t IDX = 0; IDX < RotationKeyFrameCount - 1; ++IDX) {
		if (_AnimTime < RotationKeyFrameList[IDX + 1].TimeStamp)	return IDX;
	}
	assert(0);
	return NULL;
}
uint32_t Channel::Get_ScaleIndex(const _float& _AnimTime) {
	for (uint32_t IDX = 0; IDX < ScaleKeyFrameCount - 1; ++IDX) {
		if (_AnimTime < ScaleKeyFrameList[IDX + 1].TimeStamp)	return IDX;
	}
	assert(0);
	return NULL;
}

XMVECTOR Channel::InterpolatePosition(const _float& _AnimTime) {
	if (PositionKeyFrameCount == 1)	return XMVectorSet(PositionKeyFrameList[0].Position.x, PositionKeyFrameList[0].Position.y, PositionKeyFrameList[0].Position.z, 1.f);

	int32_t	Index00 = Get_PositionIndex(_AnimTime);
	int32_t	Index01 = Index00 + 1;

	_float	ScaleFactor = Get_ScaleFactor(PositionKeyFrameList[Index00].TimeStamp, PositionKeyFrameList[Index01].TimeStamp, _AnimTime);
	return XMVectorLerp(XMLoadFloat3(&PositionKeyFrameList[Index00].Position), XMLoadFloat3(&PositionKeyFrameList[Index01].Position), ScaleFactor);
}		 
XMVECTOR Channel::InterpolateRotation(const _float& _AnimTime) {
	if (RotationKeyFrameCount == 1) {
		return XMQuaternionNormalize(RotationKeyFrameList[0].Rotation);
	}
	int32_t	Index00 = Get_RotationIndex(_AnimTime);
	int32_t	Index01 = Index00 + 1;

	_float	 ScaleFactor = Get_ScaleFactor(RotationKeyFrameList[Index00].TimeStamp, RotationKeyFrameList[Index01].TimeStamp, _AnimTime);
	return XMQuaternionSlerp(XMLoadFloat4(&RotationKeyFrameList[Index00].Rotation), XMLoadFloat4(&RotationKeyFrameList[Index01].Rotation), ScaleFactor);

}		 
XMVECTOR Channel::InterpolateScale(const _float& _AnimTime) {
	if (ScaleKeyFrameCount == 1)	return XMVectorSet(ScaleKeyFrameList[0].Scale.x, ScaleKeyFrameList[0].Scale.y, ScaleKeyFrameList[0].Scale.z, 0.f);

	int32_t	Index00 = Get_ScaleIndex(_AnimTime);
	int32_t	Index01 = Index00 + 1;

	_float	 ScaleFactor = Get_ScaleFactor(ScaleKeyFrameList[Index00].TimeStamp, ScaleKeyFrameList[Index01].TimeStamp, _AnimTime);
	return XMVectorLerp(XMLoadFloat3(&ScaleKeyFrameList[Index00].Scale), XMLoadFloat3(&ScaleKeyFrameList[Index01].Scale), ScaleFactor);
}

_float	 Channel::Get_ScaleFactor(_float _LastTimeStamp, _float _NextTimeStamp, _float _AnimTime) {
	_float MidWayLength = _AnimTime - _LastTimeStamp;
	// "이전 키프레임 시간"과 "다음 키프레임 시간"의 사이를 CurrentTime이라고 하면, "이전 키프레임 시간"에서 CurrentTime까지가 < MidWayLength >
	_float KeyFrameTime = _NextTimeStamp - _LastTimeStamp;
	// "이전 키프레임 시간"과 "다음 키프레임 시간" 사이의 시간을 KeyFrameTime
	_float ScaleFactor = MidWayLength / KeyFrameTime;
	// 즉, ScaleFactor는 "1 키프레임이 지나기까지 얼마나 진행됐는지에 대한 "비율"이다. 
	//  0.0(최소) ~ 1.0(최대)의 사이 값인데, 0.0에 가까울 수록, 이전 프레임이 방금 막 끝난것 이고, 1.0에 가까울 수록 이제 곧 다음 프레임이 시작 되는 것.
	return ScaleFactor;
}

uint32_t Channel::FB_Get_PositionIndex(const _float& _AnimTime) {
	for (uint32_t IDX = 0; IDX < PositionKeyFrameCount - 1; ++IDX) {
		if (_AnimTime <= FB_PositionKeyFrameList[IDX + 1].TimeStamp())	return IDX;
	}
	assert(0);
	return NULL;
}
uint32_t Channel::FB_Get_RotationIndex(const _float& _AnimTime) {
	for (uint32_t IDX = 0; IDX < RotationKeyFrameCount - 1; ++IDX) {
		if (_AnimTime <= FB_RotationKeyFrameList[IDX + 1].TimeStamp())	return IDX;
	}
	assert(0);
	return NULL;
}
uint32_t Channel::FB_Get_ScaleIndex(const _float& _AnimTime) {
	for (uint32_t IDX = 0; IDX < ScaleKeyFrameCount - 1; ++IDX) {
		if (_AnimTime <= FB_ScaleKeyFrameList[IDX + 1].TimeStamp())	return IDX;
	}
	assert(0);
	return NULL;
}

XMVECTOR Channel::FB_InterpolatePosition(const _float& _AnimTime) {
	if (PositionKeyFrameCount == 1)	return XMVectorSet(FB_PositionKeyFrameList[0].Position().x(), FB_PositionKeyFrameList[0].Position().y(), FB_PositionKeyFrameList[0].Position().z(), 1.f);

	int32_t	Index00 = FB_Get_PositionIndex(_AnimTime);
	int32_t	Index01 = Index00 + 1;

	_float	ScaleFactor = Get_ScaleFactor(FB_PositionKeyFrameList[Index00].TimeStamp(), FB_PositionKeyFrameList[Index01].TimeStamp(), _AnimTime);
	
	FB_Utility::Vector3 PrevPos = FB_PositionKeyFrameList[Index00].Position();
	FB_Utility::Vector3 CurrentPos = FB_PositionKeyFrameList[Index01].Position();
	XMVECTOR PrevFramePosition = XMVectorSet(PrevPos.x(), PrevPos.y(), PrevPos.z(), 1.f);
	XMVECTOR CurrentFramePosition = XMVectorSet(CurrentPos.x(), CurrentPos.y(), CurrentPos.z(), 1.f);
	
	return XMVectorLerp(PrevFramePosition, CurrentFramePosition, ScaleFactor);
}
XMVECTOR Channel::FB_InterpolateRotation(const _float& _AnimTime) {
	
	if (RotationKeyFrameCount == 1) {
		FB_Utility::Vector4 RotVec = FB_RotationKeyFrameList[0].Rotation();
		XMVECTOR CHNRotation = { RotVec.x(), RotVec.y(), RotVec.z(), RotVec.w() };
		return XMQuaternionNormalize(CHNRotation);
	}
	int32_t	Index00 = FB_Get_RotationIndex(_AnimTime);
	int32_t	Index01 = Index00 + 1;

	_float	 ScaleFactor = Get_ScaleFactor(FB_RotationKeyFrameList[Index00].TimeStamp(), FB_RotationKeyFrameList[Index01].TimeStamp(), _AnimTime);
	
	FB_Utility::Vector4 PrevRot = FB_RotationKeyFrameList[Index00].Rotation();
	FB_Utility::Vector4 CurrentRot = FB_RotationKeyFrameList[Index01].Rotation();
	XMVECTOR PrevFrameRotation = XMVectorSet(PrevRot.x(), PrevRot.y(), PrevRot.z(), PrevRot.w());
	XMVECTOR CurrentFrameRotation = XMVectorSet(CurrentRot.x(), CurrentRot.y(), CurrentRot.z(), CurrentRot.w());

	if (XMVectorGetX(XMVector4Dot(PrevFrameRotation, CurrentFrameRotation)) < 0.0f) {
		CurrentFrameRotation = XMVectorNegate(CurrentFrameRotation);
	}

	return XMQuaternionSlerp(PrevFrameRotation, CurrentFrameRotation, ScaleFactor);
}
XMVECTOR Channel::FB_InterpolateScale(const _float& _AnimTime) {

	if (ScaleKeyFrameCount == 1)	return XMVectorSet(FB_ScaleKeyFrameList[0].Scale().x(), FB_ScaleKeyFrameList[0].Scale().y(), FB_ScaleKeyFrameList[0].Scale().z(), 0.f);

	int32_t	Index00 = FB_Get_ScaleIndex(_AnimTime);
	int32_t	Index01 = Index00 + 1;

	_float	 ScaleFactor = Get_ScaleFactor(FB_ScaleKeyFrameList[Index00].TimeStamp(), FB_ScaleKeyFrameList[Index01].TimeStamp(), _AnimTime);

	FB_Utility::Vector3 PrevSca = FB_ScaleKeyFrameList[Index00].Scale();
	FB_Utility::Vector3 CurrentSca = FB_ScaleKeyFrameList[Index01].Scale();
	XMVECTOR PrevFrameScale = XMVectorSet(PrevSca.x(), PrevSca.y(), PrevSca.z(), 0.f);
	XMVECTOR CurrentFrameSCale = XMVectorSet(CurrentSca.x(), CurrentSca.y(), CurrentSca.z(), 0.f);

	return XMVectorLerp(PrevFrameScale, CurrentFrameSCale, ScaleFactor);
}

BoneTransform Channel::Evaluate(const _float& _AnimTime) {
	if(ReferencedByBinary)	return BoneTransform{ FB_InterpolatePosition(_AnimTime)	, FB_InterpolateRotation(_AnimTime)	, FB_InterpolateScale(_AnimTime) };
	else					return BoneTransform{ InterpolatePosition(_AnimTime)	, InterpolateRotation(_AnimTime)	, InterpolateScale(_AnimTime)	 };

}

flatbuffers::Offset<FB_Channel::Channel> Channel::Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder) {

	vector<FB_Channel::PositionKeyFrame>	FB_PositionVector;
	vector<FB_Channel::RotationKeyFrame>	FB_RotationVector;
	vector<FB_Channel::ScaleKeyFrame>		FB_ScaleVector;

	for (auto& PKF : PositionKeyFrameList) {
		FB_Channel::PositionKeyFrame PKFData(FB_Utility::Vector3(PKF.Position.x, PKF.Position.y, PKF.Position.z), PKF.TimeStamp);
		FB_PositionVector.push_back(PKFData);
	}
	for (auto& RKF : RotationKeyFrameList) {
		FB_Channel::RotationKeyFrame RKFData(FB_Utility::Vector4(RKF.Rotation.x, RKF.Rotation.y, RKF.Rotation.z, RKF.Rotation.w), RKF.TimeStamp);
		FB_RotationVector.push_back(RKFData);
	}
	for (auto& SKF : ScaleKeyFrameList) {
		FB_Channel::ScaleKeyFrame SKFData(FB_Utility::Vector3(SKF.Scale.x, SKF.Scale.y, SKF.Scale.z), SKF.TimeStamp);
		FB_ScaleVector.push_back(SKFData);
	}

	auto FB_PositionList = _Builder.CreateVectorOfStructs(FB_PositionVector);
	auto FB_RotationList = _Builder.CreateVectorOfStructs(FB_RotationVector);
	auto FB_ScaleList	 = _Builder.CreateVectorOfStructs(FB_ScaleVector);

	return FB_Channel::CreateChannel(_Builder, FB_PositionList, FB_RotationList, FB_ScaleList,
		PositionKeyFrameCount, RotationKeyFrameCount, ScaleKeyFrameCount, ChannelBoneIndex);
}

HRESULT Channel::Decompose_Channel(const aiNodeAnim* _AnimNode, shared_ptr<MeshLoader>& _Loader) {
	ChannelBoneIndex = _Loader->Get_ModelBoneIndex(_AnimNode->mNodeName.C_Str());

	Set_AnimationKeyFrame(_AnimNode);

	return S_OK;
}

HRESULT Channel::Decompose_Channel(const FB_Channel::Channel* _Channel) {

	PositionKeyFrameCount = _Channel->PositionKeyFrameCount();
	RotationKeyFrameCount = _Channel->RotationKeyFrameCount();
	ScaleKeyFrameCount = _Channel->ScaleKeyFrameCount();

	FB_PositionKeyFrameList.reserve(PositionKeyFrameCount);
	FB_RotationKeyFrameList.reserve(RotationKeyFrameCount);
	FB_ScaleKeyFrameList.reserve(ScaleKeyFrameCount);

	auto PKFList = _Channel->PositionKeyFrameList();
	auto RKFList = _Channel->RotationKeyFrameList();
	auto SKFList = _Channel->ScaleKeyFrameList();

	for (uint32_t IDX = 0; IDX < PositionKeyFrameCount; ++IDX) {
		auto PKFPOS = PKFList->Get(IDX)->Position();
		auto PKFTRS = PKFList->Get(IDX)->TimeStamp();
		FB_PositionKeyFrameList.push_back({ PKFPOS, PKFTRS });
	}
	for (uint32_t IDX = 0; IDX < RotationKeyFrameCount; ++IDX) {
		auto RKFROT = RKFList->Get(IDX)->Rotation();
		auto RKFTRS = RKFList->Get(IDX)->TimeStamp();

		FB_RotationKeyFrameList.push_back({ RKFROT, RKFTRS });
	}
	for (uint32_t IDX = 0; IDX < ScaleKeyFrameCount; ++IDX) {
		auto SKFROT = SKFList->Get(IDX)->Scale();
		auto SKFTRS = SKFList->Get(IDX)->TimeStamp();

		FB_ScaleKeyFrameList.push_back({ SKFROT, SKFTRS });
	}
	
	ChannelBoneIndex	= _Channel->ChannelBoneIndex();
	ReferencedByBinary	= true;

	return S_OK;
}

shared_ptr<Channel> Channel::Create() {
	auto Instance = shared_ptr<Channel>(new Channel());
	if (FAILED(Instance->Initialize())) {
		MSG_BOX("Cannot Create Bone");
		return nullptr;
	}
	return Instance;
}
