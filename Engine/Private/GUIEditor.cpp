#include "GUIEditor.h"
#include "GameInstance.h"
#include "MeshLoader.h"
#include "UIObject.h"
#include "../../Client/Public/Player.h"


GUIEditor::GUIEditor(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX){}
GUIEditor::~GUIEditor() {
    Release_GUIEditor();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);


HRESULT GUIEditor::Ready_GUIEditor()
{
    Selected_Mesh = nullptr;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    Setting_DarkTheme();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;

    ImGuiStyle& Style = ImGui::GetStyle();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    Style.ScaleAllSizes(main_scale);
    Style.FontScaleDpi = main_scale;
    Style.WindowRounding = 5.f;
    Style.Colors[ImGuiCol_WindowBg].w = 1.f;

    ImGui_ImplWin32_Init(GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription().hWnd);
    ImGui_ImplDX11_Init(GRPDEV.Get(), DEVCTX.Get());

    Selected_Object = nullptr;
    Dump_Object = nullptr;
    PreviouseRayRecipient = nullptr;

    GuizmoState = ImGuizmo::TRANSLATE;
    GuizmoMode = ImGuizmo::WORLD;

    GizmoMatrix = MAKE_FLOAT_MATRIX_IDT;
    float IdentityMat[] = MAKE_FLOAT_MATRIX_IDT;
    memcpy(GuizmoTransformMat, IdentityMat, sizeof(float) * 16);

    ax::NodeEditor::Config Config;
    Config.SettingsFile = "BehaviorTree.json";

    g_Context = ax::NodeEditor::CreateEditor(&Config);
    EffectProtoList = GameInstance::GetInstance().Get_EffectManager()->Get_EffectList();

    return S_OK;
}

VOID    GUIEditor::Render_Begin_GUIEditor() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    RECT rc{};
    HWND hWnd = GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription().hWnd;
    GetWindowRect(hWnd, &rc);

    ImGuizmo::SetRect(static_cast<_float>(rc.left), static_cast<_float>(rc.top), 1280.f, 720.f);
}
VOID    GUIEditor::Render_GUIEditor() {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    GameInstance::GetInstance().Get_MeshGenerator()->IMGUI_MeshGenerator();
    IMGUIZMO_ObjectTransform();
    //IMGUI_EffectEditor();
    //IMGUI_Navigator();
    //IMGUI_EffectController();
    //IMGUI_UIEditor();
    //IMGUI_ShaderController();
}   
VOID    GUIEditor::Render_End_GUIEditor()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}
VOID    GUIEditor::Setting_DarkTheme() {
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}
_bool   GUIEditor::PreOccupy_IMGUI(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) return true;
    return false;
}

VOID    GUIEditor::IMGUIZMO_ObjectTransform() {

    XMFLOAT4X4 ViewMatrix, ProjMatrix, WorldMatrix;
    shared_ptr<GameObject> RayRecipient = GameInstance::GetInstance().Get_RayCaster()->Get_CastedObject();
    if (nullptr == RayRecipient) return;

    if (PreviouseRayRecipient != RayRecipient) {
        XMFLOAT4X4 WORLD = {};
        XMStoreFloat4x4(&WORLD, static_pointer_cast<Transform>(RayRecipient->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_WorldMatrix());
        memcpy(GuizmoTransformMat, &WORLD, sizeof(_float) * 16);
        PreviouseRayRecipient = RayRecipient;
    }

    if (nullptr != RayRecipient) {
        ImGui::Begin("Transform Controller");
        ImGuizmo::SetOrthographic(false);

        shared_ptr<Transform> POTransform = static_pointer_cast<Transform>(RayRecipient->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
        XMStoreFloat4x4(&ViewMatrix, static_pointer_cast<Camera>(GameInstance::GetInstance().Get_MainCamera())->Get_ViewMatrix());
        XMStoreFloat4x4(&ProjMatrix, static_pointer_cast<Camera>(GameInstance::GetInstance().Get_MainCamera())->Get_ProjMatrix());
        XMStoreFloat4x4(&WorldMatrix, POTransform->Get_WorldMatrix());

        float View[] = MAKE_FLOAT_MATRIX(ViewMatrix); float Proj[] = MAKE_FLOAT_MATRIX(ProjMatrix); float World[] = MAKE_FLOAT_MATRIX(WorldMatrix);

        
        ImGui::SeparatorText("Standard Axis");

        if (!(GuizmoState == ImGuizmo::SCALE)) {
            if (ImGui::RadioButton("LocalAxis", GuizmoMode == ImGuizmo::LOCAL)) GuizmoMode = ImGuizmo::LOCAL;        ImGui::SameLine();
            if (ImGui::RadioButton("WorldAxis", GuizmoMode == ImGuizmo::WORLD)) GuizmoMode = ImGuizmo::WORLD;        ImGui::NewLine();
        }

        ImGui::SeparatorText("GameObject Transform");

        if (ImGui::RadioButton("TRS", GuizmoState == ImGuizmo::TRANSLATE))      GuizmoState = ImGuizmo::TRANSLATE;   ImGui::SameLine();
        if (ImGui::RadioButton("ROT", GuizmoState == ImGuizmo::ROTATE))         GuizmoState = ImGuizmo::ROTATE;      ImGui::SameLine();
        if (ImGui::RadioButton("SCA", GuizmoState == ImGuizmo::SCALE))          GuizmoState = ImGuizmo::SCALE;       ImGui::NewLine();

        if (!(ImGuizmo::IsUsing())) {
            XMVECTOR POSVEC = POTransform->Get_WorldPosition(), ROTVEC = POTransform->Get_WorldRotationQuat(), SCAVEC = POTransform->Get_WorldScale();

            XMFLOAT3 Degree = QuaternionToDegree(ROTVEC);

            float TRSMat[3] = { XMVectorGetX(POSVEC), XMVectorGetY(POSVEC), XMVectorGetZ(POSVEC) };
            float ROTMat[3] = { Degree.x, Degree.y, Degree.z };
            float SCAMat[3] = { XMVectorGetX(SCAVEC), XMVectorGetY(SCAVEC), XMVectorGetZ(SCAVEC) };

            if (ImGui::InputFloat3("TRS", TRSMat)) { POTransform->Set_WorldPosition( TRSMat[0], TRSMat[1], TRSMat[2] ); };
            if (ImGui::InputFloat3("ROT", ROTMat)) { 
                XMVECTOR NewQuat = DegreeToQuaternion({ ROTMat[0], ROTMat[1], ROTMat[2] });
                POTransform->Set_WorldRotationQuat(NewQuat);
            };
            if (ImGui::InputFloat3("SCA", SCAMat)) {
                if (SCAMat[0] == 0.f || isnan(SCAMat[0])) SCAMat[0] = 0.01f; 
                if (SCAMat[1] == 0.f || isnan(SCAMat[1])) SCAMat[1] = 0.01f; 
                if (SCAMat[2] == 0.f || isnan(SCAMat[2])) SCAMat[2] = 0.01f;

                POTransform->Set_WorldScale(SCAMat[0], SCAMat[1], SCAMat[2]);
            };
        }
        XMFLOAT4X4 WORLD = {};
        XMStoreFloat4x4(&WORLD, static_pointer_cast<Transform>(RayRecipient->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_WorldMatrix());
        memcpy(GuizmoTransformMat, &WORLD, sizeof(_float) * 16);

        ImGuizmo::Manipulate(View, Proj, GuizmoState, GuizmoMode, GuizmoTransformMat, NULL, NULL);

        if (ImGuizmo::IsUsing()) {
            XMFLOAT4X4 UpdatedMatrix = {
                GuizmoTransformMat[0],  GuizmoTransformMat[1],  GuizmoTransformMat[2],  GuizmoTransformMat[3],
                GuizmoTransformMat[4],  GuizmoTransformMat[5],  GuizmoTransformMat[6],  GuizmoTransformMat[7],
                GuizmoTransformMat[8],  GuizmoTransformMat[9],  GuizmoTransformMat[10], GuizmoTransformMat[11],
                GuizmoTransformMat[12], GuizmoTransformMat[13], GuizmoTransformMat[14], GuizmoTransformMat[15]
            };
            XMMATRIX TransformMatrix = XMLoadFloat4x4(&UpdatedMatrix);

            XMVECTOR Translation, RotationQuat, Scale;
            XMMatrixDecompose(&Scale, &RotationQuat, &Translation, TransformMatrix);

            POTransform->Set_WorldPosition(GuizmoTransformMat[12], GuizmoTransformMat[13], GuizmoTransformMat[14]);
            POTransform->Set_WorldRotationQuat(RotationQuat);

            XMFLOAT4 ScaleVec;
            XMStoreFloat4(&ScaleVec, Scale);
            if (ScaleVec.x == 0.f || isnan(ScaleVec.x)) ScaleVec.x = 0.01f;
            if (ScaleVec.y == 0.f || isnan(ScaleVec.y)) ScaleVec.y = 0.01f;
            if (ScaleVec.z == 0.f || isnan(ScaleVec.z)) ScaleVec.z = 0.01f;

            POTransform->Set_WorldScale(XMLoadFloat4(&ScaleVec));
        }
        ImGui::End();
    }
}

VOID GUIEditor::IMGUI_EffectEditor() {
    ImGui::Begin("EffectEditor");
    if (ImGui::TreeNode("Effecter")) {
        for (uint32_t IDX = 0; IDX < EffectProtoList->size(); ++IDX) {
            string Label = "Effect" + to_string(IDX);
            if (ImGui::Selectable(Label.c_str())) {
                //GameInstance::GetInstance().Get_EffectManager()->Play_Effect(IDX, { 0.f, 0.f, 0.f }, { 5.f, 5.f, 5.f }, 100.f);
            }
            ImGui::Separator();
        }
        ImGui::TreePop();
    }
    ImGui::End();
}

VOID GUIEditor::IMGUI_Navigator() {
    ImGui::Begin("Navigator");
    auto GameObjectList = GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_Layer(5)->Get_GameObjectList();
    shared_ptr<GameObject> GOBJ = GameInstance::GetInstance().Get_Navigator()->Get_CastedObject();

    if (nullptr != GOBJ) {
        string OBJText = "Selected Object : " + GOBJ->Get_ObjectTag();
        ImGui::TextEx(OBJText.c_str());
    }
    ImGui::SeparatorText("NavMesh Type Selector");
    _bool CLock = GameInstance::GetInstance().Get_Navigator()->Get_ControlLock();
    if(ImGui::Checkbox("Control Lock", &CLock)) {
        ImGui::SameLine();
        GameInstance::GetInstance().Get_Navigator()->Set_ControlLock(true);
        string LockText = "Locked Object : " + GOBJ->Get_ObjectTag();
        ImGui::TextEx(LockText.c_str());
    }

    static bool NavMeshTypeFlag[] = { true, false, false, false };
	ImGui::SeparatorText("NavMesh Type Selector");
    if (ImGui::Checkbox("Horizontal", &NavMeshTypeFlag[0])) {
        if (NavMeshTypeFlag[1]) NavMeshTypeFlag[1] = false;
        if (NavMeshTypeFlag[2]) NavMeshTypeFlag[2] = false;
        if (NavMeshTypeFlag[3]) NavMeshTypeFlag[3] = false;
        GameInstance::GetInstance().Get_Navigator()->Set_CurrentNavMeshType(NAVMESH_TYPE::NAVMESH_HORIZONTAL);
    }ImGui::SameLine();
    if (ImGui::Checkbox("Vertical", &NavMeshTypeFlag[1])) {
        if (NavMeshTypeFlag[0]) NavMeshTypeFlag[0] = false;
        if (NavMeshTypeFlag[2]) NavMeshTypeFlag[2] = false;
        if (NavMeshTypeFlag[3]) NavMeshTypeFlag[3] = false;
        GameInstance::GetInstance().Get_Navigator()->Set_CurrentNavMeshType(NAVMESH_TYPE::NAVMESH_VERTICAL);
    }ImGui::SameLine();
    if (ImGui::Checkbox("Diagonal", &NavMeshTypeFlag[2])) {
        if (NavMeshTypeFlag[0]) NavMeshTypeFlag[0] = false;
        if (NavMeshTypeFlag[1]) NavMeshTypeFlag[1] = false;
        if (NavMeshTypeFlag[3]) NavMeshTypeFlag[3] = false;
        GameInstance::GetInstance().Get_Navigator()->Set_CurrentNavMeshType(NAVMESH_TYPE::NAVMESH_DIAGONAL);
    }
    if (ImGui::Checkbox("Block", &NavMeshTypeFlag[3])) {
        if (NavMeshTypeFlag[0]) NavMeshTypeFlag[0] = false;
        if (NavMeshTypeFlag[1]) NavMeshTypeFlag[1] = false;
        if (NavMeshTypeFlag[2]) NavMeshTypeFlag[2] = false;
        GameInstance::GetInstance().Get_Navigator()->Set_CurrentNavMeshType(NAVMESH_TYPE::NAVMESH_BLOCK);
    }

    else if (!NavMeshTypeFlag[0] && !NavMeshTypeFlag[1] && !NavMeshTypeFlag[2] && !NavMeshTypeFlag[3]) {
        NavMeshTypeFlag[0] = true;
        GameInstance::GetInstance().Get_Navigator()->Set_CurrentNavMeshType(NAVMESH_TYPE::NAVMESH_HORIZONTAL);
    }

    if (ImGui::TreeNode("NVG")) {
        int32_t front = 0;
        for (auto iter = GameObjectList.begin(); iter != GameObjectList.end(); iter++) {
            string  Label = to_string(front++) + "." + (*iter)->Get_ObjectTag();
            if (ImGui::Selectable(Label.c_str())) {
                GameInstance::GetInstance().Get_Navigator()->Set_CastedObject(*iter);
                GameInstance::GetInstance().Get_RayCaster()->Set_CastedObject(*iter);
            }
            ImGui::Separator();
        }
        ImGui::TreePop();
    }
    ImGui::End();
}

VOID GUIEditor::IMGUI_UIEditor() {
    ImGui::Begin("UI Editor");

    if (ImGui::TreeNode("UIE")) {
        int32_t front = 0;
        for (auto iter = UIList.begin(); iter != UIList.end(); iter++) {
            string  Label = to_string(front++) + "." + (*iter)->Get_ObjectTag();
            if (ImGui::Selectable(Label.c_str())) {
                SelectedUIObject = (*iter);
            }
            ImGui::Separator();
        }
        ImGui::TreePop();
    }

    ImGui::SeparatorText("UIObject Transform");
    if (nullptr != SelectedUIObject) {
        _float Pos[2]   = { SelectedUIObject->Get_Position().x, SelectedUIObject->Get_Position().y };
        _float Sca      = SelectedUIObject->Get_Scale();
        _float Opc      = SelectedUIObject->Get_Alpha();

        if (ImGui::InputFloat2("Pos", Pos)) {
            SelectedUIObject->Set_Position({ Pos[0], Pos[1] });
        }
        if (ImGui::InputFloat("ScaX", &Sca)) { 
            SelectedUIObject->Set_Scale(Sca);
        }
        if (ImGui::InputFloat("Opc", &Opc)) { 
            SelectedUIObject->Set_Alpha(Opc);
        }
    }
    
    ImGui::End();
}

VOID GUIEditor::IMGUI_ShaderController() {
    if (nullptr == Component_Shader) {
        Component_Shader = static_pointer_cast<Shader>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(0, COMPONENT_TYPE::COMPONENT_MESHINSTANCE, nullptr));
    }
    ImGui::Begin("Shader Editor"); 
    {
        ImGui::SeparatorText("Light Controller");
        if (ImGui::InputFloat4("LightDirection", LightDirection)) { Component_Shader->Bind_RawValue("g_vLightDir", &LightDirection, sizeof(_float) * 4); }
        if (ImGui::InputFloat4("LightDiffuse", LightDiffuse)) { Component_Shader->Bind_RawValue("g_vLightDiffuse", &LightDiffuse, sizeof(_float) * 4); }
        if (ImGui::InputFloat4("LightAmbient", LightAmbient)) { Component_Shader->Bind_RawValue("g_vLightAmbient", &LightAmbient, sizeof(_float) * 4); }
        if (ImGui::InputFloat4("LightSpecular", LightSpecular)) { Component_Shader->Bind_RawValue("g_vLightSpecular", &LightSpecular, sizeof(_float) * 4); }
    }
    {
        ImGui::SeparatorText("RimLight Controller");
        if (ImGui::InputFloat3("RimLightDiffuse", RimLightDiffuse)) { Component_Shader->Bind_RawValue("g_fRimLightDiffuse", &RimLightDiffuse, sizeof(_float) * 3); } 
        if (ImGui::InputFloat("RimLightIntensity", &RimLightIntensity)) { Component_Shader->Bind_RawValue("g_fRimLightIntensity", &RimLightIntensity, sizeof(_float)); }
    }
    {
        ImGui::SeparatorText("Emissive Controller");
        if (ImGui::InputFloat4("EmissiveDiffuse", EmissiveDiffuse)) { Component_Shader->Bind_RawValue("g_EmissiveDiffuse", &EmissiveDiffuse, sizeof(_float) * 4); }
        if (ImGui::InputFloat("EmissiveIntensity", &EmissiveIntensity)) { Component_Shader->Bind_RawValue("g_EmissiveIntensity", &EmissiveIntensity, sizeof(_float)); }
    }
    {
        ImGui::SeparatorText("MatCap Controller");
        if (ImGui::InputFloat("MatCapIntensity", &MatCapIntensity)) { Component_Shader->Bind_RawValue("g_fMatCapIntensity", &MatCapIntensity, sizeof(_float)); }
    }
    {
        ImGui::SeparatorText("ToonShading Controller");
        if (ImGui::InputFloat("ShadowBorderLine", &ShadowBorderLine)) { Component_Shader->Bind_RawValue("ShadowBorderLine", &ShadowBorderLine, sizeof(_float)); }
        if (ImGui::InputFloat("ShadowScattering", &ShadowScattering)) { Component_Shader->Bind_RawValue("ShadowScattering", &ShadowScattering, sizeof(_float)); }
        if (ImGui::InputFloat("ShadowIntensity", &ShadowIntensity)) { Component_Shader->Bind_RawValue("ShadowIntensity", &ShadowIntensity, sizeof(_float)); }
        if (ImGui::InputFloat("BrightIntensity", &BrightIntensity)) { Component_Shader->Bind_RawValue("BrightIntensity", &BrightIntensity, sizeof(_float)); }
    }
    ImGui::End();
}

VOID GUIEditor::IMGUI_EffectController() {
    ImGui::Begin("Effect Editor");

    if (nullptr == EffectPrototypeList) {
        EffectPrototypeList = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer())->Get_EffectProtoList();
    }

    if (ImGui::TreeNode("EFFECT EDITOR")) {
        int32_t front = 0;
        auto EPList = *EffectPrototypeList;
        for (auto iter = EPList.begin(); iter != EPList.end(); iter++) {
            if (ImGui::Selectable(iter->first.c_str())) {
                SelectedEffectObject = iter->second;
            }
            ImGui::Separator();
        }
        ImGui::TreePop();
    }
    if (ImGui::ButtonEx("Play Effect") || KEY_DOWN(DIK_SPACE)) {
        if (nullptr != SelectedEffectObject) {
            shared_ptr<GameObject>	GOBJ = SelectedEffectObject->Clone(nullptr);
            GOBJ->Set_ObjectTag(SelectedEffectObject->Get_ObjectTag() + "_Clone");
            GOBJ->Set_AssetPath("_PRTTAG");
            GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Add_GameObject(8, GOBJ);
        }
    }
    //if (nullptr != SelectedEffectObject) {
    //
    //    ImGui::SeparatorText("Basic Controller");
    //    _float EffectScale[3] = { SelectedEffectObject->Get_EffectScale().x, SelectedEffectObject->Get_EffectScale().y, SelectedEffectObject->Get_EffectScale().z };
    //    if (ImGui::InputFloat3("EffectScale", EffectScale)) { SelectedEffectObject->Set_EffectScale({ EffectScale[0], EffectScale[1], EffectScale[2] }); }
    //
    //    _float EffectRotation[3] = { SelectedEffectObject->Get_EffectRotation().x, SelectedEffectObject->Get_EffectRotation().y, SelectedEffectObject->Get_EffectRotation().z };
    //    if (ImGui::SliderFloat3("EffectRotation", EffectRotation, 0.f, 360.f)) { SelectedEffectObject->Set_EffectRotation({ EffectRotation[0], EffectRotation[1], EffectRotation[2] }); }
    //
    //    ImGui::SeparatorText("Offset Controller");
    //    _float Distance = SelectedEffectObject->Get_DistanceFromPlayer();
    //    if (ImGui::InputFloat("DistanceFromPlayer", &Distance)) { SelectedEffectObject->Set_DistanceFromPlayer(Distance); }
    //
    //    XMFLOAT2 EffectRot= SelectedEffectObject->Get_EffectOrbitRotation();
    //    _float EffectOrbitRotation[2] = { EffectRot.x, EffectRot.y };
    //    if (ImGui::SliderFloat2("EffectOrbitRotation", EffectOrbitRotation, 0.f, 360.f)) { SelectedEffectObject->Set_EffectOrbitRotation(EffectOrbitRotation[0], EffectOrbitRotation[1]); }
    //
    //    XMFLOAT3 Offset = SelectedEffectObject->Get_EffectOffset();
    //    _float EffectOffset[3] = { Offset.x, Offset.y, Offset.z };
    //    if (ImGui::InputFloat3("EffectOffset", EffectOffset)) { SelectedEffectObject->Set_EffectOffset(XMFLOAT3(EffectOffset[0], EffectOffset[1], EffectOffset[2])); }
    //
    //    ImGui::SeparatorText("Effect Controller");
    //    _float ScrollSpeed[2] = { SelectedEffectObject->Get_EffectScrollSpeed().x, SelectedEffectObject->Get_EffectScrollSpeed().y };
    //    if (ImGui::InputFloat2("EffectScrollSpeed", ScrollSpeed)) { SelectedEffectObject->Set_EffectScrollSpeed({ ScrollSpeed[0], ScrollSpeed[1] }); }
    //
    //    _float EffectAlpha = SelectedEffectObject->Get_EffectAlpha();
    //    if (ImGui::InputFloat("EffectAlpha", &EffectAlpha)) { SelectedEffectObject->Set_EffectAlpha(EffectAlpha); }
    //
    //    _float3 EffectEmissiveColor = SelectedEffectObject->Get_EffectEmissiveColor();
    //    _float  EffectEmissiveIntensity = SelectedEffectObject->Get_EffectEmissiveIntensity();
    //
    //    _float EmissiveColor[4] = { EffectEmissiveColor.x, EffectEmissiveColor.y, EffectEmissiveColor.z, EffectEmissiveIntensity };
    //    if (ImGui::InputFloat4("EmissiveColor", EmissiveColor)) { SelectedEffectObject->Set_EmissiveOption(XMFLOAT3(EmissiveColor[0], EmissiveColor[1], EmissiveColor[2]), EmissiveColor[3]); }
    //
    //
    //    ImGui::SeparatorText("Dissolve Controller");
    //
    //    _float DissolveScrollSpeed[2] = { SelectedEffectObject->Get_DissolveScrollSpeed().x, SelectedEffectObject->Get_DissolveScrollSpeed().y };
    //    if (ImGui::InputFloat2("DissolveScrollSpeed", DissolveScrollSpeed)) { SelectedEffectObject->Set_EffectScrollSpeed({ DissolveScrollSpeed[0], DissolveScrollSpeed[1] }); }
    //
    //    _float DissolveStrength = SelectedEffectObject->Get_DissolveStrength();
    //    if (ImGui::InputFloat("DissolveStrength", &DissolveStrength)) { SelectedEffectObject->Set_DissolveStrength(DissolveStrength); }
    //
    //    _float DissolveEdgeWidth = SelectedEffectObject->Get_DissolveEdgeWidth();
    //    if (ImGui::InputFloat("DissolveEdgeWidth", &DissolveEdgeWidth)) { SelectedEffectObject->Set_DissolveEdgeWidth(DissolveEdgeWidth); }
    //
    //    _float3 DissolveEdgeColor = SelectedEffectObject->Get_DissolveEdgeColor();
    //    _float  DissolveColor[3] = { DissolveEdgeColor.x, DissolveEdgeColor.y, DissolveEdgeColor.z };
    //    if (ImGui::InputFloat4("DissolveEdgeColor", DissolveColor)) { SelectedEffectObject->Set_DissolveEdgeColor(XMFLOAT3(DissolveColor[0], DissolveColor[1], DissolveColor[2])); }
    //
    //
    //    ImGui::SeparatorText("Offset Controller");
    //    _float2 EffectTextureOffset = SelectedEffectObject->Get_EffectTextureOffset();
    //    _float  EffectTexOffset[2] = { EffectTextureOffset.x, EffectTextureOffset.y };
    //    if (ImGui::InputFloat2("EffectTextureOffset", EffectTexOffset)) { SelectedEffectObject->Set_EffectTextureOffset(XMFLOAT2(EffectTexOffset[0], EffectTexOffset[1])); }
    //
    //    _float2 DissolveTextureOffset = SelectedEffectObject->Get_DissolveTextureOffset();
    //    _float  DissolveTexOffset[2] = { DissolveTextureOffset.x, DissolveTextureOffset.y };
    //    if (ImGui::InputFloat2("DissolveTextureOffset", DissolveTexOffset)) { SelectedEffectObject->Set_DissolveTextureOffset(XMFLOAT2(DissolveTexOffset[0], DissolveTexOffset[1])); }
    //
    //    _float EffectFadeOutValue = SelectedEffectObject->Get_EffectFadeOutValue();
    //    if (ImGui::InputFloat("EffectFadeOutValue", &EffectFadeOutValue)) { SelectedEffectObject->Set_EffectFadeOutValue(EffectFadeOutValue); }
    //}
    ImGui::End();
}

XMFLOAT3 GUIEditor::QuaternionToDegree(XMVECTOR _Quat) {
    XMFLOAT4 QuatVec;
    XMStoreFloat4(&QuatVec, _Quat);

    _float sinr_cosp = 2.f * (QuatVec.w * QuatVec.z + QuatVec.x * QuatVec.y);
    _float cosr_cosp = 1.f - 2.f * (QuatVec.y * QuatVec.y + QuatVec.z * QuatVec.z);
    _float roll = atan2(sinr_cosp, cosr_cosp);

    _float sinp = 2.f * (QuatVec.w * QuatVec.y - QuatVec.z * QuatVec.x);
    _float pitch;
    if (abs(sinp) >= 1.f)
        pitch = copysign(XM_PI / 2.f, sinp);
    else
        pitch = asin(sinp);

    _float siny_cosp = 2.f * (QuatVec.w * QuatVec.x + QuatVec.y * QuatVec.z);
    _float cosy_cosp = 1.f - 2.f * (QuatVec.x * QuatVec.x + QuatVec.y * QuatVec.y);
    _float yaw = atan2(siny_cosp, cosy_cosp);

    return XMFLOAT3(XMConvertToDegrees(pitch), XMConvertToDegrees(yaw), XMConvertToDegrees(roll));
}

XMVECTOR GUIEditor::DegreeToQuaternion(XMFLOAT3 _Degree) {
    _float Pitch = XMConvertToRadians(_Degree.x);
    _float Yaw = XMConvertToRadians(_Degree.y);
    _float Roll = XMConvertToRadians(_Degree.z);

    return XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll);
}

unique_ptr<GUIEditor>	GUIEditor::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto UM = unique_ptr<GUIEditor>(new GUIEditor(_GRPDEV, _DEVCTX));
    if (FAILED(UM->Ready_GUIEditor())) {
        MSG_BOX("Cannot Create GUIEditor.");
        return nullptr;
    }

    return UM;
}
VOID    GUIEditor::Release_GUIEditor() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}