#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>

void bind_prints(py::module_& m) {
    m.def("pinfo", &mantis::utils::pinfo, "Print an info message with mantis formatting");
    m.def("perror", &mantis::utils::perror, "Print an error message with mantis formatting");
    m.def("pwarn", &mantis::utils::pwarn, "Print a warning message with mantis formatting");
}