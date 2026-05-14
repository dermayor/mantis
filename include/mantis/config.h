//
// Created by Jon Kler on 4/14/25.
//

#ifndef MANTIS_CONFIG_H
#define MANTIS_CONFIG_H

#include <string>

// only allow client code to link to things that are part of the mantis api
#define MANTIS_API __attribute__((visibility("default")))

/* add default vals */
#define MANTIS_CONFIG_VARS \
    CONFIG_VAR(ALLOW_VIRTUAL, bool, false) \
    CONFIG_VAR(FORCE_VIRTUAL, bool, false) \
    CONFIG_VAR(USE_CONFIG_MSDRS, bool, false) \
    CONFIG_VAR(DEFAULT_VIRTUAL_TX_MODE, std::string, "FILE") \
    CONFIG_VAR(BASE_FILENAME, std::string, "mantis") \
    CONFIG_VAR(DEFAULT_VIRTUAL_RX_MODE, std::string, "FILE") \
    CONFIG_VAR(DEFAULT_VIRTUAL_HOST, std::string, "") \
    CONFIG_VAR(DEFAULT_VIRTUAL_PORT, int, 7777)   \
    CONFIG_VAR(SOCKET_TYPE, int, 2)   \
    CONFIG_VAR(FILE_EXTENSION, std::string, ".bin") \
    CONFIG_VAR(MONITOR_THREAD_SLEEP_TIME, int, 200) \
    CONFIG_VAR(RECONNECTION_THREAD_SLEEP_TIME, int, 500) \
    CONFIG_VAR(CPU_SAMPLE_FORMAT, std::string, "32fc") \
    CONFIG_VAR(OTW_SDR_FORMAT, std::string, "sc16")

namespace mantis::config {

#define CONFIG_VAR(name, type, default_val) extern MANTIS_API type name;
    MANTIS_CONFIG_VARS
#undef CONFIG_VAR

}  // namespace mantis::config

#endif  // MANTIS_CONFIG_H
