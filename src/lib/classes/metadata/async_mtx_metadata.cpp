//
// Created by Jon Kler on 6/8/25.
//

#include <mantis/lib/classes/metadata/async_mtx_metadata.h>

#ifdef M_UHD

uhd::async_metadata_t mantis::go::async_mtx_metadata::from_async_mtx_md(async_mtx_metadata &a_mtx_md) {
    uhd::async_metadata_t to_return{};

    to_return.channel = a_mtx_md.channel;
    to_return.has_time_spec = a_mtx_md.has_time_spec;
    to_return.time_spec = {a_mtx_md.time_spec.get_full_secs(), a_mtx_md.time_spec.get_frac_secs()};
    to_return.event_code = static_cast<uhd::async_metadata_t::event_code_t>(a_mtx_md.code);

    return to_return;
}

mantis::go::async_mtx_metadata mantis::go::async_mtx_metadata::to_async_mtx_md(uhd::async_metadata_t &a_mtx_md) {
    async_mtx_metadata to_return{};

    to_return.channel = a_mtx_md.channel;
    to_return.time_spec = {a_mtx_md.time_spec.get_full_secs(), a_mtx_md.time_spec.get_frac_secs()};
    to_return.has_time_spec = a_mtx_md.has_time_spec;
    to_return.code = static_cast<metadata_errors::async_mtx_metadata_error_codes>(a_mtx_md.event_code);

    return to_return;
}

#endif
