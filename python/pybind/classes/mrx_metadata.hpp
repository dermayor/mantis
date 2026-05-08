#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_rx_metadata(py::module_& m) {
    auto py_mrx_metadata = py::class_<mantis::go::mrx_metadata>(m, "mrx_metadata");
    py_mrx_metadata
        .def(py::init<>())
        // Basic fields
        .def_readwrite("timeout", &mantis::go::mrx_metadata::timeout)
        .def_readwrite("start_of_burst", &mantis::go::mrx_metadata::start_of_burst)
        .def_readwrite("end_of_burst", &mantis::go::mrx_metadata::end_of_burst)
        .def_readwrite("more_fragments", &mantis::go::mrx_metadata::more_fragments)
        .def_readwrite("out_of_sequence", &mantis::go::mrx_metadata::out_of_sequence)
        .def_readwrite("one_packet", &mantis::go::mrx_metadata::one_packet)
        .def_readwrite("fragment_offset", &mantis::go::mrx_metadata::fragment_offset)

        // Error Code (using the enum we bound earlier)
        .def_readwrite("code", &mantis::go::mrx_metadata::code)

        // Time handling
        .def_readwrite("time_spec", &mantis::go::mrx_metadata::time_spec)
        .def_readwrite("has_time_spec", &mantis::go::mrx_metadata::has_time_spec)
        .def("set_time_spec", &mantis::go::mrx_metadata::set_time_spec, py::arg("time_spec"),
             "Sets the desired RX start time.")

        // EOV Positions (exposed as a list for Python safety)

        .def(
            "set_eov_positions",
            [](mantis::go::mrx_metadata& self, std::vector<size_t> values) {
                static std::vector<size_t> storage;
                storage = std::move(values);
                self.eov_positions = storage.data();
                self.eov_positions_size = storage.size();
                self.eov_positions_count = storage.size();
            },
            py::arg("positions"), "Set EOV positions from a Python list.")
        .def(
            "get_eov_positions",
            [](const mantis::go::mrx_metadata& self) {
                std::vector<size_t> vec;
                if (self.eov_positions && self.eov_positions_count > 0) {
                    vec.assign(self.eov_positions, self.eov_positions + self.eov_positions_count);
                }
                return vec;
            },
            "Get EOV positions as a Python list.");
}