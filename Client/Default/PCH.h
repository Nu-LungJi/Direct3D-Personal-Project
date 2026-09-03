#ifndef PCH_H
#define PCH_H
//////////////////////////////////////////// Precompiled Header

#include "framework.h"
#include <process.h>

//////////////////////////////////////////// Engine Import

#include "Engine_Define.h"
#include "GameInstance.h"

//////////////////////////////////////////// Client Function

#include "ENUM.h"
#include "DEFINE.h"
#include "Common.h"

//////////////////////////////////////////// Object List
/////////////////////// Abstract Object
#include "GameObject.h"
#include "Component.h"
/////////////////////// Global Object
#include "UIObject.h"
#include "FontObject.h"
/////////////////////// Component
#include "Buffer.h"
#include "Terrain.h"
#include "Texture.h"
#include "RectBuffer.h"
#include "CubeBuffer.h"
#include "Shader.h"
#include "Transform.h"
#include "Collider.h"
#include "MeshLoader.h"
#include "Animator.h"
#include "NavMeshAgent.h"
/////////////////////// Object
#include "BehaviorTree_Knight.h"
#include "BehaviorTree_Void.h"
#include "BehaviorTree_FinalBoss.h"

#include "StateMachine.h"

#include "Player.h"
#include "PlayerEffect.h"
#include "Monster_Knight.h"
#include "Monster_Void.h"
#include "Monster_FinalBoss.h"
#include "BossEffect.h"
#include "BossCard.h"

#include "DynamicCamera.h"
#include "ActionCamera.h"
#include "ActionCamera_Boss.h"
#include "PlayerCamera.h"

#include "Topography.h"
#include "Ocean.h"
#include "Portal.h"
#include "RewardChest.h"
#include "Bicon.h"
#include "SkySphere.h"

#include "PlayerHitBox.h"
#include "HitBoxPool.h"

#include "HPBarUI.h"
#include "DamageFontUI.h"
#include "EventUI.h"
#include "PopUpUI.h"
#include "BossUI.h"
#include "MainUI.h"
//////////////////////////////////////////// Scene List

#include "LoadingScene.h"
#include "MainScene.h"
#include "BossScene.h"

//////////////////////////////////////////// Manager

#include "GameManager.h"

//////////////////////////////////////////// Global Variable

extern HWND			hWnd;
extern HINSTANCE	hInst;

//////////////////////////////////////////// End
#endif //PCH_H