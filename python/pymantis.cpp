#include <pybind11/pybind11.h>
#include <mantis/mantis.h>
#include "pybind/classes/time_tag.hpp"
#include "pybind/errors/error_codes.hpp"
#include "pybind/errors/metadata_errors.hpp"
#include "pybind/msdr/msdr_params.hpp"
#include "pybind/mchannel/mtx_channel.hpp"
#include "pybind/mchannel/mrx_channel.hpp"
#include "pybind/device_manager/device_manager.hpp"
#include "pybind/classes/mtx_metadata.hpp"
#include "pybind/classes/async_mtx_metadata.hpp"
#include "pybind/classes/mrx_metadata.hpp"
#include "pybind/utilities/prints.hpp"
#include "pybind/utilities/general.hpp"

namespace py = pybind11;


PYBIND11_MODULE(mantis, m) {
    m.attr("__version__") = MANTIS_VERSION;
// namespaces
    py::module_ utils = m.def_submodule("utils", "mantis utility functions");
    bind_prints(utils);
    bind_general(utils);

    // classes
    bind_time_tag(m);
    bind_errors(m);
    bind_metadata_errors(m);
    bind_msdr_params(m);
    bind_tx_metadata(m);
    bind_async_tx_metadata(m);
    bind_rx_metadata(m);
    bind_tx_channel(m);
    bind_rx_channel(m);
    bind_device_manager(m);
}