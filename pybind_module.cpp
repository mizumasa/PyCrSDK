#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PyCrSDK.hpp"

// Pythonバインディング用
namespace py = pybind11;

PYBIND11_MODULE(pycrsdk, m) {
    m.doc() = "Minimal Sony Camera Remote SDK Python binding example";

    py::class_<PyCrSDK>(m, "PyCrSDK")
        .def(py::init<>())
        .def("sdk_init", &PyCrSDK::sdk_init)
        .def("sdk_release", &PyCrSDK::sdk_release)
        .def("enumerate_cameras", &PyCrSDK::enumerate_cameras)
        .def("connect_camera", &PyCrSDK::connect_camera)
        .def("disconnect_camera", &PyCrSDK::disconnect_camera)
        .def("capture_image", &PyCrSDK::capture_image)
        .def("execute_movie_rec", &PyCrSDK::execute_movie_rec)
        .def("get_aperture", &PyCrSDK::get_aperture, py::arg("no"))
        .def("get_iso", &PyCrSDK::get_iso, py::arg("no"))
        .def("get_shutter_speed", &PyCrSDK::get_shutter_speed, py::arg("no"))
        .def("get_extended_shutter_speed", &PyCrSDK::get_extended_shutter_speed, py::arg("no"))
        .def("print_aperture", &PyCrSDK::print_aperture, py::arg("no"))
        .def("print_iso", &PyCrSDK::print_iso, py::arg("no"))
        .def("print_shutter_speed", &PyCrSDK::print_shutter_speed, py::arg("no"))
        .def("print_extended_shutter_speed", &PyCrSDK::print_extended_shutter_speed, py::arg("no"))
        .def("set_aperture", &PyCrSDK::set_aperture, py::arg("no"),py::arg("value"))
        .def("set_iso", &PyCrSDK::set_iso, py::arg("no"),py::arg("value"))
        .def("set_shutter_speed", &PyCrSDK::set_shutter_speed, py::arg("no"),py::arg("value"))
        .def("set_extended_shutter_speed", &PyCrSDK::set_extended_shutter_speed, py::arg("no"),py::arg("value"))
        .def("get_zoom_current_position", &PyCrSDK::get_zoom_current_position, py::arg("no"))
        .def("get_zoom_max_position", &PyCrSDK::get_zoom_max_position, py::arg("no"))
        .def("get_zoom_min_position", &PyCrSDK::get_zoom_min_position, py::arg("no"))
        .def("get_zoom_position_step", &PyCrSDK::get_zoom_position_step, py::arg("no"))
        .def("get_zoom_max_speed", &PyCrSDK::get_zoom_max_speed, py::arg("no"))
        .def("get_zoom_min_speed", &PyCrSDK::get_zoom_min_speed, py::arg("no"))
        .def("set_zoom_speed", &PyCrSDK::set_zoom_speed, py::arg("no"),py::arg("speed"))
        .def("get_live_view", &PyCrSDK::get_live_view, py::arg("no"), py::arg("py_buf"))
        .def("download_latest_files", &PyCrSDK::download_latest_files,
              py::arg("no"), py::arg("slot"), py::arg("file_num"), py::arg("mode"))
        .def("get_connected_model", &PyCrSDK::get_connected_model)
    ;
}
