#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_errors(py::module_& m) {
    py::class_<mantis::errors> err(m, "errors", "Mantis Errors Class, Contains error codes and errno function");

    py::enum_<mantis::errors::error_code>(err, "error_code", "mantis error codes")
        .value("SUCCESS", mantis::errors::error_code::SUCCESS)
        .value("NO_SDRS_CONNECTED", mantis::errors::error_code::NO_SDRS_CONNECTED)
        .value("NO_SDRS_FOUND", mantis::errors::error_code::NO_SDRS_FOUND)
        .value("INSUFFICIENT_SDRS_FOUND", mantis::errors::error_code::INSUFFICIENT_SDRS_FOUND)
        .value("NO_MATCHING_SDR", mantis::errors::error_code::NO_MATCHING_SDR)
        .value("INVALID_SDR", mantis::errors::error_code::INVALID_SDR)
        .value("INVALID_MCR", mantis::errors::error_code::INVALID_MCR)
        .value("INVALID_ARGUMENT", mantis::errors::error_code::INVALID_ARGUMENT)
        .value("NO_TX_CHANNELS", mantis::errors::error_code::NO_TX_CHANNELS)
        .value("NO_RX_CHANNELS", mantis::errors::error_code::NO_RX_CHANNELS)
        .value("INSUFFICIENT_TX_CHANNELS", mantis::errors::error_code::INSUFFICIENT_TX_CHANNELS)
        .value("INSUFFICIENT_RX_CHANNELS", mantis::errors::error_code::INSUFFICIENT_RX_CHANNELS)
        .value("POWER_REF_UNAVAILABLE", mantis::errors::error_code::POWER_REF_UNAVAILABLE)
        .value("SOCKET_CONECTION_FAILURE", mantis::errors::error_code::SOCKET_CONECTION_FAILURE)
        .value("TOTAL_SOCKET_SEND_FAILURE", mantis::errors::error_code::TOTAL_SOCKET_SEND_FAILURE)
        .value("TOTAL_SOCKET_RECV_FAILURE", mantis::errors::error_code::TOTAL_SOCKET_RECV_FAILURE)
        .value("PARTIAL_SOCKET_SEND_FAILURE", mantis::errors::error_code::PARTIAL_SOCKET_SEND_FAILURE)
        .value("PARTIAL_SOCKET_RECV_FAILURE", mantis::errors::error_code::PARTIAL_SOCKET_RECV_FAILURE)
        .value("FAILED_TO_ACCEPT_CLIENT", mantis::errors::error_code::FAILED_TO_ACCEPT_CLIENT)
        .value("FAILED_TO_GET_SOCKET_FD", mantis::errors::error_code::FAILED_TO_GET_SOCKET_FD)
        .value("FAILED_TO_BIND", mantis::errors::error_code::FAILED_TO_BIND)
        .value("FAILED_TO_LISTEN", mantis::errors::error_code::FAILED_TO_LISTEN)
        .value("CHANNEL_IN_USE", mantis::errors::error_code::CHANNEL_IN_USE)
        .value("TIMEOUT", mantis::errors::error_code::TIMEOUT)
        .value("INVALID_TIME_SOURCE", mantis::errors::error_code::INVALID_TIME_SOURCE);

        err.def_static("mantis_errno", &mantis::errors::mantis_errno);
}
