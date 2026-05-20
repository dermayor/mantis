
//
// Created by Jon Kler on 4/7/25.
//

#pragma once

#include <memory>
#include <vector>

#include <mantis/config.h>
#include <mantis/lib/classes/mchannel/mrx_channel.h>
#include <mantis/lib/classes/mchannel/mtx_channel.h>

#include <nlohmann/json.hpp>

#ifdef M_UHD

#include <uhd/usrp/multi_usrp.hpp>

#endif

namespace mantis::go {
    /**
     * @brief Generic, driver agnostic SDR class. This class is intended only
     * for internal mantis usage. Users may set all sdr parameters through an mrx/mtx_channel
     */
    class msdr {
    public:
        using sptr = std::shared_ptr<msdr>;

        /**
         * @brief constructs msdr through msdr_params. Only moves the params,
         * actual channels are given in the relevant <driver>_device_finder functions.
         */
        explicit msdr(const params::msdr_params _params);

        /**
         * constructs msdr through json (used for virtual msdrs)
         */
        explicit msdr(nlohmann::json msdr_json);

        /**
         * constructs custom msdr (used for virtual msdrs)
         */
        msdr(params::msdr_params _params, size_t _tx_channel_count, size_t _rx_channel_count);

        msdr() = default;


        /**
         * @brief Performs a health check on the sdr. Checks all tx and rx channels.
         * Implementation of the cannel healthcheck function in driver specific
         */
        [[nodiscard]] bool health_check();

        /**
         * @brief Gets a mtx_channel from the msdr.
         * @param channel_num the channel number to get
         * @return a pair of error_code and mtx_channel::sptr. The error_code will
         * be SUCCESS and a valid mtx_channel::sptr will be given if the operation
         * was successful.
         * @return nullptr and NO_TX_CHANNELS if no tx channels are available, or
         * INSUFFICIENT_TX_CHANNELS if the requested channel number is greater than
         * the number of tx channels available on the msdr,
         */
        [[nodiscard]] std::pair<errors::error_code, mtx_channel::sptr>
        get_tx_channel(size_t channel_num);

        /**
         * @brief Gets a mrx_channel from the msdr.
         * @param channel_num the channel number to get
         * @return a pair of error_code and mrx_channel::sptr. The error_code will
         * be SUCCESS and a valid mrx_channel::sptr will be given if the operation
         * was successful.
         * @return nullptr and NO_RX_CHANNELS if no rx channels are available, or
         * INSUFFICIENT_RX_CHANNELS if the requested channel number is greater than
         * the number of rx channels available on the msdr,
         */
        [[nodiscard]] std::pair<errors::error_code, mrx_channel::sptr>
        get_rx_channel(size_t channel_num);

        /**
         * @brief Invalidates the msdr and all its channels.
         * @note This DOES NOT DELETE THE SDR pointer. The pointer will still exist
         * but should not be used. Users should ensure channels are valid before use
         * by calling th i_channel::is_valid() function.
         */
        void invalidate();

        /**
         * @brief Checks if the msdr is valid.
         * @return true if the msdr is valid, false otherwise
         */
        [[nodiscard]] bool is_valid() const;

        /**
         * @brief Gets the msdr_params for this msdr.
         */
        [[nodiscard]] params::msdr_params get_params() const;

        /**
         * @brief Get a reference to the connected mtx_channels vector.
         */
        [[nodiscard]] std::vector<mtx_channel::sptr> &get_tx_channels();

        /**
         * @brief Get a reference to the connected mrx_channels vector.
         */
        [[nodiscard]] std::vector<mrx_channel::sptr> &get_rx_channels();

    private:
        params::msdr_params sdr_params;
        std::vector<mtx_channel::sptr> tx_channels;
        std::vector<mrx_channel::sptr> rx_channels;
        bool valid{true};
    };
}  // namespace mantis::go
