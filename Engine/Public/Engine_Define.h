#ifndef Engine_Define_h__
#define Engine_Define_h__

#pragma warning(disable : 4251)
#pragma warning(disable : 4005)
#pragma warning(disable : 4251)

////////////////////////////////// STL Container

#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <filesystem>

////////////////////////////////// 
////////////////////////////////// Modern C++

#include <memory>
#include <functional>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <any>

////////////////////////////////// 
////////////////////////////////// DirectX

#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <wrl.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <dxgidebug.h>

////////////////////////////////// 
////////////////////////////////// DirectXTK

#include <DirectXTK/GamePad.h>
#include <DirectXTK/Keyboard.h>
#include <DirectXTK/Mouse.h>
#include <DirectXTK/CommonStates.h>
#include <DirectXTK/SimpleMath.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/DirectXHelpers.h>
#include <DirectXTK/GeometricPrimitive.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/SpriteBatch.h> 
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/Effects.h>
#include <DirectXTex.h>
#include <DirectXTK/Model.h>

////////////////////////////////// 
////////////////////////////////// Assimp

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

////////////////////////////////// 
////////////////////////////////// FMOD

#include <FMOD/fmod.hpp>
#include <FMOD/fmod.h>
#include <FMOD/fmod_errors.h>

#include "../../Extension/XML/XML/tinyxml2.h"
#include "../Engine/Public/magic_enum.hpp"
#include <FlatBuffer/flatbuffers.h>

////////////////////////////////// 
////////////////////////////////// IMGUI

#define IMGUI_DEFINE_MATH_OPERATORS
#include <IMGUI/imgui.h>
#include <IMGUI/imgui_internal.h>
#define IM_VEC2_CLASS_EXTRA
#include <IMGUI/imconfig.h>
#include <IMGUI/imgui_impl_dx11.h>
#include <IMGUI/imgui_impl_win32.h>
#include <IMGUI/ImGuizmo.h>

#include <NodeEditor/imgui_node_editor.h>
#include <NodeEditor/imgui_node_editor_internal.h>

////////////////////////////////// 
////////////////////////////////// 

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

////////////////////////////////// using namespace 

using namespace std;
using namespace Engine;
using namespace chrono;
using namespace DirectX;
using namespace Microsoft::WRL;

using namespace ANIMATOR_COMPONENT;
using namespace Description;
using namespace SHADER;
////////////////////////////////// 

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif
#endif // Engine_Define_h__
