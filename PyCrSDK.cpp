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
    return get_zoom_distance_current(no);
}

int CameraManager::get_zoom_max_position(int no)
{
    return get_zoom_distance_max(no);
}

int CameraManager::get_zoom_min_position(int no)
{
    return get_zoom_distance_min(no);
}

int CameraManager::get_zoom_position_step(int no)
{
    return get_zoom_distance_step(no);
}

int CameraManager::get_zoom_distance_current(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_current_position();
}

int CameraManager::get_zoom_distance_max(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_max_position();
}

int CameraManager::get_zoom_distance_min(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_min_position();
}

int CameraManager::get_zoom_distance_step(int no)
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

bool CameraManager::zoom_start(int no, int speed)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->zoom_start(speed);
}

bool CameraManager::zoom_stop(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->zoom_stop();
}

bool CameraManager::zoom_move_relative_int16(int no, int value)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->zoom_move_relative_int16(value);
}

std::tuple<int, int, int> CameraManager::get_zoom_speed_range(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return {-1, -1, -1};

    int min_speed = -1;
    int max_speed = 1;
    int step = 1;
    camera->get_zoom_speed_range(min_speed, max_speed, step);
    return {min_speed, max_speed, step};
}

int CameraManager::get_zoom_abs_position_current(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_abs_position_current();
}

std::tuple<int, int, int> CameraManager::get_zoom_abs_position_range(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return {-1, -1, -1};

    int min_pos = -1;
    int max_pos = -1;
    int step = -1;
    if (!camera->get_zoom_abs_position_range(min_pos, max_pos, step)) {
        return {-1, -1, -1};
    }
    return {min_pos, max_pos, step};
}

bool CameraManager::set_zoom_abs_position(int no, int position)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_zoom_abs_position(position);
}

bool CameraManager::cancel_zoom_abs_position(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->cancel_zoom_abs_position();
}

int CameraManager::get_zoom_driving_status(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_zoom_driving_status();
}

int CameraManager::get_lens_information_enable_status(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_lens_information_enable_status();
}

bool CameraManager::request_lens_information(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->request_lens_information();
}

py::list CameraManager::get_lens_information(int no)
{
    py::list result;

    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return result;

    auto lens_info = camera->get_lens_information();
    for (const auto& entry : lens_info) {
        py::dict item;
        item["type"] = std::get<0>(entry);
        item["data_version"] = std::get<1>(entry);
        item["normalized_value"] = std::get<2>(entry);
        item["focus_position"] = std::get<3>(entry);
        result.append(item);
    }
    return result;
}

int CameraManager::get_focus_distance_in_meter(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_focus_distance_in_meter();
}

int CameraManager::get_focus_distance_in_feet(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_focus_distance_in_feet();
}

int CameraManager::get_focal_distance_unit(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_focal_distance_unit();
}

int CameraManager::get_focus_abs_position_current(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_focus_abs_position_current();
}

std::tuple<int, int, int> CameraManager::get_focus_abs_position_range(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return {-1, -1, -1};

    int min_pos = -1;
    int max_pos = -1;
    int step = -1;
    if (!camera->get_focus_abs_position_range(min_pos, max_pos, step)) {
        return {-1, -1, -1};
    }
    return {min_pos, max_pos, step};
}

bool CameraManager::set_focus_abs_position(int no, int position)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_focus_abs_position(position);
}

bool CameraManager::cancel_focus_abs_position(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->cancel_focus_abs_position();
}

int CameraManager::get_focus_driving_status(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_focus_driving_status();
}

bool CameraManager::focus_start(int no, int speed)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->focus_start(speed);
}

bool CameraManager::focus_stop(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->focus_stop();
}

bool CameraManager::focus_move_relative_int16(int no, int value)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->focus_move_relative_int16(value);
}

std::tuple<int, int, int> CameraManager::get_focus_speed_range(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return {-1, -1, -1};

    int min_speed = -1;
    int max_speed = -1;
    int step = -1;
    if (!camera->get_focus_speed_range(min_speed, max_speed, step)) {
        return {-1, -1, -1};
    }
    return {min_speed, max_speed, step};
}

int CameraManager::get_focus_operation_int16_enable_status(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_focus_operation_int16_enable_status();
}

int CameraManager::get_follow_focus_position_current(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return -1;
    return camera->get_follow_focus_position_current();
}

std::tuple<int, int, int> CameraManager::get_follow_focus_position_range(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return {-1, -1, -1};

    int min_pos = -1;
    int max_pos = -1;
    int step = -1;
    if (!camera->get_follow_focus_position_range(min_pos, max_pos, step)) {
        return {-1, -1, -1};
    }
    return {min_pos, max_pos, step};
}

bool CameraManager::set_follow_focus_position(int no, int position)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_follow_focus_position(position);
}

bool CameraManager::request_zoom_and_focus_presets(int no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->request_zoom_and_focus_presets();
}

py::list CameraManager::get_zoom_and_focus_presets(int no)
{
    py::list result;

    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return result;

    auto presets = camera->get_zoom_and_focus_presets();
    for (const auto& entry : presets) {
        py::dict item;
        item["preset_no"] = std::get<0>(entry);
        item["is_exists"] = std::get<1>(entry);
        item["lens_model_name"] = std::get<2>(entry);
        item["zoom_distance"] = std::get<3>(entry);
        item["focal_distance"] = std::get<4>(entry);
        item["zoom_only_enable_status"] = std::get<5>(entry);
        item["zoom_only_value"] = std::get<6>(entry);
        result.append(item);
    }
    return result;
}

bool CameraManager::save_zoom_and_focus_preset(int no, int preset_no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->save_zoom_and_focus_preset(preset_no);
}

bool CameraManager::load_zoom_and_focus_preset(int no, int preset_no)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->load_zoom_and_focus_preset(preset_no);
}

bool CameraManager::set_zoom_and_focus_preset_zoom_only(int no, int preset_no, bool enabled)
{
    CameraDevicePtr camera = nullptr;
    if(!findTarget(no,camera,true))return false;
    return camera->set_zoom_and_focus_preset_zoom_only(preset_no, enabled);
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
