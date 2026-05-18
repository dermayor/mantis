#pragma once 

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

// Template helper to bind common i_channel methods
template <typename T, typename Holder>
void bind_common_channel_methods(py::class_<mantis::interfaces::i_channel<T>, Holder>& cl) {
    cl.def("healthcheck", &mantis::interfaces::i_channel<T>::healthcheck)
        .def("sync", &mantis::interfaces::i_channel<T>::sync, py::arg("time_stamp"))
        .def("get_time_now", &mantis::interfaces::i_channel<T>::get_time_now)
        .def("get_channel_num", &mantis::interfaces::i_channel<T>::get_channel_num)
        .def("is_valid", &mantis::interfaces::i_channel<T>::is_valid)
        .def("invalidate", &mantis::interfaces::i_channel<T>::invalidate)
        .def("set_rate", &mantis::interfaces::i_channel<T>::set_rate)
        .def("get_rate", &mantis::interfaces::i_channel<T>::get_rate)
        .def("set_freq", &mantis::interfaces::i_channel<T>::set_freq, py::arg("freq"), py::arg("lo") = py::none())
        .def("get_freq", &mantis::interfaces::i_channel<T>::get_freq)
        .def("set_gain", &mantis::interfaces::i_channel<T>::set_gain)
        .def("get_gain", &mantis::interfaces::i_channel<T>::get_gain)
        .def("get_valid_sample_rates", &mantis::interfaces::i_channel<T>::get_valid_sample_rates)
        .def("set_clock_source", &mantis::interfaces::i_channel<T>::set_clock_source)
        .def("set_time_source", &mantis::interfaces::i_channel<T>::set_time_source)
        .def("can_set_power_ref", &mantis::interfaces::i_channel<T>::can_set_power_ref)
        .def("set_power_ref", &mantis::interfaces::i_channel<T>::set_power_ref)
        .def("get_power_ref", &mantis::interfaces::i_channel<T>::get_power_ref)
        .def("get_params", &mantis::interfaces::i_channel<T>::get_params, py::return_value_policy::reference_internal);
}