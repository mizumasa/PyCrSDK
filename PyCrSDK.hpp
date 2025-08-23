#pragma once
#include <string>
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

    int  get_zoom_current_position(int no);
    int  get_zoom_max_position(int no);
    int  get_zoom_min_position(int no);
    int  get_zoom_position_step(int no);
    int  get_zoom_max_speed(int no);
    int  get_zoom_min_speed(int no);
    bool set_zoom_speed(int no, int speed);
    
    bool get_live_view(int no, py::buffer py_buf);
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
