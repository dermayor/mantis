//
// Created by Jon Kler on 6/5/25.
//

#include "mantis/lib/utilities/prints.h"
#include "mantis/version.h"
#include <iostream>

void mantis::utils::pwarn(const std::string& warning) {
    std::cerr << "\033[33m[MANTIS][WARNING] " << warning << "\033[0m" << std::endl;
}

void mantis::utils::perror(const std::string& error) {
    std::cerr << "\033[31m[MANTIS][ERROR] " << error << "\033[0m" << std::endl;
}

void mantis::utils::pinfo(const std::string& message) {
    std::cout << "\033[32m[MANTIS][INFO] " << message << "\033[0m\n";
}

void mantis::utils::pversion() {
    std::cout << "\033[32m[MANTIS VERSION] " << MANTIS_VERSION << "\033[0m" << std::endl;
}