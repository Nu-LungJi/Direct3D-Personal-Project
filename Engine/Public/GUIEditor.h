#pragma once
#include "Engine_Define.h"
#include "Animator.h"
#include "Animation.h"
#include "../../Client/Public/PlayerEffect.h"

BEGIN(Engine)
class ENGINE_DLL GUIEditor {
private:
	GUIEditor(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~GUIEditor();

public:
	HRESULT	Ready_GUIEditor();
	VOID	Render_Begin_GUIEditor();
	VOID	Render_GUIEditor();
	VOID	Render_End_GUIEditor();

	VOID	Register_AnimationComponent(shared_ptr<Animator> _Animator) { AnimatorList.push_back(_Animator); }
	VOID	Register_UIObject(shared_ptr<UIObject> _UIO) { UIList.push_back(_UIO); }

	VOID    Setting_DarkTheme();
	_bool   PreOccupy_IMGUI(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	VOID	Release_GUIEditor();

public:
	VOID	IMGUIZMO_ObjectTransform();
	VOID	IMGUI_EffectEditor();
	VOID	IMGUI_Navigator();
	VOID	IMGUI_UIEditor();
	VOID	IMGUI_ShaderController();
	VOID	IMGUI_EffectController();

	XMFLOAT3 QuaternionToDegree(XMVECTOR _Quat);
	XMVECTOR DegreeToQuaternion(XMFLOAT3 _Degree);

	static	unique_ptr<GUIEditor>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>				GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>			DEVCTX = { nullptr };

	vector<shared_ptr<Animator>>		AnimatorList			= { nullptr };
	vector<shared_ptr<MeshLoader>>		MeshList				= { nullptr };
	shared_ptr<Animator>				Selected_Object			= { nullptr };
	shared_ptr<MeshLoader>				Selected_Mesh			= { nullptr };
	shared_ptr<Animator>				Dump_Object				= { nullptr };
	shared_ptr<Animation>				CurrentAnimation		= { nullptr };

	/////////////////////////////////////////////////////////////////// IMGUIZMO
	shared_ptr<GameObject>				PreviouseRayRecipient	= { nullptr };
	ImGuizmo::OPERATION					GuizmoState				= {         };
	ImGuizmo::MODE						GuizmoMode				= {         };
	XMMATRIX							GizmoMatrix				= {         };
	float								GuizmoTransformMat[16]	= {         };

	float alpha = 0.f;
	ax::NodeEditor::EditorContext* g_Context;

	unordered_map<string, Effekseer::EffectRef>*		EffectProtoList;

	vector<shared_ptr<UIObject>>		UIList;
	shared_ptr<UIObject>    SelectedUIObject = { nullptr };
	shared_ptr<PlayerEffect>    SelectedEffectObject = { nullptr };

	shared_ptr<Shader>		Component_Shader = { nullptr };
	unordered_map<string, shared_ptr<PlayerEffect>>* EffectPrototypeList = { nullptr };

	_float LightDirection[4]	= { -1.f, -1.f, 1.f, 0.f};
	_float LightDiffuse[4]		= { 1.f, 0.9f, 0.9f, 1.f };
	_float LightAmbient[4]		= { 1.f,  1.f, 1.f, 1.f };
	_float LightSpecular[4]		= { 0.25f, 0.25f, 0.25f, 0.25f };

	_float RimLightDiffuse[3]	= { 15.f, 0.f, 0.f };
	_float RimLightIntensity	= { 5.f };

	_float EmissiveDiffuse[4]	= {0.56f, 0.f, 1.f, 1.f};
	_float EmissiveIntensity	= { 0.1f };

	_float MatCapIntensity		= { 0.3f };

	_float	 ShadowBorderLine	= { 0.2f };
	_float	 ShadowScattering	= { 0.015f };
	_float	 ShadowIntensity	= { 1.1f };
	_float   BrightIntensity	= { 1.4f };
	////////////////////////////////////////////////////////
	//_float		EffectScrollSpeed[2] = { -0.7f, 0.3f };
	//_float      EffectAlpha = { 1.f };
	//
	//_float		EffectEmissiveColor[3] = { 0.f, 0.f, 0.f };
	//_float      EffectEmissiveIntensity = { 0.f };
	//
	//_float		DissolveScrollSpeed[2] = { -0.3f, 0.f };
	//
	//_float      DissolveStrength	= { 0.2f };
	//_float      DissolveEdgeWidth	= { 1.5f };
	//_float      DissolveEdgeColor[3] = { 1.f, 1.f, 1.f };
	//
	//_float		EffectTextureOffset[2] = { 0.2f, 0.f };
	//_float		DissolveTextureOffset[2] = { 0.f, 0.f };
	//
	//_float      EffectFadeOutValue = { 0.2f };
}; 
END