#pragma once
#include "Engine_Define.h"
#include "MeshLoader_Ad.h"

BEGIN(Engine)
struct KeyFrame {
	XMFLOAT3	Position;
	XMFLOAT4	Rotation;
	XMFLOAT3	Scaling;

	_float		TimeStamp;
};
class ENGINE_DLL Channel_Ad {
private:
	Channel_Ad();
public:
	~Channel_Ad();

public:
	HRESULT		Initialize(const aiNodeAnim* _Channel, class MeshLoader_Ad* _ModelLoader);
	HRESULT		Update_TransformMatrix(const vector<shared_ptr<class Bone_Ad>>& BoneList, uint32_t* _CurrentKeyFrameIndex, _float _CurrentTimeStampPosition);


	static		shared_ptr<Channel_Ad> Create(const aiNodeAnim* _Channel, class MeshLoader_Ad* _ModelLoader);

private:

	vector<KeyFrame>		KeyFrameList;
	uint32_t				KeyFrameCount	= {  0 };
	int32_t					BoneIndex		= { -1 };
};
END