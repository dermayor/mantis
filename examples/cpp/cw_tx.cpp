#include <mantis/mantis.h>
#include <fstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

static constexpr double TX_BUFF_SIZE = 16e5; // should be divisible by both 8 and 16
static constexpr double threshold = 1e-6;
static constexpr size_t INDICATOR_SLEEP_TIME = 5;

#define PASSES_THRESHOLD(a, b) (std::abs(a - b) < threshold)

static std::string program_description = "Transmit CW at specified parameters.";

int main(int argc, char **argv) {
    po::options_description desc(program_description + "\nOptions");
    desc.add_options()
            ("help,h", "produce help message")
            ("args", po::value<std::string>(), "device args str")
            ("freq", po::value<double>()->default_value(100e6, "100e6"), "Tx freq [Hz], will default to 100MHz")
            ("lo", po::value<double>(), "lo offset freq [Hz] By default will allow SDR lib to choose most fitting lo")
            ("rate", po::value<double>()->default_value(10e6, "10e6"), "Tx rate [Sps], will default to 10 MSps")
            ("gain", po::value<double>()->default_value(0.0), "Tx gain")
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
    auto [err, tx_channel] = d_manager.get_tx_channel(params, channel);
    if (!mantis::errors::succeeded(err)) {
        mantis::utils::perror("Failed to Acquire Channel: " + mantis::errors::mantis_errno(err));
    }

    /// configure channel
    double ac_freq = tx_channel->set_freq(freq, lo);
    if (!PASSES_THRESHOLD(ac_freq, freq)) {
        mantis::utils::pwarn(
            "Couldn't set freq to: " + std::to_string(freq) + ". Actual freq: " + std::to_string(ac_freq) + " Hz");
    }

    double ac_gain = tx_channel->set_gain(gain);
    if (!PASSES_THRESHOLD(ac_gain, gain)) {
        mantis::utils::pwarn(
            "Couldn't set gain to: " + std::to_string(gain) + ". Actual gain: " + std::to_string(ac_gain) + " dB");
    }

    double ac_rate = tx_channel->set_rate(rate);
    if (!PASSES_THRESHOLD(ac_rate, rate)) {
        mantis::utils::pwarn(
            "Couldn't set rate to: " + std::to_string(rate) + ". Actual rate: " + std::to_string(ac_rate) + " Sps");
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


    char *tx_buff = (char *) malloc(TX_BUFF_SIZE * sizeof(char));
    if (!tx_buff) {
        mantis::utils::perror("Failed to malloc Tx buffer");
        return EXIT_FAILURE;
    }

    // gen samples
    if (sample_size == 8) {
        auto data_ptr = reinterpret_cast<std::complex<float> *>(tx_buff);
        static std::complex<float> sample = {1, 0};
        for (int i = 0; i < TX_BUFF_SIZE / sample_size; i++) {
            data_ptr[i] = sample;
        }
    } else {
        auto *data_ptr = reinterpret_cast<std::complex<double> *>(tx_buff);
        static std::complex<double> sample = {1, 0};
        for (int i = 0; i < TX_BUFF_SIZE / sample_size; i++) {
            // gotta love strong typing
            data_ptr[i] = sample;
        }
    }

    bool running = true;
    auto indicator = std::thread([&running]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(INDICATOR_SLEEP_TIME));
            std::cout << "+" << std::flush;
        }
    });

    mantis::go::mtx_metadata tx_md{.start_of_burst = true, .end_of_burst = false};

    mantis::utils::pinfo("++++++Begin TX+++++");
    while (true) {
        if (!tx_channel->is_valid()) [[unlikely]] {
            mantis::utils::perror("SDR healthcheck Failed, channel invalid. Shutting Down...");
            running = false;
            indicator.join();
            return EXIT_FAILURE;
        }
        tx_channel->send(tx_buff, sample_size, TX_BUFF_SIZE / sample_size, tx_md);
    }
}
