//
// Created by Jon Kler on 5/6/25.
//

#pragma once

#include <cstddef>
#include <mantis/config.h>
#include <mantis/lib/classes/time_tag.h>
#include <mantis/lib/errors/mrx_metadata_errors.h>

#ifdef __uhd

#include <uhd/usrp/multi_usrp.hpp>

#endif

namespace mantis::go {
    /**
     * @brief Metadata for mrx_channels. Contains several fields for rx control
     */
    struct MANTIS_API mrx_metadata {
        size_t eov_positions_size = 0;
        size_t eov_positions_count{0};
        size_t fragment_offset{0};

        /**
         * @brief unix time in microseconds to recv samples at if set before recv, after recv will be overwritten with
         * actual rx time. If not set, samples will be received as soon as possible. It is recommended you set the
         * timespec relative to the channel's current time.          
         * @note After recv is caled users should either manually set has_time_spec to false r use a different
         * mrx_metdata in the next call. otherwise the system may try to perform a scheduled recv in the past.
         */
        time_tag time_spec{0};
        double timeout{0.1};

        size_t* eov_positions = nullptr;

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
        bool more_fragments = false;
        bool out_of_sequence{false};
        bool one_packet{false};

        /**
         * @brief Helper to set time spec and has time spec
         * @param _time_spec: time tag to begin rx. After the receive call, the timespec will contain the actual time
         * samples were received.\n It is recommended you set the timespec relative to the channel's current time
         */
        void set_time_spec(time_tag _time_spec);

        /**
         * Error returned. NO_ERROR (0) on success
         */
        mantis::metadata_errors::mrx_metadata_error_codes code;

#ifdef __uhd

        uhd::rx_metadata_t convert_to_uhd_md_rx();

        static void from_uhd_md_rx(mantis::go::mrx_metadata& params, uhd::rx_metadata_t md);
#endif
    };
} // namespace mantis::go
