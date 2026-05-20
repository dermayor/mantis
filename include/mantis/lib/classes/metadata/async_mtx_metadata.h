//
// Created by Jon Kler on 6/8/25.
//

#pragma once

#include <mantis/lib/errors/async_mtx_metadata_errors.h>
#include <mantis/lib/classes/time_tag.h>

#ifdef M_UHD

#include <uhd/usrp/multi_usrp.hpp>

#endif

namespace mantis::go {
    /**
    * @brief async_metadata struct to return info about a tx_channel
    * @param channel channel number
    * @param has_time_spec flag to indicate whether event has a timespec
    * @param time_spec time_spec of the event
    */
    struct async_mtx_metadata {
        size_t channel;
        bool has_time_spec;
        time_tag time_spec;

        metadata_errors::async_mtx_metadata_error_codes code;

#ifdef M_UHD

        /**
         * @brief converts async_mtx_metadata to uhd::async_metadata_t
         */
        static uhd::async_metadata_t from_async_mtx_md(async_mtx_metadata &a_mtx_md);

        /**
         * @brief converts uhd::async_metadata_t to async_mtx_metadata
         */
        static async_mtx_metadata to_async_mtx_md(uhd::async_metadata_t &a_mtx_md);

#endif
    };

}
