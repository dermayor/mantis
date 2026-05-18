//
// Created by Jon Kler on 3/23/25.
//

#include "mantis/lib/device_manager/device_manager.h"

#include <fstream>
#include <mantis/lib/utilities/prints.h>
#include <utility>

// #include "mantis/lib/device_manager/device_manager_utils.h"

#include <nlohmann/json.hpp>

#ifdef __uhd

#include "mantis/driver_impl/uhd/interface_impl/uhd_device_finder.h"

#endif

#include "mantis/driver_impl/virtual/interface_impl/virtual_device_finder.h"

#ifdef __pymantis

#include <csignal>

#endif

mantis::device_manager& mantis::device_manager::get_instance() {
    static device_manager device_manager;
    return device_manager;
}

mantis::device_manager::device_manager() {
#ifdef __pymantis
    std::signal(SIGINT, [](int sig) {
        std::exit(128 + sig);
    }); // needed this because otherwise pybind11 may not exit a while(true) cpp loop
#endif

    this->configure();
    this->running.store(true);                               // monitor running
    errors::succeeded(this->dispatch_monitoring_thread());   // TODO fail behavior?
    errors::succeeded(this->dispatch_reconnection_thread()); // TODO get rid of this for now
}

mantis::device_manager::~device_manager() {
    if (this->running.load()) {
        this->running.store(false);
    }

    if (this->reconnection_thread.joinable()) {
        this->reconnection_thread.join();
    }

    if (this->monitoring_thread.joinable()) {
        this->monitoring_thread.join();
    }
}

mantis::errors::error_code mantis::device_manager::dispatch_monitoring_thread() {
    this->monitoring_thread = std::thread([&]() {
        while (this->running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config::MONITOR_THREAD_SLEEP_TIME));
            this->lock.lock();
            for (auto& msdr : this->connected_sdrs) {
                if (!msdr->health_check()) {
                    msdr->invalidate();
                }
            }
            this->lock.unlock();
        }
    });
    return errors::error_code::SUCCESS;
}

mantis::errors::error_code mantis::device_manager::dispatch_reconnection_thread() {
    this->reconnection_thread = std::thread([&]() {
        while (this->running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config::RECONNECTION_THREAD_SLEEP_TIME));
            //            this->lock.lock();
            //            for (auto &msdr: this->connected_sdrs) {
            ////                if (!msdr->is_valid()) {
            ////                    throw mantis::runtime_error(
            ////                            "Cannot reach SDR, Ensure SDR powered on and network "
            ////                            "connected");
            ////                    //                    std::cout << "attempting reconnect" <<
            ////                    //                    std::endl; TODO: actually reconnect
            ////                    //                    (void) reconnect_msdr(msdr); // the
            ////                    //                    perror here doesn't really matter since
            ////                    //                    we're already connected
            ////                }
            //            }
            //            this->lock.unlock();
        }
    });

    return errors::error_code::SUCCESS;
}

mantis::errors::error_code mantis::device_manager::reconnect_msdr(go::msdr::sptr invalid_sdr) {
    //    // since you skip existing sdrs (at least in uhd init)
    //    std::vector<go::msdr::sptr> potential_sdr_match{};
    //    auto errcode =
    //            this->init_internal(1, potential_sdr_match, invalid_sdr->get_params());
    //    if (potential_sdr_match.empty()) {
    //        return errcode;
    //    }
    //
    //    go::msdr::sptr sdr_match =
    //            potential_sdr_match.at(0);
    //    device_manager_utils::replace_msdr_sptr_channels_and_data(invalid_sdr,
    //                                                              sdr_match);
    return errors::error_code::SUCCESS; // refactor when making this normal
}

mantis::errors::error_code mantis::device_manager::init(int num, params::msdr_params msdr_params) {
    this->lock.lock();
    mantis::errors::error_code result = mantis::device_manager::init_internal(num, this->connected_sdrs, msdr_params);
    this->lock.unlock();
    return result;
}

mantis::errors::error_code mantis::device_manager::init_internal(int num, std::vector<go::msdr::sptr>& o_found_sdrs,
                                                                 params::msdr_params msdr_params) {

    if (config::FORCE_VIRTUAL && config::ALLOW_VIRTUAL) {
        auto err = driver_impl::virtual_i::virtual_device_finder::init(num, o_found_sdrs, msdr_params);
        if (!errors::succeeded(err))
            return err;
        return errors::error_code::SUCCESS;
    }

    int needed_sdrs{num};
    // before we init we need the total number of found devices
#ifdef __uhd
    int uhd_sdrs = driver_impl::uhd_i::uhd_device_finder::get_num_connected_unused(msdr_params);

    if (uhd_sdrs > needed_sdrs) { // only init what we need
        uhd_sdrs = needed_sdrs;
        needed_sdrs = 0;
    } else {
        needed_sdrs -= uhd_sdrs;
    }

#endif

    if (!config::ALLOW_VIRTUAL && needed_sdrs > 0) {
        return errors::error_code::INSUFFICIENT_SDRS_FOUND;
    }

#ifdef __uhd
    if (!config::FORCE_VIRTUAL) {
        auto err = driver_impl::uhd_i::uhd_device_finder::init(uhd_sdrs, o_found_sdrs, msdr_params);
        if (!errors::succeeded(err) && !config::ALLOW_VIRTUAL)
            return err;
    }
#endif

    // remainder goes to virtual
    if (config::ALLOW_VIRTUAL) {
        auto err = driver_impl::virtual_i::virtual_device_finder::init(needed_sdrs, o_found_sdrs, msdr_params);
        if (!errors::succeeded(err)) {
            return err;
        }
    }

    return errors::error_code::SUCCESS;
}

mantis::errors::error_code mantis::device_manager::init_all(params::msdr_params msdr_params) {
#ifdef __uhd
    auto err = driver_impl::uhd_i::uhd_device_finder::init(0, this->connected_sdrs, std::move(msdr_params));
    if (!errors::succeeded<errors::error_code::NO_SDRS_CONNECTED>(err)) {
        return err;
    }
#endif
    return errors::error_code::SUCCESS;
}

std::pair<mantis::errors::error_code, mantis::go::mtx_channel::sptr>
mantis::device_manager::get_tx_channel(const params::msdr_params& msdr_params, size_t channel_num) {

    errors::error_code relevant_error =
        (this->connected_sdrs.empty()) ? errors::error_code::NO_SDRS_CONNECTED : errors::error_code::NO_MATCHING_SDR;

    // TODO: add scan for matching devices and init them, do the same for rx
    for (const auto& msdr : this->connected_sdrs) {
        if (!params::msdr_params::compare(msdr_params, msdr->get_params())) {
            continue;
        }

        if (!msdr->is_valid()) {
            relevant_error = errors::error_code::INVALID_SDR;
            continue;
        }

        auto [error_code, tx_channel] = msdr->get_tx_channel(channel_num);

        if (!errors::succeeded(error_code)) {
            relevant_error = error_code; // update relevant error to most recent perror
            continue;
        }

        if (tx_channel.use_count() > 2) {
            relevant_error = errors::error_code::CHANNEL_IN_USE;
            continue;
        }

        return {errors::error_code::SUCCESS, tx_channel};
    }

    if (errors::succeeded_as_long_as_not<errors::error_code::INVALID_SDR>(relevant_error) && config::ALLOW_VIRTUAL) {

        auto virtual_msdr = std::make_shared<go::msdr>(msdr_params, channel_num + 1, 0);

        this->connected_sdrs.emplace_back(virtual_msdr);

        auto [error_code, tx_channel] = virtual_msdr->get_tx_channel(channel_num);
        if (!errors::succeeded(error_code)) {
            return {error_code, nullptr};
        }

        return {errors::error_code::SUCCESS, tx_channel};
    }

    return {relevant_error, nullptr};
}

std::pair<mantis::errors::error_code, mantis::go::mrx_channel::sptr>
mantis::device_manager::get_rx_channel(const params::msdr_params& msdr_params, size_t channel_num) {

    errors::error_code relevant_error =
        (this->connected_sdrs.empty()) ? errors::error_code::NO_SDRS_CONNECTED : errors::error_code::NO_MATCHING_SDR;
    for (const auto& msdr : this->connected_sdrs) {
        if (!params::msdr_params::compare(msdr_params, msdr->get_params())) {
            continue;
        }

        if (!msdr->is_valid()) {
            relevant_error = errors::error_code::INVALID_SDR;
            continue;
        }

        auto [error_code, rx_channel] = msdr->get_rx_channel(channel_num);

        if (!errors::succeeded(error_code)) {
            relevant_error = error_code; // update relevant error to most recent perror
            continue;
        }

        if (rx_channel.use_count() > 2) {
            relevant_error = errors::error_code::CHANNEL_IN_USE;
            continue;
        }

        return {errors::error_code::SUCCESS, rx_channel};
    }

    if (errors::succeeded_as_long_as_not<errors::error_code::INVALID_SDR>(relevant_error) && config::ALLOW_VIRTUAL) {

        auto virtual_msdr = std::make_shared<go::msdr>(msdr_params, 0, channel_num + 1);

        this->connected_sdrs.emplace_back(virtual_msdr);

        auto [error_code, rx_channel] = virtual_msdr->get_rx_channel(channel_num);
        if (!errors::succeeded(error_code)) {
            return {error_code, nullptr};
        }

        return {errors::error_code::SUCCESS, rx_channel};
    }

    return {relevant_error, nullptr};
}

std::vector<mantis::params::msdr_params> mantis::device_manager::find(params::msdr_params params) {
    std::vector<params::msdr_params> found_devices{};

#ifdef __uhd
    std::string args_str = params.get_find_args();
    auto found_uhd_devices = uhd::device::find(args_str);
    for (const auto& device : found_uhd_devices) {
        params::msdr_params curr_params;
        params::msdr_params::from_str(device.to_string(), curr_params, ",");
        found_devices.emplace_back(curr_params);
    }
#endif
    return found_devices;
}

std::vector<mantis::params::msdr_params> mantis::device_manager::get_connected() {
    std::vector<params::msdr_params> connected_devices{};
    connected_devices.reserve(this->connected_sdrs.size());

    for (const auto& msdr : this->connected_sdrs) {
        connected_devices.emplace_back(msdr->get_params());
    }
    return connected_devices;
}

void mantis::device_manager::configure() {
    const char* config_file_raw = std::getenv("MANTIS_CONFIG_FILE");
    std::string config_file;
    if (config_file_raw == nullptr) {

        config_file_raw = std::getenv("HOME");
        if (config_file_raw == nullptr) {
            throw mantis::runtime_error(
                "failes to parse env vars MANTIS_CONFIG_FILE and HOME, cannot search for cofig file");
        }

        config_file = std::string(config_file_raw) + "/.config/mantis/mantis-config.json";

    } else {
        config_file = config_file_raw;
    }

    std::ifstream mantis_config_json(config_file);
    if (!mantis_config_json.is_open()) {
        throw std::runtime_error(std::string("Unable to open Json: ") + config_file +
                                 ", set MANTIS_CONFIG_FILE env var for custom location");
    }
    nlohmann::json data = nlohmann::json::parse(mantis_config_json);

    // add warnings when null
#define CONFIG_VAR(name, type, default_val)                                                                            \
    try {                                                                                                              \
        if (data.contains(#name))                                                                                      \
            config::name = data[#name].get<type>();                                                                    \
    } catch (...) {                                                                                                    \
        throw mantis::runtime_error("Failed parsing json field: " #name);                                              \
    }
    MANTIS_CONFIG_VARS
#undef CONFIG_VAR

    if (config::ALLOW_VIRTUAL && config::USE_CONFIG_MSDRS) {
        if (!data.contains("VIRTUAL_MSDRS")) {
            mantis::utils::pwarn("USE_CONFIG_MSDRS enabled but none given");
            return;
        }
        for (auto& msdr_json : data["VIRTUAL_MSDRS"]) {
            this->connected_sdrs.emplace_back(std::make_shared<go::msdr>(msdr_json));
        }
    }
}

namespace mantis {
    std::ostream& operator<<(std::ostream& stream, const mantis::device_manager& d_manager) {
        if (d_manager.connected_sdrs.empty()) {
            stream << "No devices connected\n";
            return stream;
        }

        int i = 0;
        for (const auto& msdr : d_manager.connected_sdrs) {
            stream << "=================================\n";
            stream << "Mantis Device " << std::to_string(i++) << "\n";
            stream << "=================================\n";
            stream << msdr->get_params() << std::endl;
            stream << "\n\n";
        }
        return stream;
    }
} // namespace mantis
