//
// Created by Jon Kler on 4/7/25.
//

#include "mantis/lib/classes/msdr/msdr.h"
#include "mantis/driver_impl/virtual/utilities/virtual_utils.h"
#include <mantis/lib/errors/exceptions.h>

#ifdef M_UHD

#include <utility>

#include "mantis/driver_impl/uhd/utilities/uhd_utils.h"

#endif


#include <iostream>
#include <mantis/lib/utilities/prints.h>

static int sdr_init_num = 0;

// to fit standard, capitalizes given field before searching for it
#define DESERIALIZE_IF_PROVIDED(field, def_value) \
    field_to_upper = #field; \
    std::transform(field_to_upper.begin(), field_to_upper.end(), field_to_upper.begin(), ::toupper); \
    params.field = (msdr_json.contains(field_to_upper) && !msdr_json[field_to_upper].empty()) ? msdr_json[field_to_upper].get<std::string>(): def_value;

mantis::go::msdr::msdr(const params::msdr_params _params)
        : sdr_params(std::move(_params)) {
    sdr_init_num++;
}

mantis::go::msdr::msdr(nlohmann::json msdr_json) {
    params::msdr_params params;

    size_t channel_num = 0;

    // take all fields, capitalize names, and parse if relevant
    std::string field_to_upper;
#define PARAM(field, def_val) DESERIALIZE_IF_PROVIDED(field, def_val)
    MSDR_CONFIG_PARAMS
    MSDR_SEARCH_PARAMS
#undef PARAM

    if (params.type.empty()) {
        params.type = "virtual";
    }

    for (auto &channel_data: msdr_json["TX_CHANNELS"]) {
        this->tx_channels.emplace_back(
                driver_impl::virtual_i::virtual_mtx_channel_from_json(channel_data, channel_num++, sdr_init_num));
    }
    params.tx_channel_count = channel_num;

    channel_num = 0;
    for (auto &channel_data: msdr_json["RX_CHANNELS"]) {
        this->rx_channels.emplace_back(
                driver_impl::virtual_i::virtual_mrx_channel_from_json(channel_data, channel_num++, sdr_init_num));
    }
    params.rx_channel_count = channel_num;

    this->sdr_params = params;

    sdr_init_num++;
}

mantis::go::msdr::msdr(params::msdr_params _params, size_t _tx_channel_count, size_t _rx_channel_count) : sdr_params(
        std::move(_params)) {
    for (size_t curr_chann = 0; curr_chann < _tx_channel_count; curr_chann++) {
        this->tx_channels.emplace_back(
                driver_impl::virtual_i::make_virtual_mtx_channel(this->sdr_params, curr_chann, sdr_init_num));
    }

    for (size_t curr_chann = 0; curr_chann < _rx_channel_count; curr_chann++) {
        this->rx_channels.emplace_back(
                driver_impl::virtual_i::make_virtual_mrx_channel(this->sdr_params, curr_chann, sdr_init_num));
    }
    sdr_init_num++;

}

[[nodiscard]] bool mantis::go::msdr::health_check() {
    bool healthy = true;
    for (const auto &channel: this->tx_channels) {
        if (!channel->healthcheck()) {
            healthy = false;
            break;
        }
    }

    if (healthy) {
        for (const auto &channel: this->rx_channels) {
            if (!channel->healthcheck()) {
                healthy = false;
                break;
            }
        }
    }
    return healthy;
}

[[nodiscard]] std::pair<mantis::errors::error_code, mantis::go::mtx_channel::sptr>
mantis::go::msdr::get_tx_channel(size_t channel_num) {
    if (tx_channels.empty()) {
        return {errors::error_code::NO_TX_CHANNELS, nullptr};
    } else if (channel_num > tx_channels.size() - 1) {
        return {errors::error_code::INSUFFICIENT_TX_CHANNELS, nullptr};
    }

    return {errors::error_code::SUCCESS, tx_channels.at(channel_num)};
}

[[nodiscard]] std::pair<mantis::errors::error_code, mantis::go::mrx_channel::sptr>
mantis::go::msdr::get_rx_channel(size_t channel_num) {
    if (rx_channels.empty()) {
        return {errors::error_code::NO_RX_CHANNELS, nullptr};
    } else if (channel_num > rx_channels.size() - 1) {
        return {errors::error_code::INSUFFICIENT_RX_CHANNELS, nullptr};
    }

    return {errors::error_code::SUCCESS, rx_channels.at(channel_num)};
}

void mantis::go::msdr::invalidate() {
    this->valid = false;
    for (auto &tx_channel: this->tx_channels) {
        tx_channel->invalidate();
    }
    for (auto &rx_channel: this->rx_channels) {
        rx_channel->invalidate();
    }
}

[[nodiscard]] bool mantis::go::msdr::is_valid() const { return this->valid; }

[[nodiscard]] mantis::params::msdr_params mantis::go::msdr::get_params() const {
    return this->sdr_params;
}

[[nodiscard]] std::vector<mantis::go::mtx_channel::sptr> &mantis::go::msdr::get_tx_channels() {
    return this->tx_channels;
}

[[nodiscard]] std::vector<mantis::go::mrx_channel::sptr> &mantis::go::msdr::get_rx_channels() {
    return this->rx_channels;
}
