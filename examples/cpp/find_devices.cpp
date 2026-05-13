//
// Created by Jon Kler on 6/11/25.
//

#include <mantis/mantis.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

static std::string program_description = "Find all connected devices matching arguments.";

int main(int argc, char **argv) {
    po::options_description desc(program_description + "\nOptions");
    desc.add_options()
            ("help,h", "produce help message")
            ("args", po::value<std::string>(), "device args str. Leave empty to find all");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }

    po::notify(vm);

    mantis::utils::pversion();
    
    auto &d_manager = mantis::device_manager::get_instance();

    mantis::params::msdr_params params{};
    if (vm.count("args")) {
        if (!mantis::errors::succeeded(mantis::params::msdr_params::from_str(vm["args"].as<std::string>(), params))) {
            throw mantis::runtime_error("Could not parse arg string: " + vm["args"].as<std::string>());
        }
    }

    auto found_devices = d_manager.find(params);

    if (found_devices.empty()) {
        std::cout << "No Devices Found\n";
        return EXIT_SUCCESS;
    }

    static int i = 0;
    for (const auto &device_params: found_devices) {
        std::cout << "=================================\n";
        std::cout << "Mantis Device " << std::to_string(i++) << "\n";
        std::cout << "=================================\n";
        std::cout << device_params << std::endl;
        std::cout << "\n\n";
    }

    return EXIT_SUCCESS;
}
