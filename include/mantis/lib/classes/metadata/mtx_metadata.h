//
// Created by Jon Kler on 4/17/25.
//

#pragma once

#include <cstddef>
#include <mantis/config.h>
#include <mantis/lib/classes/time_tag.h>

#ifdef M_UHD

#include <uhd/usrp/multi_usrp.hpp>

#endif

namespace mantis::go {
/**
 * @brief Metadata for mtx_channels. Contains several fields for transmission
 * control
 */
    struct MANTIS_API mtx_metadata {
        size_t eov_positions_size = 0;
        size_t *eov_positions = nullptr;

        /**
         * time to send samples at.\n
         * It is recommended you set the timespec relative to the channel's current
         * time
         * @note Users should use set_time_spec() to set this value and not modify
         * it directly.
         */
        time_tag time_spec{0};
        double timeout{0.1};

        bool start_of_burst{false};
        bool end_of_burst{false};

        /**
         * @brief Indicates if the time spec is set.
         * This is used to determine if the time spec should be sent with the
         * samples.
         * @note Users should use set_time_spec() to set this value and not modify
         * it directly.
         */
        bool has_time_spec{false};

        /**
         * @brief Helper to set time spec and has time spec
         * @param _time_spec: unix time in microseconds to send samples at.\n
         * It is recommended you set the timespec relative to the channel's current
         * time
         */
        void set_time_spec(time_tag _time_spec);

#ifdef M_UHD

        //! creates uhd_md_tx from current object
        uhd::tx_metadata_t convert_to_uhd_md_tx();

        /**
         * @brief update params with new data from md
         * @param params params to update
         * @param md uhd metadata to update from
         */
        static void from_uhd_md_tx(mtx_metadata &params, uhd::tx_metadata_t md);

#endif

    };
}  // namespace mantis::go
