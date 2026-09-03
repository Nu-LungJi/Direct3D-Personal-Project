#include "Channel_Ad.h"
Channel_Ad::Channel_Ad()    {   }
Channel_Ad::~Channel_Ad()   {   }

HRESULT Channel_Ad::Initialize(const aiNodeAnim* _Channel, MeshLoader_Ad* _ModelLoader) {
	BoneIndex = _ModelLoader->Get_ModelBoneIndex(_Channel->mNodeName.C_Str());

	KeyFrameCount = max(max(_Channel->mNumScalingKeys, _Channel->mNumRotationKeys), _Channel->mNumPositionKeys);

	_float3 PositionValue		= {};
	_float4 RotationValue		= {};
	_float3 ScaleValue			= {};

	for (uint32_t IDX = 0; IDX < KeyFrameCount; ++IDX) {
		KeyFrame KeyFrame = {};
		if (_Channel->mNumScalingKeys > IDX) {
			memcpy(&ScaleValue, &_Channel->mScalingKeys[IDX].mValue, sizeof(_float3));
			KeyFrame.TimeStamp = _Channel->mScalingKeys[IDX].mTime;
		}
		if (_Channel->mNumRotationKeys > IDX) {	
			//memcpy(&RotationValue, &_Channel->mRotationKeys[IDX].mValue, sizeof(_float4));
			RotationValue = _float4(_Channel->mRotationKeys[IDX].mValue.x, 
									_Channel->mRotationKeys[IDX].mValue.y, 
									_Channel->mRotationKeys[IDX].mValue.z, 
									_Channel->mRotationKeys[IDX].mValue.w);
			
			KeyFrame.TimeStamp = _Channel->mRotationKeys[IDX].mTime;
		}
		if (_Channel->mNumPositionKeys > IDX) {
			memcpy(&PositionValue, &_Channel->mPositionKeys[IDX].mValue, sizeof(_float3));
			KeyFrame.TimeStamp = _Channel->mPositionKeys[IDX].mTime;
		}

		KeyFrame.Scaling = ScaleValue;
		KeyFrame.Rotation = RotationValue;
		KeyFrame.Position = PositionValue;

		KeyFrameList.push_back(KeyFrame);
	}

	
	return S_OK;
}
HRESULT Channel_Ad::Update_TransformMatrix(const vector<shared_ptr<class Bone_Ad>>& BoneList, uint32_t* _CurrentKeyFrameIndex, _float _CurrentTimeStampPosition) {
	
	if (_CurrentTimeStampPosition == 0.f) { *_CurrentKeyFrameIndex = 0; }

	KeyFrame	LastKeyFrame = KeyFrameList.back();

	XMVECTOR	PositionVec, RotationVec, ScalingVec;		

	if (_CurrentTimeStampPosition >= LastKeyFrame.TimeStamp) {
		ScalingVec = XMLoadFloat3(&LastKeyFrame.Scaling);
		RotationVec = XMLoadFloat4(&LastKeyFrame.Rotation);
		PositionVec = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.Position), 1.f);
	}
	else {
		while (_CurrentTimeStampPosition >= KeyFrameList[(*_CurrentKeyFrameIndex) + 1].TimeStamp) { 
			*_CurrentKeyFrameIndex += 1; 
		}

		_float Ratio = (_CurrentTimeStampPosition - KeyFrameList[(*_CurrentKeyFrameIndex)].TimeStamp) /
			(KeyFrameList[(*_CurrentKeyFrameIndex) + 1].TimeStamp - KeyFrameList[(*_CurrentKeyFrameIndex)].TimeStamp);

		XMVECTOR BFScaleFrame, AFScaleFrame, BFPositionFrame, AFPositionFrame, BFRotationFrame, AFRotationFrame;

		BFScaleFrame = XMLoadFloat3(&KeyFrameList[(*_CurrentKeyFrameIndex)].Scaling);
		AFScaleFrame = XMLoadFloat3(&KeyFrameList[(*_CurrentKeyFrameIndex) + 1].Scaling);
		ScalingVec = XMVectorLerp(BFScaleFrame, AFScaleFrame, Ratio);

		BFRotationFrame = XMLoadFloat4(&KeyFrameList[(*_CurrentKeyFrameIndex)].Rotation);
		AFRotationFrame = XMLoadFloat4(&KeyFrameList[(*_CurrentKeyFrameIndex) + 1].Rotation);
		RotationVec = XMQuaternionSlerp(BFRotationFrame, AFRotationFrame, Ratio);

		BFPositionFrame = XMVectorSetW(XMLoadFloat3(&KeyFrameList[(*_CurrentKeyFrameIndex)].Position), 1.f);
		AFPositionFrame = XMVectorSetW(XMLoadFloat3(&KeyFrameList[(*_CurrentKeyFrameIndex) + 1].Position), 1.f);
		PositionVec = XMVectorLerp(BFPositionFrame, AFPositionFrame, Ratio);
	}

	XMMATRIX BoneTransformMatrix = XMMatrixAffineTransformation(ScalingVec, XMVectorSet(0.f, 0.f, 0.f, 1.f), RotationVec, PositionVec);
	BoneList[BoneIndex]->Update_BoneMatrix(BoneTransformMatrix);

	return S_OK;
}
shared_ptr<Channel_Ad> Channel_Ad::Create(const aiNodeAnim* _Channel, MeshLoader_Ad* _ModelLoader) {
	auto Instance = shared_ptr<Channel_Ad>(new Channel_Ad());
	if (FAILED(Instance->Initialize(_Channel, _ModelLoader))) {
		MSG_BOX("Cannot Create Channel");
		return nullptr;
	}
	return Instance;
}
