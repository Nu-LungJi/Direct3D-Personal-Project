#include "MapEditor.h"
#include "MeshLoader.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "WorldMap SaveSchema File_generated.h"

MapEditor::MapEditor(ComPtr<ID3D11Device>  _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}
MapEditor::~MapEditor() {};

HRESULT MapEditor::Initialize_MapEditor() {
    MOBJIndex   = 0;
    SaveAccTime = 0.f;
    Enable_AutoSave = false;

    hWnd = GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription().hWnd;
    return S_OK;
}

HRESULT MapEditor::Save_WorldMap_ObjectList(const string& _Directory, uint32_t _SceneIndex, uint32_t _LayerIndex) {
    
    flatbuffers::FlatBufferBuilder Builder(1024 * 1024);
    // SaveFile에 저장될 모든 맵 오브젝트들 리스트
    vector<flatbuffers::Offset<SaveMapData::MapObjectData>> ObjectList;

    list<shared_ptr<GameObject>> GameObjectList = GameInstance::GetInstance().Get_SceneManager()->Get_Layer(_SceneIndex, _LayerIndex)->Get_GameObjectList();

    for (auto& OBJ : GameObjectList) {
        shared_ptr<Transform>   OBJTRS = static_pointer_cast<Transform>(OBJ->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
        shared_ptr<MeshLoader>  OBJMDL = static_pointer_cast<MeshLoader>(OBJ->Find_Component(COMPONENT_TYPE::COMPONENT_MODEL));

        /////////////////////////////////////// Reference & Tag
        auto AssetPath = Builder.CreateString(static_pointer_cast<MapObject>(OBJ)->Get_AssetPath());
        auto AssetTag  = Builder.CreateString(static_pointer_cast<MapObject>(OBJ)->Get_ObjectTag());
        /////////////////////////////////////// Transform
        SaveMapData::Vector3   Position(XMVectorGetX(OBJTRS->Get_WorldPosition()), XMVectorGetY(OBJTRS->Get_WorldPosition()), XMVectorGetZ(OBJTRS->Get_WorldPosition()));
        SaveMapData::Vector3   Rotation(XMVectorGetX(OBJTRS->Get_WorldRotation()), XMVectorGetY(OBJTRS->Get_WorldRotation()), XMVectorGetZ(OBJTRS->Get_WorldRotation()));
        SaveMapData::Vector3   Scale(XMVectorGetX(OBJTRS->Get_WorldScale()), XMVectorGetY(OBJTRS->Get_WorldScale()), XMVectorGetZ(OBJTRS->Get_WorldScale()));

        auto GameObjectData = SaveMapData::CreateMapObjectData(Builder, AssetPath, AssetTag, &Position, &Rotation, &Scale);

        ObjectList.push_back(GameObjectData);
    }

    // flatbuffer에 정의된 vector로 변환
    auto OBJList = Builder.CreateVector(ObjectList);
    // 월드맵 파일명 지정.
    time_t timer = std::time(nullptr);    tm now;
    localtime_s(&now, &timer);

    auto WorldMapTag = Builder.CreateString("WutheringWave Save File");
    // WorldMapData table 
    auto WorldMap = SaveMapData::CreateWorldMapData(Builder, WorldMapTag, OBJList);
    // Finish
    Builder.Finish(WorldMap);

    // Save as Binary File
    string StreamPath   = _Directory + "WutheringWave Save File";

    string DayString, HourString, MinuteString, SecondString;
    DayString    = now.tm_mday < 10 ? "0" + to_string(now.tm_mday) : to_string(now.tm_mday);
    HourString   = now.tm_hour < 10 ? "0" + to_string(now.tm_hour) : to_string(now.tm_hour);
    MinuteString = now.tm_min  < 10 ? "0" + to_string(now.tm_min)  : to_string(now.tm_min);
    SecondString = now.tm_sec  < 10 ? "0" + to_string(now.tm_sec)  : to_string(now.tm_sec);

    StreamPath = StreamPath + DayString + HourString + MinuteString + SecondString + ".bin";

    ofstream Steam(StreamPath.c_str(), ios::binary);
    Steam.write((char*)Builder.GetBufferPointer(), Builder.GetSize());
    Steam.close();

    return S_OK;
}
HRESULT MapEditor::Load_WorldMap_ObjectList(const string& _Directory, uint32_t _SceneIndex, uint32_t _LayerIndex) {
    
    string FilePath = "";
    Load_LatestFile(_Directory, FilePath);
    if (FilePath.empty()) return E_FAIL;

    ifstream Stream(FilePath.c_str(), ios::binary | ios::ate);
    streamsize StreamSize = Stream.tellg();
    Stream.seekg(0, ios::beg);

    vector<char> Buffer(StreamSize);
    Stream.read(Buffer.data(), StreamSize);

    auto WorldMap = SaveMapData::GetWorldMapData(Buffer.data());

    for (auto OBJ : *WorldMap->objectlist()) {
        XMFLOAT3 POS = { OBJ->position()->x(), OBJ->position()->y() , OBJ->position()->z()  };
        XMFLOAT3 ROT = { OBJ->rotation()->x(), OBJ->rotation()->y() , OBJ->rotation()->z()  };
        XMFLOAT3 SCA = { OBJ->scale()->x()   , OBJ->scale()->y()    , OBJ->scale()->z()     };

        string TAG          = OBJ->objecttag()->str();
        string AssetPath    = OBJ->assetpath()->str();

        shared_ptr<MapObject>  DumpOBJ = static_pointer_cast<MapObject>(GameInstance::GetInstance().Get_ProtoManager()->
            Clone_ProtoType(_SceneIndex, AssetPath, TAG, nullptr));
        if (DumpOBJ == nullptr) continue;
        DumpOBJ->Set_AssetPath(AssetPath);

        shared_ptr<Transform> Component_Transform = static_pointer_cast<Transform>(DumpOBJ->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
        Component_Transform->Set_WorldPosition( POS.x, POS.y, POS.z );
        Component_Transform->Set_WorldRotation( ROT.x, ROT.y, ROT.z );
        Component_Transform->Set_WorldScale( SCA.x, SCA.y, SCA.z );

        GameInstance::GetInstance().Get_SceneManager()->Add_GameObject(_SceneIndex, _LayerIndex, DumpOBJ);
        GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(DumpOBJ);

        shared_ptr<MeshLoader> Component_Model = static_pointer_cast<MeshLoader>(DumpOBJ->Find_Component(COMPONENT_TYPE::COMPONENT_MODEL));
        Component_Model->Update_BoundingBox(Component_Transform->Get_WorldMatrix());
    }

    return S_OK;
}

HRESULT MapEditor::Load_LatestFile(const string& _Directory, string& _Filepath) {
    std::string path = _Directory; // 탐색할 경로
    filesystem::path latest_file;
    filesystem::file_time_type max_time;

    try {
        if (!filesystem::exists(path) || !filesystem::is_directory(path)) {
            std::cout << "경로가 존재하지 않거나 디렉터리가 아닙니다." << std::endl;
            return E_FAIL;
        }

        for (const auto& entry : filesystem::directory_iterator(path)) {
            if (filesystem::is_regular_file(entry) && entry.path().extension() == ".bin") {

                auto current_time = filesystem::last_write_time(entry);
                if (latest_file.empty() || current_time > max_time) {
                    max_time = current_time;
                    latest_file = entry.path();
                }
            }
        }

        if (!latest_file.empty()) {
            std::cout << "가장 최근 .bin 파일: " << latest_file.filename() << std::endl;
            std::cout << "경로: " << latest_file.string() << std::endl;
        }
        else {
            std::cout << ".bin 파일을 찾을 수 없습니다." << std::endl;
        }
    }
    catch (const filesystem::filesystem_error& e) {
        std::cerr << "에러 발생: " << e.what() << std::endl;
    }
    _Filepath = latest_file.string();
    return S_OK;
}

VOID MapEditor::Create_MapObject(uint32_t _SceneIndex, uint32_t _LayerIndex, const string& _CloneTag) {
    string TAG = "MOBJ" + to_string(MOBJIndex++);
    shared_ptr<MapObject> MOBJ = static_pointer_cast<MapObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(_SceneIndex, _CloneTag, TAG, nullptr));
    GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(MOBJ);
    MOBJ->Set_AssetPath(_CloneTag);
    GameInstance::GetInstance().Get_SceneManager()->Add_GameObject(_SceneIndex, _LayerIndex, MOBJ);
}
unique_ptr<MapEditor>	MapEditor::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto UM = unique_ptr<MapEditor>(new MapEditor(_GRPDEV, _DEVCTX));
    if (FAILED(UM->Initialize_MapEditor()))
        MSG_BOX("Cannot Create MapEditor.");

    return UM;
}