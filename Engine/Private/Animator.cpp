#include "Animator.h"
#include "MeshLoader.h"
#include "Animation.h"
#include "Bone.h"
#include "Channel.h"
#include "GameObject.h"

Animator::Animator(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Component(_GRPDEV, _DEVCTX){}
Animator::Animator(CONST Animator& _PRTOBJ) : Component(_PRTOBJ), AnimationList(_PRTOBJ.AnimationList), ModelBoneList(_PRTOBJ.ModelBoneList),
CurrentAnimation(_PRTOBJ.CurrentAnimation), Animation_Loop(_PRTOBJ.Animation_Loop), Animation_Blending(_PRTOBJ.Animation_Blending),
ModelRootBone(_PRTOBJ.ModelRootBone), FirstFrameCheck(_PRTOBJ.FirstFrameCheck), MovementAmount(_PRTOBJ.MovementAmount){}

HRESULT	Animator::Initialize_ProtoType() {
    MovementAmount = { 1.f, 1.f, 1.f };
    return S_OK;
}
HRESULT	Animator::Initialize(VOID* _ARG) {
    Animation_BlendTime = 0.25f;
    return S_OK;
}

VOID Animator::Update_Animation(const _float& _DT) {
    if (Animation_Blending && nullptr != PreviousAnimation)
        Update_BlendAnimation(_DT);
    else if (CurrentAnimation)
        Update_SingleAnimation(_DT);
    Update_RootMotion(_DT);
}

VOID Animator::Update_SingleAnimation(const _float& _DT) {
    _float CurrentAnimTime = CurrentAnimation->Get_CurrentTime();
    if (Animation_Loop != ANIMATION_LOOP::PAUSED) {
        CurrentAnimation->Update_Animation(_DT, Animation_Loop, this);
    }

    for (uint32_t IDX = 0; IDX < ModelBoneList.size(); ++IDX) {
        BoneTransform BonePose = CurrentAnimation->Get_BoneTransform(IDX, CurrentAnimTime, ModelBoneList);

        XMMATRIX FinalBoneMatrix = XMMatrixScalingFromVector(BonePose.Scale) * XMMatrixRotationQuaternion(BonePose.Rotation) * XMMatrixTranslationFromVector(BonePose.Position);
    
        ModelBoneList[IDX]->Set_LocalTransform(FinalBoneMatrix);
    }
}

VOID Animator::Update_BlendAnimation(const _float& _DT) {
    Animation_BlendWeight += _DT;
    _float CurrentAnimationBlendWeight = Animation_BlendWeight / Animation_BlendTime;
    if (CurrentAnimationBlendWeight > 1.f) { CurrentAnimationBlendWeight = 1.f; }
    
    _float PreviousAnim_AnimTime = PreviousAnimation->Get_CurrentTime();
    _float CurrentAnim_AnimTime  = CurrentAnimation ->Get_CurrentTime();
    BoneTransform PrevBonePose, CurrBonePose;
    //PreviousAnimation->Update_Animation(_DT, Animation_Loop, this);
    CurrentAnimation ->Update_Animation(_DT, Animation_Loop, this);

    for (uint32_t IDX = 0; IDX < ModelBoneList.size(); ++IDX) {
        PrevBonePose = PreviousAnimation->Get_BoneTransform(IDX, PreviousAnim_AnimTime, ModelBoneList);
        CurrBonePose = CurrentAnimation ->Get_BoneTransform(IDX, CurrentAnim_AnimTime, ModelBoneList);

        if (XMVectorGetX(XMVector4Dot(PrevBonePose.Rotation, CurrBonePose.Rotation)) < 0.0f) {
            CurrBonePose.Rotation = XMVectorNegate(CurrBonePose.Rotation);
        }
        XMVECTOR    FinalBonePosition, FinalBoneRotation, FinalBoneScale;
        if   (ModelBoneList[IDX] == ModelRootBone) { FinalBonePosition = CurrBonePose.Position; }
        else { FinalBonePosition = XMVectorLerp(PrevBonePose.Position, CurrBonePose.Position, CurrentAnimationBlendWeight); }
       
        FinalBoneRotation   = XMQuaternionSlerp(PrevBonePose.Rotation, CurrBonePose.Rotation, CurrentAnimationBlendWeight);
        FinalBoneScale      = XMVectorLerp(PrevBonePose.Scale, CurrBonePose.Scale, CurrentAnimationBlendWeight);

        XMMATRIX    FinalBoneMatrix = XMMatrixScalingFromVector(FinalBoneScale) * XMMatrixRotationQuaternion(FinalBoneRotation) * XMMatrixTranslationFromVector(FinalBonePosition);

        ModelBoneList[IDX]->Set_LocalTransform(FinalBoneMatrix);
    }

    if (Animation_BlendWeight >= Animation_BlendTime) {
        Animation_BlendWeight   = 0.f;
        Animation_Blending      = false;
        PreviousAnimation->Set_AnimationState(ANIMATION_STATE::ONREADY);
        PreviousAnimation->Set_CurrentTime(0.f);
    }
}

VOID Animator::Update_RootMotion(const _float& _DT) {
    for (auto& Bone : ModelBoneList)
        Bone->Update_Bone(ModelBoneList);

    XMMATRIX CurrComBinedTransform = ModelRootBone->Get_ComBinedTransform();

    if (FirstFrameCheck) {
        ModelRootBone->Set_PrevComBinedTransform(CurrComBinedTransform);
        FirstFrameCheck = false;
    }
    XMMATRIX PrevComBinedTransform = ModelRootBone->Get_PrevComBinedTransform();

    XMVECTOR DeltaPos = CurrComBinedTransform.r[3] - PrevComBinedTransform.r[3];
    DeltaPos = XMVector3Rotate(DeltaPos, Component_Transform->Get_WorldRotationQuat());

    XMVECTOR NextPosition = Component_Transform->Get_WorldPosition() + DeltaPos * XMLoadFloat3(&MovementAmount);
    _float   NextHeight = 0.f, CellHeight = 0.f;

    if (Component_NavMeshAgent->Evaluate_NextPositionCell(NextPosition, &NextHeight, &CellHeight, _DT)) {
        uint32_t OnAirState = *Component_Owner->Get_ObjectOnAir();
        if      (OnAirState == static_cast<uint32_t>(JUMPSTATE::JUMPING)) {
            Component_Transform->Set_WorldPosition(NextPosition);
        }
        else if (OnAirState == static_cast<uint32_t>(JUMPSTATE::FALLING)) {
            Component_Transform->Set_WorldPosition(NextPosition);
            if (CellHeight + 0.12f > XMVectorGetY(NextPosition))
                Component_Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::LAND));
        }
        else {
            XMVECTOR WorldPosition = Component_Transform->Get_WorldPosition();
            Component_Transform->Set_WorldPosition(XMVectorSetY(NextPosition, NextHeight));
        }
    }
    else {
        XMVECTOR WorldPosition = Component_Transform->Get_WorldPosition();
        Component_Transform->Set_WorldPosition(XMVectorSetY(WorldPosition, XMVectorGetY(NextPosition)));
    }
    ModelRootBone->Set_PrevComBinedTransform(CurrComBinedTransform);
    ModelRootBone->Set_ComBinedTransformTranslation({ 0.f, 0.f, 0.f });
    ModelRootBone->Set_LocalTransformTranslation({ 0.f, 0.f, 0.f });

    for (auto& Bone : ModelBoneList)
        Bone->Update_Bone(ModelBoneList);
}

VOID Animator::Play_Animation(uint32_t _Index, _bool _AnimationLoop, _float _AnimSpeed) {
    Animation_Loop = static_cast<ANIMATION_LOOP>(_AnimationLoop + 1);

    CurrentAnimation->Set_AnimationState(ANIMATION_STATE::ONREADY);

    PreviousAnimation       = CurrentAnimation;
    CurrentAnimation        = AnimationList[_Index];
    CurrentAnimationIndex   = _Index;

    AnimationName = CurrentAnimation->Get_AnimationName();

    CurrentAnimation->Set_AnimationState(ANIMATION_STATE::ONRUNNING);
    CurrentAnimation->Set_CurrentTime(0.f);
    CurrentAnimation->Set_AnimationSpeed(_AnimSpeed);
    
    Animation_BlendWeight = 0.f;
    FirstFrameCheck = true;
}

VOID Animator::Play_Animation(shared_ptr<Animation> _Animation, _bool _AnimationLoop, _float _AnimSpeed) {
    Animation_Loop = static_cast<ANIMATION_LOOP>(_AnimationLoop + 1);

    CurrentAnimation->Set_AnimationState(ANIMATION_STATE::ONREADY);

    PreviousAnimation = CurrentAnimation;
    CurrentAnimation = _Animation;
    CurrentAnimationIndex = CurrentAnimation->Get_AnimationIndex();

    AnimationName = CurrentAnimation->Get_AnimationName();

    CurrentAnimation->Set_AnimationState(ANIMATION_STATE::ONRUNNING);
    CurrentAnimation->Set_CurrentTime(0.f);
    CurrentAnimation->Set_AnimationSpeed(_AnimSpeed);

    Animation_BlendWeight = 0.f;
    FirstFrameCheck = true;
}


HRESULT	Animator::Decompose_Animator(shared_ptr<MeshLoader> _Loader) {
    auto ModelAnimationList = _Loader->Get_AnimationList();
    uint32_t ModelAnimationCount = static_cast<uint32_t>(ModelAnimationList.size());

    for (uint32_t IDX = 0; IDX < ModelAnimationCount; IDX++) {
        shared_ptr<Animation> Animation = Animation::Create();
        Animation->Decompose_Animation(ModelAnimationList[IDX], _Loader);
        Animation->Set_AnimationIndex(IDX);
        AnimationList.push_back(Animation);
    }
    if (AnimationList.size() == 0) {
        MSG_BOX("Cannot Register Animation Model : No Animation in Model.");
        return E_FAIL;
    }
    CurrentAnimation = AnimationList[0];
    CurrentAnimationIndex = 0;
    AnimationName = CurrentAnimation->Get_AnimationName();

    ModelBoneList   = _Loader->Get_ModelBoneList();
    for (auto& Bone : ModelBoneList) {
        if (Bone->Get_BoneName() == "Root") {
            ModelRootBone = Bone;
            break;
        }
    }
    return S_OK;
}

flatbuffers::Offset<FB_Animator::Animator> Animator::Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder) {
    vector<flatbuffers::Offset<FB_Animation::Animation>>    FB_AnimationVector;

    for (auto& Anim : AnimationList) {
        FB_AnimationVector.push_back(Anim->Compose_BinaryData(_Builder));
    }
    auto FB_AnimationList = _Builder.CreateVector(FB_AnimationVector);

    return FB_Animator::CreateAnimator(_Builder, FB_AnimationList);
}

HRESULT Animator::Decompose_BinaryData(const flatbuffers::Vector<flatbuffers::Offset<FB_Animation::Animation>>* _AnimationList, shared_ptr<MeshLoader> _Loader) {
    
    for (uint32_t IDX = 0; IDX < _AnimationList->size(); ++IDX) {

        shared_ptr<Animation> Anim = Animation::Create();
        Anim->Decompose_Animation(_AnimationList->Get(IDX), _Loader);
        AnimationList.push_back(Anim);
    }

    CurrentAnimation        = AnimationList[0];
    CurrentAnimationIndex   = 0;
    AnimationName          = CurrentAnimation->Get_AnimationName();
    ModelBoneList           = _Loader->Get_ModelBoneList();

    for (auto& Bone : ModelBoneList) {
        if (Bone->Get_BoneName() == "Root") {
            ModelRootBone = Bone;
            break;
        }
    }
    return S_OK;
}

unique_ptr<Animator >	Animator::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto Instance = unique_ptr<Animator>(new Animator(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize_ProtoType())) {
        MSG_BOX("Cannot Create Animator");
        return nullptr;
    }
    return Instance;
}
shared_ptr<Component>	Animator::Clone(VOID* _ARG) {
    auto Instance = shared_ptr<Animator>(new Animator(*this));
    if (FAILED(Instance->Initialize(_ARG))) {
        MSG_BOX("Cannot Clone Animator");
        return nullptr;
    }
    return Instance;
}