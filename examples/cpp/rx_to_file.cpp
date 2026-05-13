//
// Created by Jon Kler on 6/10/25.
//

#include <mantis/mantis.h>
#include <fstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

static constexpr double RX_BUFF_SIZE = 1e5;
static constexpr double threshold = 1e-6;
static constexpr size_t INDICATOR_SLEEP_TIME = 3;

static std::string program_description = "Receive samples at specified RX params and write to file.";

#define PASSES_THRESHOLD(a, b) (std::abs(a - b) < threshold)

int main(int argc, char **argv) {
    po::options_description desc(program_description + "\nOptions");
    desc.add_options()
            ("help,h", "produce help message")
            ("filename", po::value<std::string>()->required(),
             "REQUIRED. Full path of the file to write to. Opens in overwrite mode")
            ("args", po::value<std::string>(), "device args str")
            ("freq", po::value<double>()->default_value(100e6, "100e6"), "Rx freq [Hz], will default to 100MHz")
            ("lo", po::value<double>(), "lo offset freq [Hz] By default will allow SDR lib to choose most fitting lo")
            ("rate", po::value<double>()->default_value(10e6, "10e6"), "Rx rate [Sps], will default to 10 MSps")
            ("gain", po::value<double>()->default_value(0.0), "Rx gain")
            ("master_clock_rate,mcr", po::value<double>(),
             "master clock rate; can also be passed in args as master_clock_rate. This argument takes precedence")
            ("channel", po::value<size_t>()->default_value(0), "channel num");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }

    po::notify(vm);


    mantis::utils::pversion();
    
    /// first, try to open file
    std::ofstream file(vm["filename"].as<std::string>(), std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        mantis::utils::perror("Failed to open file: " + vm["filename"].as<std::string>());
        return EXIT_FAILURE;
    }

    auto &d_manager = mantis::device_manager::get_instance();

    /// acquire sdr
    mantis::params::msdr_params params{};
    if (vm.count("args")) {
        if (!mantis::errors::succeeded(mantis::params::msdr_params::from_str(vm["args"].as<std::string>(), params))) {
            throw mantis::runtime_error("Could not parse arg string: " + vm["args"].as<std::string>());
        }
    }

    if (vm.count("master_clock_rate")) {
        params.master_clock_rate = vm["master_clock_rate"].as<double>();
    }

    if (!mantis::errors::succeeded(d_manager.init(1, params))) {
        throw mantis::runtime_error("Could not init relevant SDR");
    }

    std::optional<double> lo = (vm.count("lo") ? std::optional<double>{vm["lo"].as<double>()} : std::nullopt);

    double freq = vm["freq"].as<double>();
    double rate = vm["rate"].as<double>();
    double gain = vm["gain"].as<double>();
    size_t channel = vm["channel"].as<size_t>();

    /// try to acquire channel
    auto [err, rx_channel] = d_manager.get_rx_channel(params, channel);
    if (!mantis::errors::succeeded(err)) {
        mantis::utils::perror("Failed to Acquire Channel: " + mantis::errors::mantis_errno(err));
    }

    /// configure channel
    double ac_freq = rx_channel->set_freq(freq, lo);
    if (!PASSES_THRESHOLD(ac_freq, freq)) {
        mantis::utils::pwarn(
            "Couldn't set freq to: " + std::to_string(freq) + ". Actual freq: " + std::to_string(ac_freq) + " Hz");
    }

    double ac_gain = rx_channel->set_gain(gain);
    if (!PASSES_THRESHOLD(ac_gain, gain)) {
        mantis::utils::pwarn(
            "Couldn't set gain to: " + std::to_string(gain) + ". Actual gain: " + std::to_string(ac_gain) + " dB");
    }

    double ac_rate = rx_channel->set_rate(rate);
    if (!PASSES_THRESHOLD(ac_rate, rate)) {
        mantis::utils::pwarn(
            "Couldn't set rate to: " + std::to_string(rate) + ". Actual rate: " + std::to_string(ac_rate) + " Hz");
    }

    char *rx_buff = (char *) malloc(RX_BUFF_SIZE * sizeof(char));
    if (!rx_buff) {
        mantis::utils::perror("Failed to malloc Rx buffer");
        return EXIT_FAILURE;
    }

    int sample_size;
    if (mantis::config::CPU_SAMPLE_FORMAT == "fc32") {
        sample_size = 8;
    } else if (mantis::config::CPU_SAMPLE_FORMAT ==
               "fc64") {
        // should also not be mantis api, consider compiling with diff library target for these
        sample_size = 16;
    } else {
        mantis::utils::perror("Unknown sample formant");
        return EXIT_FAILURE;
    }
    (void) sample_size;

    mantis::go::mrx_metadata rx_md{.start_of_burst = true};

    mantis::utils::pinfo("++++++Begin RX+++++");
    mantis::utils::pinfo("Quit to stop");

    size_t sample_num = RX_BUFF_SIZE / sample_size;

    // dispatch thread to indicate rx
    bool running = true;
    auto indicator = std::thread([&running]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(INDICATOR_SLEEP_TIME));
            std::cout << "+" << std::flush;
        }
    });

    while (true) {
        if (!rx_channel->is_valid()) [[unlikely]] {
            mantis::utils::perror("SDR healthcheck Failed, channel invalid. Shutting Down...");
            running = false;
            indicator.join();
            return EXIT_FAILURE;
        }

        rx_channel->receive(rx_buff, sample_size, sample_num, rx_md);
        rx_md.has_time_spec = false; // reset timespec so we dont schedule the next recv to be in the past 
        file.write(rx_buff, sample_num);
    }
}
