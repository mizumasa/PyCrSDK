#pragma once
#include <string>
#include <vector>
#include "CRSDK/CameraRemote_SDK.h"
#include "CRSDK/CameraRemote_SDK.h"
#include "CameraDevice.h"
#include "Text.h"

class PyCrSDK
{
public:
    PyCrSDK();
    ~PyCrSDK();

    // ---------- Public API ----------
    bool sdk_init();
    void sdk_release();

    int enumerate_cameras();

    bool connect_camera(int no);
    bool disconnect_camera(int no);

    bool capture_image(int no);
    int get_iso(int no);
    bool get_live_view(int no, py::buffer py_buf);

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
