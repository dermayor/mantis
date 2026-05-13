#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <mantis/mantis.h>

static constexpr double BUFF_SIZE = 1e5;
static constexpr double threshold = 1e-6;

#define PASSES_THRESHOLD(a, b) (std::abs(a - b) < threshold)

namespace po = boost::program_options;

static std::string program_description = "Mantis Bin. Use --help/-h for options.";

int main(int argc, char** argv) {

    /// params and flags
    /// TODO: consider using a per channel struct for these when writing up multichannel maybe parse as a json and take
    /// channel files? could add a mantis gen file option to create editable default json for users. perhaps a seperate
    /// bin for this?
    std::string args;
    std::string filename;

    size_t channel_num;
    size_t reps;

    double gain;
    double rate;
    double freq;
    double mcr;

    bool find;
    bool tx_from_file;
    bool rx_to_file;
    bool cw;
    bool version;
    bool repeat;
    bool drivers_list;

    po::options_description desc(program_description + "\nOptions");
    auto opts = desc.add_options();

    // program modes
    opts("help,h", "produce help message");
    opts("find,f", po::bool_switch(&find)->default_value(false),
         "find connected devices. may be used with the 'args' option to find specific devices");
    opts("version,v", po::bool_switch(&version)->default_value(false), "print mantis version");
    opts("tx_from_file,t", po::bool_switch(&tx_from_file)->default_value(false), "transmit from file");
    opts("rx_to_file,r", po::bool_switch(&rx_to_file)->default_value(false), "receive to file");
    opts("cw,w", po::bool_switch(&cw)->default_value(false), "transmit a continuous wave");
    opts("drivers_list,d", po::bool_switch(&drivers_list)->default_value(false), "list available drivers with current build");
    
    // args
    opts("args,a", po::value<std::string>(&args)->default_value(""), "device args str. Leave empty to find all");
    opts("filename,F", po::value<std::string>(&filename)->default_value(""),
         "absolute path of file to transmit/receive from");
    opts("channel_num,c", po::value<size_t>(&channel_num)->default_value(0), "channel number for tx/rx. default 0");
    opts("reps,n", po::value<size_t>(&reps)->default_value(1),
         "Number of repetitions for tx_from_file. default 1. Alternatively, use --repeat/-R to repeat indefinitely");
    opts("repeat,R", po::bool_switch(&repeat)->default_value(false),
         "used with tx_from_file/t, repeat tx indefinetly. Alternatively, use --reps/-n to specify a number of "
         "repetitions");
    opts("freq", po::value<double>(&freq)->default_value(100e6, "100e6"), "Freq [Hz], will default to 100MHz");
    opts("lo", po::value<double>(), "lo offset freq [Hz] Will allow SDR lib to choose most fitting lo if not passed");
    opts("rate", po::value<double>(&rate)->default_value(10e6, "10e6"), "Rate [Sps], will default to 10 MSps");
    opts("gain", po::value<double>(&gain)->default_value(0.0, "0.0"), "Gain [dB], will default to 0 dB");
    opts("master_clock_rate,mcr", po::value<double>(&mcr),
         "master clock rate; can also be passed in args as master_clock_rate. This argument takes precedence");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch (const po::error &e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }

    po::notify(vm);

    if (version) {
        std::cout << MANTIS_VERSION << std::endl;
        return EXIT_SUCCESS;
    }

    if (drivers_list) {
        std::cout << (std::string(MANTIS_ACTIVE_DRIVERS).empty() ? "Virtual Only" : MANTIS_ACTIVE_DRIVERS) << std::endl;
        return EXIT_SUCCESS;
    }

    auto& d_manager = mantis::device_manager::get_instance();

    mantis::params::msdr_params params{};
    if (!args.empty()) {
        if (!mantis::errors::succeeded(mantis::params::msdr_params::from_str(args, params))) {
            throw mantis::runtime_error("Could not parse arg string: " + args);
        }
    }

    auto found_devices = d_manager.find(params);

    if (find) {
        if (found_devices.empty()) {
            std::cout << "No Devices Found\n";
            return EXIT_SUCCESS;
        }

        static int i = 0;
        for (const auto& device_params : found_devices) {
            std::cout << "=================================\n";
            std::cout << "Mantis Device " << std::to_string(i++) << "\n";
            std::cout << "=================================\n";
            std::cout << device_params << std::endl;
            std::cout << "\n\n";
        }

        return EXIT_SUCCESS;
    }

    if (tx_from_file + rx_to_file + cw > 1) {
        mantis::utils::perror("Cannot use --tx_from_file/-t, --rx_to_file/-r, or --cw/-w at the same time");
        return EXIT_FAILURE;
    }

    if (filename.empty() && (tx_from_file || rx_to_file)) {
        mantis::utils::perror(
            "File path must be provided with --filename/-F when using --tx_from_file/-t or --rx_to_file/-r");
        return EXIT_FAILURE;
    }

    int sample_size;
    if (mantis::config::CPU_SAMPLE_FORMAT == "fc32") {
        sample_size = 8;
    } else if (mantis::config::CPU_SAMPLE_FORMAT == "fc64") {
        // should also not be mantis api, consider compiling with diff library target for these
        sample_size = 16;
    } else {
        mantis::utils::perror("Unknown sample formant");
        return EXIT_FAILURE;
    }

    /// config lo
    std::optional<double> lo = (vm.count("lo") ? std::optional<double>{vm["lo"].as<double>()} : std::nullopt);

    /// alloc buffer
    char* buff = (char*)malloc(BUFF_SIZE * sizeof(char));
    if (!buff) {
        mantis::utils::perror("Failed to malloc samples buffer");
        return EXIT_FAILURE;
    }

    // init sdr TODO: get rid of the init and init in get_x_channel?
    if (!mantis::errors::succeeded(d_manager.init(1, params))) {
        mantis::utils::perror("Could not init relevant SDR");
        return EXIT_FAILURE;
    }

    /// tx from file/ cw
    if (tx_from_file || cw) {

        /// acquire channel
        auto [err, tx_channel] = d_manager.get_tx_channel(params, channel_num);
        if (!mantis::errors::succeeded(err)) {
            mantis::utils::perror("Failed to Acquire Channel: " + mantis::errors::mantis_errno(err));
            return EXIT_FAILURE;
        }

        /// configure channel
        double ac_freq = tx_channel->set_freq(freq, lo);
        if (!PASSES_THRESHOLD(ac_freq, freq)) {
            mantis::utils::pwarn("Couldn't set freq to: " + std::to_string(freq) +
                                 ". Actual freq: " + std::to_string(ac_freq) + " Hz");
        }

        double ac_gain = tx_channel->set_gain(gain);
        if (!PASSES_THRESHOLD(ac_gain, gain)) {
            mantis::utils::pwarn("Couldn't set gain to: " + std::to_string(gain) +
                                 ". Actual gain: " + std::to_string(ac_gain) + " dB");
        }

        double ac_rate = tx_channel->set_rate(rate);
        if (!PASSES_THRESHOLD(ac_rate, rate)) {
            mantis::utils::pwarn("Couldn't set rate to: " + std::to_string(rate) +
                                 ". Actual rate: " + std::to_string(ac_rate) + " Hz");
        }

        /// metadata
        mantis::go::mtx_metadata tx_md{.start_of_burst = true, .end_of_burst = false};

        if (cw) {

            // fill our buffer accordingly
            if (sample_size == 8) {
                auto data_ptr = reinterpret_cast<std::complex<float>*>(buff);
                static std::complex<float> sample = {1, 0};
                for (int i = 0; i < BUFF_SIZE / sample_size; i++) {
                    data_ptr[i] = sample;
                }
            } else {
                auto* data_ptr = reinterpret_cast<std::complex<double>*>(buff);
                static std::complex<double> sample = {1, 0};
                for (int i = 0; i < BUFF_SIZE / sample_size; i++) {
                    // gotta love strong typing
                    data_ptr[i] = sample;
                }
            }
            mantis::utils::pinfo("Starting CW Tx, will continue until stopped");
            while (true) {
                if (!tx_channel->is_valid()) [[unlikely]] {
                    mantis::utils::perror("SDR healthcheck Failed, channel invalid");
                    free(buff);
                    return EXIT_FAILURE;
                }
                tx_channel->send(buff, sample_size, BUFF_SIZE / sample_size, tx_md);
            }
        }

        /// try open file
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            mantis::utils::perror("Failed to open file: " + filename);
            return EXIT_FAILURE;
        }

        mantis::utils::pinfo("Starting Tx from File: " + filename);

        while (repeat || reps--) {
            while (file.read(buff, BUFF_SIZE) || file.gcount() > 0) {
                if (!tx_channel->is_valid()) [[unlikely]] {
                    mantis::utils::perror("SDR healthcheck Failed, channel invalid");
                    free(buff);
                    return EXIT_FAILURE;
                }
                tx_channel->send(buff, sample_size, BUFF_SIZE / sample_size, tx_md);
            }
            file.clear();
            file.seekg(0, std::ios::beg);
        }

        free(buff);

        mantis::utils::pinfo("Done");
        return EXIT_SUCCESS;
    }

    /// rx from file
    if (rx_to_file) {
        /// try opening file
        std::ofstream file(filename, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            mantis::utils::perror("Failed to open file: " + filename);
            return EXIT_FAILURE;
        }

        /// acquire channel
        auto [err, rx_channel] = d_manager.get_rx_channel(params, channel_num);
        if (!mantis::errors::succeeded(err)) {
            mantis::utils::perror("Failed to Acquire Rx Channel: " + mantis::errors::mantis_errno(err));
            return EXIT_FAILURE;
        }

        /// config channel
        double ac_freq = rx_channel->set_freq(freq, lo);
        if (!PASSES_THRESHOLD(ac_freq, freq)) {
            mantis::utils::pwarn("Couldn't set freq to: " + std::to_string(freq) +
                                 ". Actual freq: " + std::to_string(ac_freq) + " Hz");
        }

        double ac_gain = rx_channel->set_gain(gain);
        if (!PASSES_THRESHOLD(ac_gain, gain)) {
            mantis::utils::pwarn("Couldn't set gain to: " + std::to_string(gain) +
                                 ". Actual gain: " + std::to_string(ac_gain) + " dB");
        }

        double ac_rate = rx_channel->set_rate(rate);
        if (!PASSES_THRESHOLD(ac_rate, rate)) {
            mantis::utils::pwarn("Couldn't set rate to: " + std::to_string(rate) +
                                 ". Actual rate: " + std::to_string(ac_rate) + " Hz");
        }

        /// metadata
        mantis::go::mrx_metadata rx_md{.start_of_burst = true};

        mantis::utils::pinfo("Starting Rx to File: " + filename + ", will run until stopped");

        while (true) {
            if (!rx_channel->is_valid()) [[unlikely]] {
                mantis::utils::perror("SDR healthcheck Failed, channel invalid");
                free(buff);
                return EXIT_FAILURE;
            }

            rx_channel->receive(buff, sample_size, BUFF_SIZE / sample_size, rx_md);
            rx_md.has_time_spec = false;  // otherwise we rx in the past
            file.write(buff, BUFF_SIZE);
        }
    }

    return EXIT_SUCCESS;
}
