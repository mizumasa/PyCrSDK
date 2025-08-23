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
        .def("get_zoom_current_position", &CameraManager::get_zoom_current_position, py::arg("no"))
        .def("get_zoom_max_position", &CameraManager::get_zoom_max_position, py::arg("no"))
        .def("get_zoom_min_position", &CameraManager::get_zoom_min_position, py::arg("no"))
        .def("get_zoom_position_step", &CameraManager::get_zoom_position_step, py::arg("no"))
        .def("get_zoom_max_speed", &CameraManager::get_zoom_max_speed, py::arg("no"))
        .def("get_zoom_min_speed", &CameraManager::get_zoom_min_speed, py::arg("no"))
        .def("set_zoom_speed", &CameraManager::set_zoom_speed, py::arg("no"),py::arg("speed"))
        .def("get_live_view", &CameraManager::get_live_view, py::arg("no"), py::arg("py_buf"))
        .def("download_latest_files", &CameraManager::download_latest_files,
              py::arg("no"), py::arg("slot"), py::arg("file_num"), py::arg("mode"))
        .def("get_connected_model", &CameraManager::get_connected_model)
    ;
}
