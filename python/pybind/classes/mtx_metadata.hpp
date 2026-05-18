#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_tx_metadata(py::module_& m) {

    // 1. Bind mtx_metadata
    py::class_<mantis::go::mtx_metadata>(m, "mtx_metadata")
        .def(py::init<>())
        .def_readwrite("timeout", &mantis::go::mtx_metadata::timeout)
        .def_readwrite("start_of_burst", &mantis::go::mtx_metadata::start_of_burst)
        .def_readwrite("end_of_burst", &mantis::go::mtx_metadata::end_of_burst)

        // We expose has_time_spec as readonly since the header says use set_time_spec
        .def_readwrite("has_time_spec", &mantis::go::mtx_metadata::has_time_spec)
        .def_readwrite("time_spec", &mantis::go::mtx_metadata::time_spec)

        .def("set_time_spec", &mantis::go::mtx_metadata::set_time_spec, py::arg("time_spec"),
             "Sets the transmission time and updates the has_time_spec flag.")

        // I honestly have no idea what this does at this time :)!
        // Handling the raw pointer eov_positions for Python
        // It's safer to expose this as a list/vector getter/setter
        .def_property(
            "eov_positions",
            [](const mantis::go::mtx_metadata& self) {
                std::vector<size_t> vec;
                if (self.eov_positions && self.eov_positions_size > 0) {
                    vec.assign(self.eov_positions, self.eov_positions + self.eov_positions_size);
                }
                return vec;
            },
            [](mantis::go::mtx_metadata& self, std::vector<size_t> values) {
                static std::vector<size_t> storage;
                storage = std::move(values);
                self.eov_positions = storage.data();
                self.eov_positions_size = storage.size();
            });
}