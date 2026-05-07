#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PyCrSDK.hpp"

// Pythonバインディング用
namespace py = pybind11;

PYBIND11_MODULE(pycrsdk, m) {
    m.doc() = "Minimal Sony Camera Remote SDK Python binding example";

    py::class_<CameraManager>(m, "CameraManager")
        .def(py::init<>())
        .def("sdk_init", &CameraManager::sdk_init)
        .def("sdk_release", &CameraManager::sdk_release)
        .def("enumerate_cameras", &CameraManager::enumerate_cameras)
        .def("connect_camera", &CameraManager::connect_camera)
        .def("disconnect_camera", &CameraManager::disconnect_camera)
        .def("capture_image", &CameraManager::capture_image)
        .def("execute_movie_rec", &CameraManager::execute_movie_rec)
        .def("get_aperture", &CameraManager::get_aperture, py::arg("no"))
        .def("get_iso", &CameraManager::get_iso, py::arg("no"))
        .def("get_shutter_speed", &CameraManager::get_shutter_speed, py::arg("no"))
        .def("get_extended_shutter_speed", &CameraManager::get_extended_shutter_speed, py::arg("no"))
        .def("print_aperture", &CameraManager::print_aperture, py::arg("no"))
        .def("print_iso", &CameraManager::print_iso, py::arg("no"))
        .def("print_shutter_speed", &CameraManager::print_shutter_speed, py::arg("no"))
        .def("print_extended_shutter_speed", &CameraManager::print_extended_shutter_speed, py::arg("no"))
        .def("set_aperture", &CameraManager::set_aperture, py::arg("no"),py::arg("value"))
        .def("set_iso", &CameraManager::set_iso, py::arg("no"),py::arg("value"))
        .def("set_shutter_speed", &CameraManager::set_shutter_speed, py::arg("no"),py::arg("value"))
        .def("set_extended_shutter_speed", &CameraManager::set_extended_shutter_speed, py::arg("no"),py::arg("value"))
        .def("set_camera_eframing", &CameraManager::set_camera_eframing, py::arg("no"), py::arg("on"))
        .def("execute_eframing", &CameraManager::execute_eframing,
            py::arg("no"),
            py::arg("in_x"), py::arg("in_y"), py::arg("in_w"), py::arg("in_h"),
            py::arg("out_x"), py::arg("out_y"), py::arg("out_w"), py::arg("out_h"),
            py::arg("horizontal_denominator") = 100,
            py::arg("vertical_denominator") = 100,
            py::arg("eframing_type") = "Single",
            py::arg("area_no") = 1,
            py::arg("ptz_speed") = -1)
        .def("update_eframing_area", &CameraManager::update_eframing_area,
            py::arg("no"),
            py::arg("area_no"),
            py::arg("group"),
            py::arg("dx"), py::arg("dy"), py::arg("dwidth"), py::arg("dheight"))
        .def("get_zoom_current_position", &CameraManager::get_zoom_current_position, py::arg("no"))
        .def("get_zoom_max_position", &CameraManager::get_zoom_max_position, py::arg("no"))
        .def("get_zoom_min_position", &CameraManager::get_zoom_min_position, py::arg("no"))
        .def("get_zoom_position_step", &CameraManager::get_zoom_position_step, py::arg("no"))
        .def("get_zoom_max_speed", &CameraManager::get_zoom_max_speed, py::arg("no"))
        .def("get_zoom_min_speed", &CameraManager::get_zoom_min_speed, py::arg("no"))
        .def("set_zoom_speed", &CameraManager::set_zoom_speed, py::arg("no"),py::arg("speed"))
        .def("zoom_start", &CameraManager::zoom_start, py::arg("no"), py::arg("speed"))
        .def("zoom_stop", &CameraManager::zoom_stop, py::arg("no"))
        .def("zoom_move_relative_int16", &CameraManager::zoom_move_relative_int16, py::arg("no"), py::arg("value"))
        .def("get_zoom_speed_range", &CameraManager::get_zoom_speed_range, py::arg("no"))
        .def("get_zoom_abs_position_current", &CameraManager::get_zoom_abs_position_current, py::arg("no"))
        .def("get_zoom_abs_position_range", &CameraManager::get_zoom_abs_position_range, py::arg("no"))
        .def("set_zoom_abs_position", &CameraManager::set_zoom_abs_position, py::arg("no"), py::arg("position"))
        .def("cancel_zoom_abs_position", &CameraManager::cancel_zoom_abs_position, py::arg("no"))
        .def("get_zoom_driving_status", &CameraManager::get_zoom_driving_status, py::arg("no"))
        .def("get_zoom_distance_current", &CameraManager::get_zoom_distance_current, py::arg("no"))
        .def("get_zoom_distance_min", &CameraManager::get_zoom_distance_min, py::arg("no"))
        .def("get_zoom_distance_max", &CameraManager::get_zoom_distance_max, py::arg("no"))
        .def("get_zoom_distance_step", &CameraManager::get_zoom_distance_step, py::arg("no"))
          .def("get_lens_information_enable_status", &CameraManager::get_lens_information_enable_status, py::arg("no"))
          .def("request_lens_information", &CameraManager::request_lens_information, py::arg("no"))
          .def("get_lens_information", &CameraManager::get_lens_information, py::arg("no"))
          .def("get_focus_distance_in_meter", &CameraManager::get_focus_distance_in_meter, py::arg("no"))
          .def("get_focus_distance_in_feet", &CameraManager::get_focus_distance_in_feet, py::arg("no"))
          .def("get_focal_distance_unit", &CameraManager::get_focal_distance_unit, py::arg("no"))
          .def("get_focus_abs_position_current", &CameraManager::get_focus_abs_position_current, py::arg("no"))
          .def("get_focus_abs_position_range", &CameraManager::get_focus_abs_position_range, py::arg("no"))
          .def("set_focus_abs_position", &CameraManager::set_focus_abs_position, py::arg("no"), py::arg("position"))
          .def("cancel_focus_abs_position", &CameraManager::cancel_focus_abs_position, py::arg("no"))
          .def("get_focus_driving_status", &CameraManager::get_focus_driving_status, py::arg("no"))
          .def("focus_start", &CameraManager::focus_start, py::arg("no"), py::arg("speed"))
          .def("focus_stop", &CameraManager::focus_stop, py::arg("no"))
          .def("focus_move_relative_int16", &CameraManager::focus_move_relative_int16, py::arg("no"), py::arg("value"))
          .def("get_focus_speed_range", &CameraManager::get_focus_speed_range, py::arg("no"))
          .def("get_focus_operation_int16_enable_status", &CameraManager::get_focus_operation_int16_enable_status, py::arg("no"))
          .def("get_follow_focus_position_current", &CameraManager::get_follow_focus_position_current, py::arg("no"))
          .def("get_follow_focus_position_range", &CameraManager::get_follow_focus_position_range, py::arg("no"))
          .def("set_follow_focus_position", &CameraManager::set_follow_focus_position, py::arg("no"), py::arg("position"))
          .def("request_zoom_and_focus_presets", &CameraManager::request_zoom_and_focus_presets, py::arg("no"))
          .def("get_zoom_and_focus_presets", &CameraManager::get_zoom_and_focus_presets, py::arg("no"))
          .def("save_zoom_and_focus_preset", &CameraManager::save_zoom_and_focus_preset, py::arg("no"), py::arg("preset_no"))
          .def("load_zoom_and_focus_preset", &CameraManager::load_zoom_and_focus_preset, py::arg("no"), py::arg("preset_no"))
          .def("set_zoom_and_focus_preset_zoom_only", &CameraManager::set_zoom_and_focus_preset_zoom_only, py::arg("no"), py::arg("preset_no"), py::arg("enabled"))
        .def("get_live_view", &CameraManager::get_live_view, py::arg("no"), py::arg("py_buf"))
        .def("download_latest_files", &CameraManager::download_latest_files,
              py::arg("no"), py::arg("slot"), py::arg("file_num"), py::arg("mode"))
        .def("get_connected_model", &CameraManager::get_connected_model)
    ;
}
