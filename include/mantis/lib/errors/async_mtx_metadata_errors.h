//
// Created by Jon Kler on 6/8/25.
//

#pragma once

#include <mantis/config.h>
#include <unordered_map>

#ifdef M_UHD

#include <uhd/usrp/multi_usrp.hpp>

#endif

namespace mantis::metadata_errors {
    /**
     * @brief possible errors for asymc_mtx_metadata
     */
    enum class MANTIS_API async_mtx_metadata_error_codes {
        //! Burst Successfully transmitted
        BURST_ACK = 0x1,
        //! Underflow, Not receiving samples fast enough
        UNDERFLOW = 0x2,
        //! Packet loss between host and device
        SEQ_ERROR = 0x4,
        //! Packet was Late, expired timespec
        LATE = 0x8,
        //! Underflow in packet
        UNDERFLOW_IN_PACKET = 0x10,
        //! Packet loss in burst
        SEQ_ERROR_IN_BURST = 0x20,
        //! custom user payload
        USER_PAYLOAD = 0x40
    };

    /**
     * @brief get detailed error message from error code
     */
    [[maybe_unused]] std::string MANTIS_API mantis_errno(async_mtx_metadata_error_codes & err);

    std::unordered_map<async_mtx_metadata_error_codes, std::string> get_mtx_metadata_err_code_map();
}
