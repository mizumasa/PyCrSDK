#include "PyCrSDK.hpp"
#include <iostream>

namespace SDK = SCRSDK;

namespace {
SDK::CrEframingType parse_eframing_type(const std::string& type_name)
{
    if (type_name == "None") {
        return SDK::CrEframingType_None;
    }
    if (type_name == "Auto") {
        return SDK::CrEframingType_Auto;
    }
    if (type_name == "Single") {
        return SDK::CrEframingType_Single;
    }
    if (type_name == "PTZ") {
        return SDK::CrEframingType_PTZ;
    }
    if (type_name == "HoldCurrentPosition") {
        return SDK::CrEframingType_HoldCurrentPosition;
    }
    if (type_name == "ForceZoomOut") {
        return SDK::CrEframingType_ForceZoomOut;
    }
    return SDK::CrEframingType_Single;
}
} // namespace

// ----------------------------------------------------------------
CameraManager::CameraManager() = default;
CameraManager::~CameraManager() { if (m_sdk_inited) sdk_release(); }
// ----------------------------------------------------------------

// ==== SDK init / release ========================================
bool CameraManager::sdk_init()
{
    if (m_sdk_inited) return true;
    if (!SDK::Init()) { std::cerr << "[PyCrSDK] SDK Init failed\n"; return false; }

    //UpdateSDKInformation();

    m_sdk_inited = true;
    return true;
}

void CameraManager::sdk_release()
{
    if (!m_sdk_inited) return;
    //disconnect_camera();
    SDK::Release();
    m_sdk_inited = false;
}
// ----------------------------------------------------------------

// ==== enumeration ==============================================
int CameraManager::enumerate_cameras()
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
bool CameraManager::connect_camera(int no, int mode)
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

bool CameraManager::disconnect_camera(int no)
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
bool CameraManager::capture_image(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    camera->af_shutter();
    return true;
}

bool CameraManager::execute_movie_rec(int no, bool down)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    camera->execute_movie_rec(down);
    return true;
}

// get ISO value
int CameraManager::get_aperture(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_aperture();
}

int CameraManager::get_iso(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_iso();
}

int CameraManager::get_shutter_speed(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_shutter_speed();
}

int CameraManager::get_extended_shutter_speed(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_extended_shutter_speed();
}

void CameraManager::print_aperture(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return;
    camera->print_aperture();
    return; 
}

void CameraManager::print_iso(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return;
    camera->print_iso();
    return;
}

void CameraManager::print_shutter_speed(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return;
    camera->print_shutter_speed();
    return;
}

void CameraManager::print_extended_shutter_speed(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return;
    camera->print_extended_shutter_speed();
    return;
}

bool CameraManager::set_aperture(int no, int value)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_aperture(value);
}

bool CameraManager::set_iso(int no, int value)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_iso(value);
}

bool CameraManager::set_shutter_speed(int no, int value)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_shutter_speed(value);
}

bool CameraManager::set_extended_shutter_speed(int no, int value)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_extended_shutter_speed(value);
}

bool CameraManager::set_camera_eframing(int no, bool on)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_camera_eframing(on);
}

bool CameraManager::execute_eframing(
    int no,
    int in_x, int in_y, int in_w, int in_h,
    int out_x, int out_y, int out_w, int out_h,
    int horizontal_denominator,
    int vertical_denominator,
    const std::string& eframing_type,
    int area_no,
    int ptz_speed)
{
    CameraDevicePtr camera = nullptr;
    if (!findTarget(no, camera, true)) return false;
    if (horizontal_denominator <= 0 || vertical_denominator <= 0) {
        std::cerr << "[PyCrSDK] denominator must be > 0\n";
        return false;
    }
    if (area_no < static_cast<int>(SDK::CrEframingArea1) || area_no > static_cast<int>(SDK::CrEframingArea4)) {
        std::cerr << "[PyCrSDK] area_no must be 1..4\n";
        return false;
    }

    SDK::CrEframingInfo info;
    info.horizontal_denominator = horizontal_denominator * 1024;
    info.vertical_denominator = vertical_denominator * 1024;
    info.eframingType = parse_eframing_type(eframing_type);

    SDK::CrEframingRectangle in_rect;
    in_rect.x = in_x * 1024;
    in_rect.y = in_y * 1024;
    in_rect.width = in_w * 1024;
    in_rect.height = in_h * 1024;

    SDK::CrEframingRectangle out_rect;
    out_rect.x = out_x * 1024;
    out_rect.y = out_y * 1024;
    out_rect.width = out_w * 1024;
    out_rect.height = out_h * 1024;

    info.addInputInfo(static_cast<SDK::CrEframingAreaNumber>(area_no), in_rect);
    info.addOutputInfo(static_cast<SDK::CrEframingAreaNumber>(area_no), out_rect);

    if (info.eframingType == SDK::CrEframingType_PTZ && ptz_speed >= 0) {
        if (!camera->set_eframing_speed_ptz(ptz_speed)) {
            return false;
        }
    }

    auto err = SDK::ExecuteEframing(camera->get_device_handle(), &info);
    if (CR_FAILED(err)) {
        std::cerr << "[PyCrSDK] ExecuteEframing failed: 0x" << std::hex << err << std::dec << "\n";
        return false;
    }
    return true;
}

bool CameraManager::update_eframing_area(
    int no,
    int area_no,
    const std::string& group,
    int dx,
    int dy,
    int dwidth,
    int dheight)
{
    CameraDevicePtr camera = nullptr;
    if (!findTarget(no, camera, true)) return false;
    if (area_no < static_cast<int>(SDK::CrEframingArea1) || area_no > static_cast<int>(SDK::CrEframingArea4)) {
        std::cerr << "[PyCrSDK] area_no must be 1..4\n";
        return false;
    }

    SDK::CrEframingAreaGroup area_group = SDK::CrEframingInputArea;
    if (group == "in" || group == "input") {
        area_group = SDK::CrEframingInputArea;
    } else if (group == "out" || group == "output") {
        area_group = SDK::CrEframingOutputArea;
    } else {
        std::cerr << "[PyCrSDK] group must be 'in'/'input' or 'out'/'output'\n";
        return false;
    }

    auto err = SDK::UpdateEframingArea(
        camera->get_device_handle(),
        static_cast<SDK::CrEframingAreaNumber>(area_no),
        area_group,
        static_cast<CrInt16>(dx * 1024),
        static_cast<CrInt16>(dy * 1024),
        static_cast<CrInt16>(dwidth * 1024),
        static_cast<CrInt16>(dheight * 1024)
    );
    if (CR_FAILED(err)) {
        std::cerr << "[PyCrSDK] UpdateEframingArea failed: 0x" << std::hex << err << std::dec << "\n";
        return false;
    }
    return true;
}

int CameraManager::get_zoom_current_position(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_current_position();
}

int CameraManager::get_zoom_max_position(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_max_position();
}

int CameraManager::get_zoom_min_position(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_min_position();
}

int CameraManager::get_zoom_position_step(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_position_step();
}

int CameraManager::get_zoom_max_speed(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true)) return -1;
    return camera->get_zoom_max_speed();
}

int CameraManager::get_zoom_min_speed(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_min_speed();
}

bool CameraManager::set_zoom_speed(int no, int speed)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_zoom_speed(speed);
}

bool CameraManager::get_live_view(int no, py::buffer py_buf)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    camera->get_live_view(0, py_buf); // 0 for LiveViewOnly
    return true;
}

bool CameraManager::download_latest_files(int no, int slot, int file_num, int mode)
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

std::string CameraManager::get_connected_model() const
{
    return m_connected ? m_current_model : "";
}
// ----------------------------------------------------------------

bool CameraManager::findTarget(int no, CameraDevicePtr& camera, bool check_connected)
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
