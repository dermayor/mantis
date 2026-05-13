#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>

void bind_metadata_errors(py::module_& m) {
    // Create a dedicated submodule: mantis.metadata_errors
    auto m_err = m.def_submodule("metadata_errors", "Mantis hardware metadata error codes and utilities");

    // 1. Bind async_mtx_metadata_error_codes (Bitflags)
    py::enum_<mantis::metadata_errors::async_mtx_metadata_error_codes>(m_err, "async_mtx_metadata_error_codes", py::arithmetic())
        .value("BURST_ACK",           mantis::metadata_errors::async_mtx_metadata_error_codes::BURST_ACK)
        .value("UNDERFLOW",           mantis::metadata_errors::async_mtx_metadata_error_codes::UNDERFLOW)
        .value("SEQ_ERROR",           mantis::metadata_errors::async_mtx_metadata_error_codes::SEQ_ERROR)
        .value("LATE",                mantis::metadata_errors::async_mtx_metadata_error_codes::LATE)
        .value("UNDERFLOW_IN_PACKET", mantis::metadata_errors::async_mtx_metadata_error_codes::UNDERFLOW_IN_PACKET)
        .value("SEQ_ERROR_IN_BURST",  mantis::metadata_errors::async_mtx_metadata_error_codes::SEQ_ERROR_IN_BURST)
        .value("USER_PAYLOAD",        mantis::metadata_errors::async_mtx_metadata_error_codes::USER_PAYLOAD);

    // 2. Bind mrx_metadata_error_codes
    py::enum_<mantis::metadata_errors::mrx_metadata_error_codes>(m_err, "mrx_metadata_error_codes", py::arithmetic())
        .value("NO_ERROR",     mantis::metadata_errors::mrx_metadata_error_codes::NO_ERROR)
        .value("TIMEOUT",      mantis::metadata_errors::mrx_metadata_error_codes::TIMEOUT)
        .value("LATE",         mantis::metadata_errors::mrx_metadata_error_codes::LATE)
        .value("BROKEN_CHAIN", mantis::metadata_errors::mrx_metadata_error_codes::BROKEN_CHAIN)
        .value("OVERFLOW",     mantis::metadata_errors::mrx_metadata_error_codes::OVERFLOW)
        .value("ALIGNMENT",    mantis::metadata_errors::mrx_metadata_error_codes::ALIGNMENT)
        .value("BAD_PACKET",   mantis::metadata_errors::mrx_metadata_error_codes::BAD_PACKET);

    // 3. Bind Overloaded mantis_errno functions
    // We use py::overload_cast to distinguish between the two versions
    m_err.def("mantis_errno", 
        py::overload_cast<mantis::metadata_errors::async_mtx_metadata_error_codes&>(&mantis::metadata_errors::mantis_errno),
        "Get detailed error message for an async_mtx_metadata_error_code.");

    m_err.def("mantis_errno", 
        py::overload_cast<mantis::metadata_errors::mrx_metadata_error_codes&>(&mantis::metadata_errors::mantis_errno),
        "Get detailed error message for an mrx_metadata_error_code.");
}