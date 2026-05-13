#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bind_device_manager(py::module_& m) {
    py::class_<mantis::device_manager, std::unique_ptr<mantis::device_manager, py::nodelete>>(m, "device_manager")
        // 1. Singleton access
        .def_static("get_instance", &mantis::device_manager::get_instance, py::return_value_policy::reference,
                    "Access the global singleton instance of the device manager.")

        // 2. Initialization methods
        .def("init", &mantis::device_manager::init, py::arg("num"), py::arg("msdr_params"),
             "Connect and initialize a requested number of SDRs.")

        .def("init_all", &mantis::device_manager::init_all, py::arg("msdr_params"),
             "Initialize all supported SDRs connected to the system.")

        // 3. Channel Acquisition (Returns a pair/tuple in Python)
        .def("get_tx_channel", &mantis::device_manager::get_tx_channel, py::arg("msdr_params"), py::arg("channel_num"),
             "Returns (error_code, mtx_channel) for the requested SDR and channel.")

        .def("get_rx_channel", &mantis::device_manager::get_rx_channel, py::arg("msdr_params"), py::arg("channel_num"),
             "Returns (error_code, mrx_channel) for the requested SDR and channel.")

        // 4. Discovery
        .def("find", &mantis::device_manager::find, py::arg("params"),
             "Returns a list of devices found on the machine.")

        .def("get_connected", &mantis::device_manager::get_connected,
             "Returns a list of params of devices currently being managed by mantis.")

        // 5. String Representation
        .def("__repr__", [](const mantis::device_manager& dm) {
            std::stringstream ss;
            ss << dm;
            return ss.str();
        });
}