import mantis
from mantis import errors as me
from mantis import utils as mu
import argparse
import numpy as np

THRESHOLD = 1e-6
TX_BUFF_SIZE = 16e5


def passes_threshold(val_1, val_2):
    return abs(val_1 - val_2) < THRESHOLD


def main():
    parser = argparse.ArgumentParser(description="Transmit continuous cw")
    parser.add_argument(
        "-a",
        "--args",
        type=str,
        default="",
        help="device args str. Leave empty to find all",
    )
    parser.add_argument("--freq", type=float, default=100e6, help="frequency in [Hz]")
    parser.add_argument("--gain", type=float, default=0, help="gain in [dB]")
    parser.add_argument("--rate", type=float, default=10e6, help="Tx rate in [Sps]")
    parser.add_argument(
        "--lo",
        type=float,
        default=None,
        help="lo offset freq [Hz] By default will allow SDR lib to choose most fitting lo",
    )
    parser.add_argument("-c", "--channel_num", type=int, default=0, help="channel num")
    parser.add_argument(
        "-mcr",
        "--master_clock_rate",
        type=str,
        default=None,
        help="master clock rate; can also be passed in args as master_clock_rate. This argument takes precedence",
    )

    args = parser.parse_args()

    d_manager = mantis.device_manager.get_instance()

    err, params = mantis.msdr_params.from_str(args.args)
    if err != me.error_code.SUCCESS:
        mu.perror(
            "Failed to parse msdr_params from args string: {} -- {}".format(
                args.args, me.mantis_errno(err)
            )
        )
        return

    if args.master_clock_rate is not None:
        params.master_clock_rate = args.master_clock_rate

    freq = args.freq
    gain = args.gain
    rate = args.rate
    channel_num = args.channel_num
    lo = args.lo

    # init sdr
    err = d_manager.init(1, params)
    if err != me.error_code.SUCCESS:
        mu.perror("Could not init relevant SDR: " + me.mantis_errno(err))
        return

    # acquire channel
    err, tx_channel = d_manager.get_tx_channel(params, channel_num)
    if err != me.error_code.SUCCESS:
        mu.perror("Failed to acquire TX channel: " + me.mantis_errno(err))
        return

    # configure channel
    ac_freq = tx_channel.set_freq(freq, lo)
    if not passes_threshold(ac_freq, freq):
        mu.pwarn(
            "Couldn't set freq to: "
            + str(freq)
            + ". Actual freq: "
            + str(ac_freq)
            + " Hz"
        )

    ac_gain = tx_channel.set_gain(gain)
    if not passes_threshold(ac_gain, gain):
        mu.pwarn(
            "Couldn't set gain to: "
            + str(gain)
            + ". Actual gain: "
            + str(ac_gain)
            + " dB"
        )
    ac_rate = tx_channel.set_rate(rate)
    if not passes_threshold(ac_rate, rate):
        mu.pwarn(
            "Couldn't set rate to: "
            + str(rate)
            + ". Actual rate: "
            + str(ac_rate)
            + " Sps"
        )

    sample_size = mu.get_sample_size()

    num_samples = int(TX_BUFF_SIZE // sample_size)

    cw = np.ones(num_samples, dtype=np.complex64)

    tx_md = mantis.mtx_metadata()
    tx_md.start_of_burst = True
    tx_md.end_of_burst = False

    mu.pinfo("Starting CW Tx, will continue until stopped")
    while True:
        if not tx_channel.is_valid():
            mu.perror(
                "SDR healthcheck failed, channel is no longer valid. shutting down..."
            )
            break
        tx_channel.send(cw, tx_md)


if __name__ == "__main__":
    main()
