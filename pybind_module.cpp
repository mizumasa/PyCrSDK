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
        .def("get_live_view", &CameraManager::get_live_view, py::arg("no"), py::arg("py_buf"))
        .def("download_latest_files", &CameraManager::download_latest_files,
              py::arg("no"), py::arg("slot"), py::arg("file_num"), py::arg("mode"))
        .def("get_connected_model", &CameraManager::get_connected_model)
    ;
}
