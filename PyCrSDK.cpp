#include "PyCrSDK.hpp"
#include <iostream>

namespace SDK = SCRSDK;

// ----------------------------------------------------------------
PyCrSDK::PyCrSDK() = default;
PyCrSDK::~PyCrSDK() { if (m_sdk_inited) sdk_release(); }
// ----------------------------------------------------------------

// ==== SDK init / release ========================================
bool PyCrSDK::sdk_init()
{
    if (m_sdk_inited) return true;
    if (!SDK::Init()) { std::cerr << "[PyCrSDK] SDK Init failed\n"; return false; }

    //UpdateSDKInformation();

    m_sdk_inited = true;
    return true;
}

void PyCrSDK::sdk_release()
{
    if (!m_sdk_inited) return;
    //disconnect_camera();
    SDK::Release();
    m_sdk_inited = false;
}
// ----------------------------------------------------------------

// ==== enumeration ==============================================
int PyCrSDK::enumerate_cameras()
{
    m_cam_list.clear();
    auto err = SDK::EnumCameraObjects(&camera_list);
    if (CR_FAILED(err) || !camera_list){
        std::cerr << err << "[PyCrSDK] EnumCameraObjects failed\n";
        return {};
    }
    
    auto ncams = camera_list->GetCount();
    cli::tout << "Camera enumeration successful. " << ncams << " detected.\n\n";

    for (CrInt32u i = 0; i < ncams; ++i) {
        auto camera_info = camera_list->GetCameraObjectInfo(i);
        cli::text conn_type(camera_info->GetConnectionTypeName());
        cli::text model(camera_info->GetModel());
        cli::text id = TEXT("");
        if (TEXT("IP") == conn_type) {
            //id.append((TCHAR*)camera_info->GetMACAddressChar());
            // or
            id.append((TCHAR*)camera_info->GetMACAddressChar(), (size_t)camera_info->GetMACAddressCharSize());
        }
        else id = ((TCHAR*)camera_info->GetId());
        cli::tout << '[' << i << "] " << model.data() << " (" << id.data() << ")\n";
    }

    /*
    const CrInt32u n = list->GetCount();
    std::vector<std::string> names; names.reserve(n);
    m_cam_list.reserve(n);

    for (CrInt32u i = 0; i < n; ++i)
    {
        const auto* info = list->GetCameraObjectInfo(i);   // const*
        if (!info) continue;
        //std::wstring w(info->GetModel());
        //std::string  model(w.begin(), w.end());
        const char* cstr = reinterpret_cast<const char*>(info->GetModel());
        std::string model(cstr ? cstr : "");

        m_cam_list.push_back({info, model});
        names.push_back(model);
    }
    // list->Release();  // 必要なら解放
    return names;
    */
   return ncams;
}
// ----------------------------------------------------------------

// ==== connect / disconnect ======================================
bool PyCrSDK::connect_camera(int no, int mode)
{
    if (!camera_list){
        std::cerr << "[PyCrSDK] No camera list available. Please enumerate cameras first.\n";
        return false;
    }
    if (camera_list->GetCount() < no) {
        cli::tout << "input value over \n";
        return false;
    }

    std::int32_t cameraNumUniq = no;

    CameraDevicePtr camera = nullptr;
    if (!findTarget(no, camera, false)) {
        cli::tout << "Connect to selected camera...\n";
        auto* camera_info = camera_list->GetCameraObjectInfo(no);

        cli::tout << "Create camera SDK camera callback object.\n";
        camera = CameraDevicePtr(new cli::CameraDevice(cameraNumUniq, camera_info));
        cameraList.push_back(camera); // add 1st
    }

    if (camera->is_connected()) {
        cli::tout << "Please disconnect\n";
    }
    else {
        if (mode < 0 || mode > 2) {
            cli::tout << "Invalid mode. Use 0=Remote, 1=ContentsTransfer, 2=RemoteTransfer.\n";
            return false;
        }
        SDK::CrSdkControlMode openMode = static_cast<SDK::CrSdkControlMode>(mode);
        SDK::CrReconnectingSet reconnect = SDK::CrReconnecting_ON;  
        auto success = camera->connect(openMode, reconnect);
        if (!success) {
            cli::tout << "Failed to connect to camera.\n";
            return false;
        }
        cli::tout << "Connected to: " << camera->get_model() << "\n";
    }

    //cli::tout << "Release enumerated camera list.\n";
    //camera_list->Release();
    return true;
}

bool PyCrSDK::disconnect_camera(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    if (camera->is_connected()) {
        camera->disconnect();
    }
    return true;
}
// ----------------------------------------------------------------

// ==== shutter ====================================================
bool PyCrSDK::capture_image(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    camera->af_shutter();
    return true;
}

bool PyCrSDK::execute_movie_rec(int no, bool down)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    camera->execute_movie_rec(down);
    return true;
}

// get ISO value
int PyCrSDK::get_iso(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    camera->get_iso();
    return 100;
}

bool PyCrSDK::get_live_view(int no, py::buffer py_buf)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    camera->get_live_view(0, py_buf); // 0 for LiveViewOnly
    return true;
}

bool PyCrSDK::download_latest_files(int no, int slot, int file_num, int mode)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    SDK::CrSlotNumber slotNumber =
        (slot == 0) ? SDK::CrSlotNumber_Slot1
                    : SDK::CrSlotNumber_Slot2;
    RemoteTransferDataKind kind =
        (mode == 0) ? RemoteTransferDataKind::Contents
                    : (mode == 1) ? RemoteTransferDataKind::Thumbnail
                                  : RemoteTransferDataKind::Screennail;
    return camera->download_latest_files(slotNumber, file_num, kind);
}

// ----------------------------------------------------------------

std::string PyCrSDK::get_connected_model() const
{
    return m_connected ? m_current_model : "";
}
// ----------------------------------------------------------------

bool PyCrSDK::findTarget(int no, CameraDevicePtr& camera, bool check_connected)
{
    bool findTarget = false;
    CameraDeviceList::const_iterator it = cameraList.begin();
    for (; it != cameraList.end(); ++it) {
        if ((*it)->get_number() == no) {
            findTarget = true;
            camera = (*it);
            break;
        }
    }
    if (!findTarget) {
        cli::tout << "The specified camera cannot be found!\n";
        return false;
    }
    if(check_connected && !camera->is_connected()) {
        cli::tout << "The specified camera is not connected!\n";
        return false;
    }
    return true;
}
