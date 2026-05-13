#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>
#include "mchannel.hpp"

namespace py = pybind11;


void bind_rx_channel(py::module_& m) {
    auto rx = py::class_<mantis::go::mrx_channel, std::shared_ptr<mantis::go::mrx_channel>>(m, "mrx_channel");
    rx.def(
        "receive",
        [](mantis::go::mrx_channel& self, py::buffer b, mantis::go::mrx_metadata& meta) -> void {
            /* Request a buffer descriptor from Python (e.g. a NumPy array) */
            py::buffer_info info = b.request();

            /* info.size is total elements, info.itemsize is size of one T (e.g. 8 for complex float) */
            /* We use the buffer's internal count to determine how many samples to read */
            self.receive(static_cast<char*>(info.ptr), info.itemsize, info.size, meta);
        },
        py::arg("buffer"), py::arg("metadata"), "receive(buffer: buffer, metadata: mantis.mrx_metadata) -> None");
    bind_common_channel_methods(rx);
}