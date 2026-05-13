
#include "mantis/lib/utilities/general.h"
#include "mantis/mantis.h"

int mantis::utils::get_sample_size() {
    if (mantis::config::CPU_SAMPLE_FORMAT == "fc32") {
        return 8;
    }

    if (mantis::config::CPU_SAMPLE_FORMAT == "fc64") {
        return 16;
    }

    throw mantis::runtime_error("Unknown sample format: " + mantis::config::CPU_SAMPLE_FORMAT);
}