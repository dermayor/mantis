//
// Created by Jon Kler on 4/7/25.
//

#include "mantis/lib/classes/msdr/msdr_params.h"
#include <mantis/lib/utilities/prints.h>
#include <vector>


#define APPEND_PARAM_TO_ARGS_IF_NOT_NULL(param) if (!this->param.empty()) {args += std::string(#param) + "=" + this->param + ","; }

// set bool to false if unequal, but allow empty.
#define INVALIDATE_IF_UNEQUAL_ALLOW_NULL(flag, param) if(specific.param != generic.param && !specific.param.empty()) flag = false;

std::string mantis::params::msdr_params::get_find_args() const {
    std::string args;
// if param isn't empty, append it to args
#define PARAM(param, default_val) APPEND_PARAM_TO_ARGS_IF_NOT_NULL(param)
    MSDR_SEARCH_PARAMS
#undef PARAM
    return args;
}

std::string mantis::params::msdr_params::get_config_args() const {
    std::string args;
#define PARAM(param, default_val) APPEND_PARAM_TO_ARGS_IF_NOT_NULL(param)
    MSDR_CONFIG_PARAMS
#undef PARAM
    return args;
}

bool mantis::params::msdr_params::compare(const msdr_params &specific, const msdr_params &generic) {
    bool valid = true;
#define PARAM(param, default_val) INVALIDATE_IF_UNEQUAL_ALLOW_NULL(valid, param)
    MSDR_SEARCH_PARAMS
#undef PARAM
#define PARAM(param, default_val) INVALIDATE_IF_UNEQUAL_ALLOW_NULL(valid, param)
    MSDR_CONFIG_PARAMS
#undef PARAM

    if (config::ALLOW_VIRTUAL && !virtual_msdr_params::compare(specific, generic)) {
        valid = false;
    }

    return valid;
}

///this map stores all possible arguments that we can have, and that way we can tell if we can change that param
std::unordered_map<std::string, std::string> possible_args = {
#define PARAM(param, default_val) {#param, default_val},
        MSDR_SEARCH_PARAMS
        MSDR_CONFIG_PARAMS
#undef PARAM
};

mantis::errors::error_code
mantis::params::msdr_params::from_str(const std::string &args, params::msdr_params &new_params,
                                      const std::string &delimiter) {
    std::vector<std::string> tokens;
    auto i_start = args.begin();
    size_t next_idex = 0;
    while (i_start != args.end()) {
        std::string curr_sub_str{i_start, args.end()};
        next_idex = curr_sub_str.find(delimiter);
        if (next_idex == std::string::npos) {
            tokens.emplace_back(curr_sub_str);
            break;
        }

        tokens.emplace_back(std::string{i_start, i_start + next_idex});
        i_start += next_idex + 1; // slip to start of next token
    }

/// split to key '=' val
    for (const auto &str: tokens) {
        static constexpr const char *param_delimiter = "=";
        const size_t idex = str.find(param_delimiter);
        if (idex == std::string::npos) {
            utils::perror("Missing Value for arg: " + str);
            return errors::error_code::INVALID_ARGUMENT;
        }

        std::string name = str.substr(0, idex);
        std::string val = str.substr(idex + 1, str.size());

/// if the argument given is one of the msdr params, set it
#define PARAM(param, default_val) if(#param == name) {new_params.param = val; continue; }
        MSDR_SEARCH_PARAMS
        MSDR_CONFIG_PARAMS
#undef PARAM
    }
    return errors::error_code::SUCCESS;
}

#ifdef M_UHD

mantis::params::msdr_params
mantis::params::msdr_params::from_uhd_device_addr(uhd::device_addr_t address, const msdr_params &original) {
    msdr_params updated{};

#define PARAM(param, default_val) updated.param = address[#param];
    MSDR_SEARCH_PARAMS
#undef PARAM

#define PARAM(param, default_val) updated.param = original.param;
    MSDR_CONFIG_PARAMS
#undef PARAM

    return updated;
}

#endif

namespace mantis::params {
    std::ostream &operator<<(std::ostream &stream, const mantis::params::msdr_params &params) {
        std::cout << "Device Info: \n";
#define PARAM(param, default_val) stream << "\t" << #param << ": " << params.param << "\n";
        MSDR_SEARCH_PARAMS
#undef PARAM

#define PARAM(param, default_val) stream << "\t" << #param << ": " << params.param << "\n";
        MSDR_CONFIG_PARAMS
#undef PARAM
        return stream;
    }
}
