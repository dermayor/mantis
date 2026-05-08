#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>

void bind_general(py::module_& m) {
    m.def("get_sample_size", &mantis::utils::get_sample_size,
          "Get the current sample size in bytes according to the CPU_SAMPLE_FORMAT config variable. Will throw a "
          "runtime error if the CPU_SAMPLE_FORMAT is not supported.");
}