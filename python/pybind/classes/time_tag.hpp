#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

void bind_time_tag(py::module_& m) {
    py::class_<mantis::time_tag>(m, "time_tag", "Mantis time_tag class. class is split into full seconds and fractional for increased accuracy")
        .def(py::init<>())
        .def(py::init<int64_t, double>(), py::arg("full_secs"), py::arg("frac_secs"))
        .def(py::init<int64_t>(), py::arg("full_secs"))
        .def_property_readonly("full_secs", &mantis::time_tag::get_full_secs)
        .def_property_readonly("frac_secs", &mantis::time_tag::get_frac_secs)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self += py::self)
        .def(py::self -= py::self);
}
