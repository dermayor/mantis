#pragma once

#include "mantis/mantis.h"
#include <pybind11/pybind11.h>
#include "mchannel.hpp"

namespace py = pybind11;


void bind_tx_channel(py::module_& m) {
// 2. TX Channel Binding
    auto tx = py::class_<mantis::go::mtx_channel, std::shared_ptr<mantis::go::mtx_channel>>(m, "mtx_channel");
    tx.def(
          "send",
          [](mantis::go::mtx_channel& self, py::buffer b, mantis::go::mtx_metadata& meta) -> void {
              py::buffer_info info = b.request();
              self.send(static_cast<const char*>(info.ptr), info.itemsize, info.size, meta);
          },
          py::arg("buffer"), py::arg("metadata"), "send(buffer: buffer, metadata: mantis.mtx_metadata) -> None")

        .def(
            "recv_async_message",
            [](mantis::go::mtx_channel& self, mantis::go::async_mtx_metadata& msg, double timeout) {
                self.recv_async_message(msg, timeout);
            },
            py::arg("message"), py::arg("timeout") = 0.1,
            "recv_async_message(message: mantis.async_mtx_metadata, timeout: float = 0.1) -> None");
    bind_common_channel_methods(tx);
}