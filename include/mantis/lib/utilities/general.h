#pragma once

#include <mantis/config.h>
#include <string>

namespace mantis::utils {
    /***
     * @brief get the current sample size according to the CPU_SAMPLE_FORMAT config variable
     * @return the sample size in bytes
     * @throws mantis::runtime_error if the CPU_SAMPLE_FORMAT is not supported
     */
    int MANTIS_API get_sample_size();

} // namespace mantis::utils