#pragma once
#include <string>
#include <tuple>
#include <vector>
#include "CRSDK/CameraRemote_SDK.h"
#include "CRSDK/CameraRemote_SDK.h"
#include "CameraDevice.h"
#include "Text.h"

class CameraManager
{
public:
    CameraManager();
    ~CameraManager();

    // ---------- Public API ----------
    bool sdk_init();
    void sdk_release();

    int enumerate_cameras();

    bool connect_camera(int no, int mode = 0); // mode: 0=Remote, 1=ContentsTransfer, 2=RemoteTransfer
    bool disconnect_camera(int no);

    bool capture_image(int no);
    bool execute_movie_rec(int no, bool down);

    int  get_aperture(int no);
    int  get_iso(int no);
    int  get_shutter_speed(int no);
    int  get_extended_shutter_speed(int no);

    void  print_aperture(int no);
    void  print_iso(int no);
    void  print_shutter_speed(int no);
    void  print_extended_shutter_speed(int no);

    bool  set_aperture(int no, int value);
    bool  set_iso(int no, int value);
    bool  set_shutter_speed(int no, int value);
    bool  set_extended_shutter_speed(int no, int value);
    bool  set_camera_eframing(int no, bool on);
    bool  execute_eframing(
        int no,
        int in_x, int in_y, int in_w, int in_h,
        int out_x, int out_y, int out_w, int out_h,
        int horizontal_denominator = 100,
        int vertical_denominator = 100,
        const std::string& eframing_type = "Single",
        int area_no = 1,
        int ptz_speed = -1
    );
    bool  update_eframing_area(
        int no,
        int area_no,
        const std::string& group,
        int dx,
        int dy,
        int dwidth,
        int dheight
    );

    int  get_zoom_current_position(int no);
    int  get_zoom_max_position(int no);
    int  get_zoom_min_position(int no);
    int  get_zoom_position_step(int no);
    int  get_zoom_max_speed(int no);
    int  get_zoom_min_speed(int no);
    bool set_zoom_speed(int no, int speed);
    bool zoom_start(int no, int speed);
    bool zoom_stop(int no);
    bool zoom_move_relative_int16(int no, int value);
    std::tuple<int, int, int> get_zoom_speed_range(int no);

    int  get_zoom_abs_position_current(int no);
    std::tuple<int, int, int> get_zoom_abs_position_range(int no);
    bool set_zoom_abs_position(int no, int position);
    bool cancel_zoom_abs_position(int no);
    int  get_zoom_driving_status(int no);

    int  get_zoom_distance_current(int no);
    int  get_zoom_distance_min(int no);
    int  get_zoom_distance_max(int no);
    int  get_zoom_distance_step(int no);

    int  get_lens_information_enable_status(int no);
    bool request_lens_information(int no);
    py::list get_lens_information(int no);
    int  get_focus_distance_in_meter(int no);
    int  get_focus_distance_in_feet(int no);
    int  get_focal_distance_unit(int no);
    int  get_focus_abs_position_current(int no);
    std::tuple<int, int, int> get_focus_abs_position_range(int no);
    bool set_focus_abs_position(int no, int position);
    bool cancel_focus_abs_position(int no);
    int  get_focus_driving_status(int no);
    bool focus_start(int no, int speed);
    bool focus_stop(int no);
    bool focus_move_relative_int16(int no, int value);
    std::tuple<int, int, int> get_focus_speed_range(int no);
    int  get_focus_operation_int16_enable_status(int no);
    int  get_follow_focus_position_current(int no);
    std::tuple<int, int, int> get_follow_focus_position_range(int no);
    bool set_follow_focus_position(int no, int position);
    bool request_zoom_and_focus_presets(int no);
    py::list get_zoom_and_focus_presets(int no);
    bool save_zoom_and_focus_preset(int no, int preset_no);
    bool load_zoom_and_focus_preset(int no, int preset_no);
    bool set_zoom_and_focus_preset_zoom_only(int no, int preset_no, bool enabled);

    py::dict get_device_property(int no, std::uint32_t prop_code);
    py::dict set_device_property(int no, std::uint32_t prop_code, std::int64_t value, int value_type = -1);
    
    bool get_live_view(int no, int selected_index, py::buffer py_buf);
    bool get_osd(int no, py::buffer py_buf);
    bool download_latest_files(int no, int slot, int file_num, int mode);
    std::string get_connected_model() const;
    // --------------------------------

private:
    bool m_sdk_inited  = false;
    bool m_connected   = false;

    // ★ SDK が返すハンドルを保持
    SCRSDK::CrDeviceHandle m_deviceHandle = 0;

    const SCRSDK::ICrCameraObjectInfo* m_current_info = nullptr;
    std::string  m_current_model;

    struct CamInfo
    {
        const SCRSDK::ICrCameraObjectInfo* info;
        std::string model_name;
    };
    std::vector<CamInfo> m_cam_list;
    SCRSDK::ICrEnumCameraObjectInfo* camera_list = nullptr;
    typedef std::shared_ptr<cli::CameraDevice> CameraDevicePtr;
    typedef std::vector<CameraDevicePtr> CameraDeviceList;
    CameraDeviceList cameraList;
    bool findTarget(int no, CameraDevicePtr& camera, bool check_connected);
};
