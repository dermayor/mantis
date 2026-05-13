//
// Created by Jon Kler on 3/23/25.
//

#pragma once

#include <thread>
#include <vector>
#include <mantis/config.h>
#include <mantis/lib/classes/msdr/msdr.h>
#include <mantis/lib/classes/msdr/msdr_params.h>
#include <mantis/lib/errors/error_codes.h>
#include <mantis/lib/errors/exceptions.h>
#include <mantis/lib/utilities/spin_lock.h>

namespace mantis {
    /**
     * @brief The device_manager is a singleton class that manages all SDR devices
     * connected to the system. It provides methods to init and initialize new SDRs,
     * as well as get tx and rx channels
     * In addition to managing the SDRs, the device_manager also dispatches a
     * monitoring thread that checks the health of the connected SDRs periodically,
     * and invalidates them if they fail the health check. The SDRS do not disappear
     * after failing the healthcheck. All users should validate their channels
     * before using them.
     */
    class MANTIS_API device_manager {
    public:
        /**
         * @brief get an instance of the device_manager.
         */
        static device_manager &get_instance();

        /**
         * @brief connect and initialize a requested number of SDRs.
         * @param num number of SDRs to connect and initialize, 0 to init all. Alternatively call init_all
         * @param msdr_params parameters to use for finding and initializing the
         * SDRs
         * @note the function will initialize the first SDRs matching the given
         * msdr_params. Users should be as specific as possible if a specific SDR is
         * desired.
         * @return error_code::SUCCESS if the operation succeeded
         */
        mantis::errors::error_code init(int num,
                                        params::msdr_params msdr_params);

        /**
         * @brief init and initialize all supported SDRs connected to the system.
         * @param msdr_params parameters to use for finding and initializing the
         * SDRs.
         * @return error_code::SUCCESS if the operation was successful
         */
        mantis::errors::error_code init_all(params::msdr_params msdr_params);

        /**
         * @brief get a mtx_channel matching the given msdr_params and channel_num.
         * @param msdr_params parameters used to init the relevanr SDR
         * @param channel_num the desired tx channel number of the relevant SDR
         * @note the function will attempt to get a tx channel from the FIRST SDR
         * matching the given parameters. It is recommended users pass as many
         * details as possible in the msdr_params
         * @return error_code::SUCCESS and a mtx_channel::sptr if the operation was
         * successful
         * @return error_code::NO_SDRS_CONNECTED and a nullptr if no SDRs are
         * connected
         * @return error_code::INVALID_SDR and a nullptr if the requested SDR was
         * found but invalidated for some reason (disconnected, failed healthcheck,
         * etc...)
         * @return error_code::NO_MATCHING_SDR and a nullptr if no matching SDR was
         * found
         * @return error_code::NO_TX_CHANNELS and a nullptr if the matching SDR has
         * no tx channels available
         * @return error_code::INSUFFICIENT_TX_CHANNELS and a nullptr if the
         * matching SDR has insufficient tx channels available (total channels <
         * channel_num + 1)
         */
        std::pair<errors::error_code, go::mtx_channel::sptr> get_tx_channel(
                const params::msdr_params &msdr_params, size_t channel_num);

        /**
         * @brief get a mrx_channel matching the given msdr_params and channel_num.
         * @param msdr_params parameters used to init the relevanr SDR
         * @param channel_num the desired rx channel number of the relevant SDR
         * @note the function will attempt to get a rx channel from the FIRST SDR
         * matching the given parameters. It is recommended users pass as many
         * details as possible in the msdr_params
         * @return error_code::SUCCESS and a mrx_channel::sptr if the operation was successful
         * @return error_code::NO_SDRS_CONNECTED and a nullptr if no SDRs are
         * connected
         * @return error_code::INVALID_SDR and a nullptr if the requested SDR was
         * found but invalidated for some reason (disconnected, failed healthcheck,
         * etc...)
         * @return error_code::NO_MATCHING_SDR and a nullptr if no matching SDR was found
         * @return error_code::INSUFFICIENT_RX_CHANNELS and a nullptr if the
         * requested channel number is greater than the number of rx channels
         * available on the SDR
         * @return error_code::NO_RX_CHANNELS and a nullptr if the matching SDR has
         * no rx channels available
         * @return error_code::INSUFFICIENT_RX_CHANNELS and a nullptr if the
         * matching SDR has insufficient rx channels available (total rx channels <
         * channel_num + 1)
         */
        std::pair<errors::error_code, go::mrx_channel::sptr> get_rx_channel(
                const params::msdr_params &msdr_params, size_t channel_num);

        /**
         * @brief return vector of params of devices connected to the machine NOT managed by mantis
         * @param params parameters to search by
         */
        std::vector<params::msdr_params> find(params::msdr_params params);

        /**
         * @brief return vector of params of devices being managed by mantis
         */
        std::vector<params::msdr_params> get_connected();


        device_manager(device_manager &other) = delete;

        device_manager(device_manager &&other) = delete;


    private:

        friend MANTIS_API std::ostream& operator<<(std::ostream &stream, const device_manager &d_manager);

        device_manager();

        ~device_manager();

        /**
        * @brief Uses the mantis-config.json file to configure mantis variables
        * @note the environment variable MANTIS_CONFIG_FILE_PATH can be used to
        * specify a different path to the mantis-config.json file.
         */
        void configure();

        mantis::errors::error_code dispatch_monitoring_thread();

        /**
         * currently doesnt do anything :)
         */
        mantis::errors::error_code dispatch_reconnection_thread();

        mantis::errors::error_code reconnect_msdr(go::msdr::sptr invalid_sdr);

        static mantis::errors::error_code init_internal(
                int num, std::vector<go::msdr::sptr> &o_found_sdrs,
                params::msdr_params msdr_params);

        std::thread reconnection_thread;
        std::thread monitoring_thread;
        std::vector<go::msdr::sptr> connected_sdrs{};
        utils::spin_lock lock;
        std::atomic<bool> running;
    };
}  // namespace mantis