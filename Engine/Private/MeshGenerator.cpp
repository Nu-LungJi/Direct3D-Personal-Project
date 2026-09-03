#include "MeshGenerator.h"
#include "GameInstance.h"
MeshGenerator::MeshGenerator(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}

HRESULT MeshGenerator::Ready_MeshGenerator() {

    SceneListElement = { "Default(CurrentScene)" };
    for (uint32_t IDX = 1; IDX < GameInstance::GetInstance().Get_SceneManager()->Get_MaxSceneCount() + 1; ++IDX)
        SceneListElement.push_back("Scene" + to_string(IDX));

    LayerListElement = { "Default(LastLayer)" };
    for (uint32_t IDX = 1; IDX < GameInstance::GetInstance().Get_SceneManager()->Get_MaxLayerCount() + 1; ++IDX)
        LayerListElement.push_back("Layer" + to_string(IDX));

    Resource_FolderName = { "MapObject Folder" , "Building Folder", "LandScape Folder", "Foliage Folder", "BossMap Folder" };

    Activate_FolderExplorer = true;
    Activate_MeshExplorer = false;

    Resource_FileName.resize(Resource_FolderName.size());
    LayerIndexList.resize((uint32_t)LANDSCAPE::TYPE_END);

    return S_OK;
}
HRESULT MeshGenerator::Generate_MeshObject(const string& _ButtonLabel, LANDSCAPE _MTYPE) {
    uint32_t SelectedSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
    if (Resource_FileName[(uint32_t)_MTYPE].size() == 0)     return E_FAIL;

    for (uint32_t IDX = 0; IDX < Resource_FileName[(uint32_t)_MTYPE].size(); ++IDX) {
        if (_ButtonLabel == "Generate Model" + to_string(IDX + 1)) {
            shared_ptr<GameObject> GOBJ = GameInstance::GetInstance().Get_ProtoManager()->
                Clone_ProtoType(SelectedSceneIndex, Resource_FileName[(uint32_t)_MTYPE][IDX].string(), Resource_FileName[(uint32_t)_MTYPE][IDX].string(), nullptr);
            GOBJ->Set_AssetPath(Resource_FileName[(uint32_t)_MTYPE][IDX].string());
            GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(GOBJ);
            GameInstance::GetInstance().Get_RayCaster()->Set_CastedObject(GOBJ);
            GameInstance::GetInstance().Get_SceneManager()->Add_GameObject(SelectedSceneIndex, LayerIndexList[(uint32_t)_MTYPE], GOBJ);

            shared_ptr<Transform> CameraTransform = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_MainCamera()->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
            XMVECTOR IntallPosition = GameInstance::GetInstance().Get_MainCamera()->Get_CameraPosition() + 5 * CameraTransform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK);
            shared_ptr<Transform> TRS = static_pointer_cast<Transform>(GOBJ->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

            TRS->Set_WorldPosition(IntallPosition);
            static_pointer_cast<MeshLoader>(GOBJ->Find_Component(COMPONENT_TYPE::COMPONENT_MODEL))->Update_BoundingBox(TRS->Get_WorldMatrix());

            return S_OK;
        }
    }
    return S_OK;
}

VOID MeshGenerator::IMGUI_MeshGenerator() {
    ImGui::Begin("Mesh Generator");

    IMGUI_SelectFolder();

    IMGUI_SelectMesh(LANDSCAPE::BUILDING);
    IMGUI_SelectMesh(LANDSCAPE::LANDSCAPE);
    IMGUI_SelectMesh(LANDSCAPE::MAPOBJ);
    IMGUI_SelectMesh(LANDSCAPE::FOLIAGE);

    IMGUI_SelectMesh(LANDSCAPE::BOSS);

    ImGui::End();
}

VOID MeshGenerator::IMGUI_SelectFolder() {
    uint32_t ColumnCount = 2;
    size_t LineCount = 4;
    if (Activate_FolderExplorer) {
        ImGui::Columns(ColumnCount, NULL, true);
        for (uint32_t LIDX = 0; LIDX < LineCount; ++LIDX) {
            ImGui::Separator();
            for (uint32_t CIDX = 0; CIDX < ColumnCount; ++CIDX) {
                if (LIDX * 2 + CIDX >= Resource_FolderName.size())    break;
                ImGui::PushID(ColumnCount * LIDX + CIDX + 1);
                ImGui::Text(Resource_FolderName[LIDX * 2 + CIDX].c_str());
                string ButtonLabel = "Open Folder";
                if (ImGui::Button(ButtonLabel.c_str(), ImVec2(-FLT_MIN, 0.0f))) {
                    CurrentOpenFolder = Resource_FolderName[LIDX * 2 + CIDX];
                    Activate_FolderExplorer = false;
                    Activate_MeshExplorer = true;
                }
                ImGui::PopID();
                ImGui::NextColumn();
            }
        }
        ImGui::Columns(1);
        ImGui::Separator();
    }
}
VOID MeshGenerator::IMGUI_SelectMesh(LANDSCAPE _MTYPE) {
    if (Activate_MeshExplorer && CurrentOpenFolder == Resource_FolderName[(uint32_t)_MTYPE]) {
        if (ImGui::Button("Back")) {
            CurrentOpenFolder = "";
            Activate_FolderExplorer = true;
            Activate_MeshExplorer = false;
        }
        uint32_t ColumnCount = 3, LineCount = static_cast<uint32_t>(Resource_FileName[(uint32_t)_MTYPE].size()) / ColumnCount + 1;
        ImGui::Columns(ColumnCount, NULL, true);
        for (uint32_t LIDX = 0; LIDX < LineCount; ++LIDX) {
            for (uint32_t CIDX = 0; CIDX < ColumnCount; ++CIDX) {
                if (ColumnCount * LIDX + CIDX < Resource_FileName[(uint32_t)_MTYPE].size()) {
                    if (CIDX == 0) ImGui::Separator();
                    ImGui::PushID(ColumnCount * LIDX + CIDX + 1);
                    ImGui::Text(Resource_FileName[(uint32_t)_MTYPE][ColumnCount * LIDX + CIDX].string().c_str());
                    string ButtonLabel = "Generate Model" + to_string(ColumnCount * LIDX + CIDX + 1);
                    if (ImGui::Button(ButtonLabel.c_str(), ImVec2(-FLT_MIN, 0.0f)))
                        Generate_MeshObject(ButtonLabel, _MTYPE);

                    ImGui::PopID();
                    ImGui::NextColumn();
                }
            }
        }
        ImGui::Columns(1);
        ImGui::Separator();
    }
}

VOID MeshGenerator::Generator_Controller() {
    if (KEY_HOLD(DIK_LCONTROL) && KEY_DOWN(DIK_C)) {
        SelectedObject = GameInstance::GetInstance().Get_RayCaster()->Get_CastedObject();
    }
    if (KEY_HOLD(DIK_LCONTROL) && KEY_DOWN(DIK_V)) {
        if (nullptr == SelectedObject) return;
        uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
        uint32_t CurrentContainedLayerIndex = 0;
        string CopiedTag = SelectedObject->Get_ObjectTag();
        
        vector<shared_ptr<Layer>> LayerList = GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_LayerList();
        for (uint32_t IDX = 0; IDX < LayerList.size(); ++IDX) {
            shared_ptr<GameObject> CheckForLayerIndex = LayerList[IDX]->Get_GameObject(CopiedTag);
            if (nullptr == CheckForLayerIndex)   continue;

            CurrentContainedLayerIndex = IDX;
            break;
        }

        shared_ptr<GameObject> GOBJ = GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, SelectedObject->Get_AssetPath(), CopiedTag, nullptr);
        GOBJ->Set_AssetPath(SelectedObject->Get_AssetPath());

        GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(GOBJ);
        GameInstance::GetInstance().Get_RayCaster()->Set_CastedObject(GOBJ);
        GameInstance::GetInstance().Get_SceneManager()->Add_GameObject(CurrentSceneIndex, CurrentContainedLayerIndex, GOBJ);

        shared_ptr<Transform> GOBJTransform = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_MainCamera()->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
        XMVECTOR IntallPosition = GameInstance::GetInstance().Get_MainCamera()->Get_CameraPosition() + 5 * GOBJTransform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK);

        static_pointer_cast<Transform>(GOBJ->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_WorldPosition(IntallPosition);
    }
}
HRESULT MeshGenerator::Import_FBXFiles(const string& _FolderPath, LANDSCAPE _MeshType) {

    if (!filesystem::exists(_FolderPath)) {
        MSG_BOX("Directory does not exist");
        return E_FAIL;
    }
    if (!filesystem::is_directory(_FolderPath)) {
        MSG_BOX("This is not Directory");
        return E_FAIL;
    }

    std::unordered_map<std::string, filesystem::path> FileHashMap;

    SCENE_DESC SceneOption = GameInstance::GetInstance().Get_SceneOption();
    uint32_t CurrentSceneIndex = SceneOption.CurrentSceneIndex;

    // 하위 폴더 전부 탐색, 특정 확장자(.bin, .fbx) 파일들만 파일명을 문자열로 저장.
    for (const auto& File_iterator : filesystem::recursive_directory_iterator(_FolderPath)) {
        if (File_iterator.is_regular_file() == FALSE)    continue;

        filesystem::path File_Path = File_iterator.path();              // 파일 경로
        filesystem::path File_Stem = File_Path.stem();                  // 파일 이름, 경로 제외
        string File_Extension = File_Path.extension().string();         // 파일 확장자

        if (File_Extension == ".bin") {
            GameInstance::GetInstance().Get_ProtoManager()->Add_ProtoType(CurrentSceneIndex, File_Stem.string(),
                MapObject::Create(GRPDEV, DEVCTX, File_Path.wstring(), CurrentSceneIndex));
            Resource_FileName[static_cast<uint32_t>(_MeshType)].push_back(File_Stem);           // 리소스 파일 이름 저장.
        }
        else if (File_Extension == ".fbx") {
            GameInstance::GetInstance().Get_ProtoManager()->Add_ProtoType(CurrentSceneIndex, File_Stem.string(),
                MapObject::Create(GRPDEV, DEVCTX, File_Path.wstring(), CurrentSceneIndex));
            Resource_FileName[static_cast<uint32_t>(_MeshType)].push_back(File_Stem);           // 리소스 파일 이름 저장.
        }
    }
    return S_OK;
}
unique_ptr<MeshGenerator> MeshGenerator::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto Instance = unique_ptr<MeshGenerator>(new MeshGenerator(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Ready_MeshGenerator())) {
        MSG_BOX("Cannot Create MeshGenerator.");
        return nullptr;
    }
    return Instance;
}