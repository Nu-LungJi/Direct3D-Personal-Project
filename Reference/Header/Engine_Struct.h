#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"
#define	 MAX_BONE_COUNT		512
#define	 MAX_BONE_INFLUENCE	4

using namespace std;
using namespace DirectX;
using namespace SimpleMath;
using namespace Microsoft::WRL;

namespace Engine
{
	
	namespace Description {
		struct ENGINE_DESC {
			HWND		hWnd;
			HINSTANCE	hInst;
			WINMODE		WindowMode;
			_float		WindowResolutionX, WindowResolutionY;
			int32_t		MaxSceneCount;
		};
		struct SCENE_DESC {
			uint32_t	MaxSceneCount;
			uint32_t	MaxLayerCount;

			uint32_t	CurrentSceneIndex;

			uint32_t	CurrentCameraCount;
		};
		struct KeyINFO {
			KEYBOARDKEYSTATE	Current_KeyState;
			BOOL		Prev_KeyState;
		};
		struct ColliderINFO {
			COLLIDER_TYPE	ColliderType;
			_float3		Center;
			_float3		Size;
		};
	}
	
	namespace ANIMATOR_COMPONENT {

		struct BoneTransform {
			XMVECTOR	Position = { 0.f, 0.f, 0.f, 1.f };
			XMVECTOR	Rotation = { 0.f, 0.f, 0.f, 1.f };
			XMVECTOR	Scale	 = { 1.f, 1.f, 1.f, 0.f };
		};

		struct KeyPosition {
			_float3		Position;
			_float		TimeStamp;
		};
		struct KeyRotation {
			Quaternion	Rotation;
			_float		TimeStamp;
		};
		struct KeyScale {
			_float3		Scale;
			_float		TimeStamp;
		};

		struct AssimpNodeData {
			_matrix		Transformation;
			string		Name;
			int32_t		ChildCount;
			vector<AssimpNodeData>	Children;
		};
	}

	namespace SHADER {
		struct VTXPOS {
			XMFLOAT3	Position;

			static constexpr uint32_t						ElementsCount = { 1 };
			static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[ElementsCount] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
		};
		struct VTXTEX {
			XMFLOAT3	Position;
			XMFLOAT2	TexCoord;

			static constexpr uint32_t						ElementsCount = { 2 };
			static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[ElementsCount] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
		};
		struct VTXCUBE {
			XMFLOAT3	Position;
			XMFLOAT3	TexCoord;

			static constexpr uint32_t						ElementsCount = { 2 };
			static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[ElementsCount] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
		};
		struct VTXNORTEX {
			VTXNORTEX() :Position{ 0.f, 0.f,0.f }, TexCoord{ 0.f, 0.f } {};
			VTXNORTEX(_float _X, _float _Y, _float _Z) : Position{ _X, _Y,_Z }, TexCoord{ 0.f, 0.f } {};
			VTXNORTEX(_float _UVX, _float _UVY) : TexCoord(_UVX, _UVY) {};

			XMFLOAT3	Position = {};
			XMFLOAT3	Normal = {};
			XMFLOAT2	TexCoord = {};

			uint32_t	BoneID[MAX_BONE_INFLUENCE] = {};			// 해당 버텍스에 영향을 주는 BoneID, 최대 4개의 본에 대해서만 영향을 받을 수 있음.
			_float		Weight[MAX_BONE_INFLUENCE] = {};			// 위 4개의 본에 의해 받는 가중치를 저장.

			static constexpr uint32_t						ElementsCount = { 3 };
			static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[ElementsCount] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
		};
		struct VTXMESH {
			VTXMESH() :Position{ 0.f, 0.f,0.f }, TexCoord{ 0.f, 0.f } {};
			VTXMESH(_float _X, _float _Y, _float _Z) : Position{ _X, _Y,_Z }, TexCoord{ 0.f, 0.f } {};
			VTXMESH(_float _UVX, _float _UVY) : TexCoord(_UVX, _UVY) {};

			XMFLOAT3	Position;
			XMFLOAT2	TexCoord;
			XMFLOAT3	Normal;
			XMFLOAT3	Tangent;
			XMFLOAT3	BiNormal;

			static constexpr uint32_t						ElementsCount = { 5 };
			static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[ElementsCount] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
		};
		struct VTXANIMMESH {
			VTXANIMMESH() :Position{ 0.f, 0.f,0.f }, TexCoord{ 0.f, 0.f } {};
			VTXANIMMESH(_float _X, _float _Y, _float _Z) : Position{ _X, _Y,_Z }, TexCoord{ 0.f, 0.f } {};
			VTXANIMMESH(_float _UVX, _float _UVY) : TexCoord(_UVX, _UVY) {};

			XMFLOAT3	Position;
			XMFLOAT3	Normal;
			XMFLOAT3	Tangent;
			XMFLOAT3	BiNormal;
			XMFLOAT2	TexCoord;

			uint32_t	BlendIndex[MAX_BONE_INFLUENCE] = { 0 };
			_float		BlendWeight[MAX_BONE_INFLUENCE] = { 0 };

			static constexpr uint32_t						ElementsCount = { 7 };
			static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[ElementsCount] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
		};

		struct VTXINS_PARTICLE {
			XMFLOAT4	RightVec, UpVec, LookVec, PosVec;
			XMFLOAT2	LifeTime;
		};
		struct VTXINS_PARTICLE_RECT {
			static constexpr uint32_t						ElementsCount = { 7 };
			static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[ElementsCount] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},

				{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0 ,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },

				{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT	   , 1, 64,	D3D11_INPUT_PER_INSTANCE_DATA, 1 }
			};
		};
		struct VTXINS_PARTICLE_POINT {
			static constexpr uint32_t						ElementsCount = { 6 };
			static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[ElementsCount] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0},

				{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0 ,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "WORLD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },

				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	, 1, 64,	D3D11_INPUT_PER_INSTANCE_DATA, 1 }
			};
		};

		struct VTXINS {
			VTXINS() {};
			VTXINS(_float _X, _float _Y, _float _Z) : Position{ _X, _Y,_Z }, TexCoord{ 0.f, 0.f } {};

			XMFLOAT3	Position;
			XMFLOAT2	TexCoord;
			XMFLOAT3	Normal;
			XMFLOAT3	Tangent;
			XMFLOAT3	BiNormal;

			XMFLOAT4	RightVec, UpVec, LookVec, PosVec;

			static constexpr uint32_t						ElementsCount = { 9 };
			static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[ElementsCount] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
				
				{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0 ,	D3D11_INPUT_PER_INSTANCE_DATA, 1 }, 
				{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32,	D3D11_INPUT_PER_INSTANCE_DATA, 1 },
				{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48,	D3D11_INPUT_PER_INSTANCE_DATA, 1 }
			};
		};
		struct Texture {
			Texture(aiTextureType _TexType, string _TexPath, string _TexName)
				: TextureType(_TexType), TextureFilePath(_TexPath), TextureFileName(_TexName), TextureResource(nullptr) {
			}
			aiTextureType						TextureType;
			string								TextureFilePath;
			string								TextureFileName;
			ComPtr<ID3D11ShaderResourceView>	TextureResource;
		};
	}

	struct NavigationResult {
		_float		Distance = FLT_MAX;
		int32_t		SubMeshIndex = -1;
		uint32_t	NavMeshFaceIndex = 0;
		XMFLOAT3	Vertex00, Vertex01, Vertex02;
	};
}

#endif // Engine_Struct_h__
