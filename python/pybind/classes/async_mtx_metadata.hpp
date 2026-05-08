#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_async_tx_metadata(py::module_& m) {
    py::class_<mantis::go::async_mtx_metadata>(m, "async_mtx_metadata")
        .def(py::init<>())
        .def_readwrite("channel", &mantis::go::async_mtx_metadata::channel)
        .def_readwrite("has_time_spec", &mantis::go::async_mtx_metadata::has_time_spec)
        .def_readwrite("time_spec", &mantis::go::async_mtx_metadata::time_spec)
        .def_readwrite("code", &mantis::go::async_mtx_metadata::code);
}