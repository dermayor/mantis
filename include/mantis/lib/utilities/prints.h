//
// Created by Jon Kler on 6/5/25.
//

#pragma once

#include <mantis/config.h>
#include <string>

namespace mantis::utils {
    //! pretty mantis warning print
    void MANTIS_API pwarn(const std::string& warning);

    //! pretty mantis error print
    void MANTIS_API perror(const std::string& error);

    //! pretty mantis info print
    void MANTIS_API pinfo(const std::string& message);

    //! print mantis version info
    void MANTIS_API pversion();
}