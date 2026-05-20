//
// Created by Jon Kler on 4/7/25.
//

#pragma once

#include <string>

#include <mantis/config.h>

#ifdef M_UHD

#include <uhd/usrp/multi_usrp.hpp>

#endif


#include <mantis/driver_impl/virtual/params/virtual_msdr_params.h>
#include <mantis/lib/errors/error_codes.h>

static constexpr const char *default_sdr_buff_size = "24862979";
static constexpr const char *default_str_param = "";

/* declare all potential sdr params, using an x macro for easy serialization */
#define MSDR_SEARCH_PARAMS            \
    PARAM(type, default_str_param)    \
    PARAM(serial, default_str_param)  \
    PARAM(name, default_str_param)    \
    PARAM(product, default_str_param) \
    PARAM(addr, default_str_param)    \
    PARAM(fpga, default_str_param)

/* These params are what we will be configuring the sdr to */
#define MSDR_CONFIG_PARAMS                       \
    PARAM(master_clock_rate, default_str_param)  \
    PARAM(send_buff_size, default_sdr_buff_size) \
    PARAM(recv_buff_size, default_sdr_buff_size) \
    PARAM(clock_source, "internal") \
    PARAM(time_source, "internal")

namespace mantis::params {
/**
 * @brief msdr_params is a struct that contains all the parameters needed to
 * find, init and configure an SDR.
 * It is given to the device_manager to find and initialize SDRs, as well as
 * used to init and get specific tx/rx channels
 */
    struct MANTIS_API msdr_params : public mantis::params::virtual_msdr_params

    {
// declare params with their default val
#define PARAM(param, default_val) std::string param{default_val};
        MSDR_SEARCH_PARAMS
        MSDR_CONFIG_PARAMS
#undef PARAM

        /**
         * @brief returns a string representation of the MSDR_SEARCH_PARAMS that
         * can be used by driver libs to find/init the sdr
         */
        [[nodiscard]] std::string
        get_find_args() const;

        /**
         * @brief returns a string representation of the MSDR_CONFIG_PARAMS that can
         * be used by driver libs to configure the sdr
         * @return arg string of key=value pairs seperated by ","
         */
        [[nodiscard]] std::string get_config_args() const;


        /**
         * @brief function used to compare params for "equality" will only invalidate the comparison if the specific params has a field differing from the generic params that is not null
         * for example, if the specific params has product set to X310, and the generic params has no product set, the result will be false.
         * However, in the reverse case the result will be true. The goal is to find any sdr that matches the "constraints" of the specific params
         * @param specific specific params
         * @param generic generic params
         */
        [[nodiscard]] static bool compare(const msdr_params &specific, const msdr_params &generic);


#ifdef M_UHD

        /**
         * @brief Forms a msdr_params struct from a uhd::device_addr_t address and
         * the original msdr_params.
         */
        static msdr_params from_uhd_device_addr(uhd::device_addr_t address,
                                                const msdr_params &original);

#endif
        /**
         * @brief update new_params with fields from a given string, seperated by custom delimiter
         * @param args string args split by a common delimiter into key=val pairs. for example: "type=b200;name=JonKler"
         * @param new_params params to parse args into
         * @param delimiter delimiter separating the args
         */
        static errors::error_code from_str(const std::string& args, params::msdr_params& new_params, const std::string& delimiter= ";");

        /**
         * @brief friend function to print msdr_params to ostream
         */
        friend MANTIS_API std::ostream& operator<<(std::ostream& stream, const msdr_params& params);
    };
}  // namespace mantis::params
