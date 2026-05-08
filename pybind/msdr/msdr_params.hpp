#include "mantis/mantis.h"
#include <pybind11/pybind11.h>

void bind_msdr_params(py::module_& m) {
    py::class_<mantis::params::msdr_params> cl(
        m, "msdr_params", "struct that contains all the parameters needed to find, init, and configure an SDR.");

    cl.def(py::init<>())
        .def("get_find_args", &mantis::params::msdr_params::get_find_args)
        .def("get_config_args", &mantis::params::msdr_params::get_config_args);

#define PARAM(name, default_val) cl.def_readwrite(#name, &mantis::params::msdr_params::name);

    MSDR_SEARCH_PARAMS
    MSDR_CONFIG_PARAMS

#undef PARAM

    // 3. Add the static methods
    cl.def_static("compare", &mantis::params::msdr_params::compare);

    cl.def_static(
        "from_str",
        [](const std::string& args, const std::string& delimiter) {
            mantis::params::msdr_params p;
            auto err = mantis::params::msdr_params::from_str(args, p, delimiter);
            return py::make_tuple(err, p);
        },
        py::arg("args"), py::arg("delimiter") = ";");

    cl.def("__repr__", [](const mantis::params::msdr_params& p) {
        std::stringstream ss;
        ss << p;
        return ss.str();
    });
}