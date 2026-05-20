# Mantis

SDR Driver Abstraction Library

## Library Install Instructions

### Ubuntu

For full install (c++ and python) run the terminal command

```bash

curl -fsSL https://raw.githubusercontent.com/MayorDermer/mantis/refs/heads/main/scripts/install.sh | bash

```

## Steps for adding support for another driver

1. Add support in the project's cmake
    - Find the relevant driver package
    - If the package is found:
        - add a compile flag indicating so (example M_UHD)
        - append the driver library to the variable MANTIS_DRIVER_LIBS var
        - append the src/driver_impl/<new_driver>/*.cpp files to the SOURCE_FILES var
        - add an install() function in the cmake that installs the include/driver_impl/<new_driver> directory to
          include/mantis/driver_impl/

2. Create 2 new directories:
    - include/driver_impl/<new_driver>/ and src/dirver_impl/<new_driver> where you will store your .h and .cpp files
      respectively

3. Implement the interfaces found in include/lib/driver_specific/interfaces and metadata conversion functions for your
   new driver. This can be done however you like, but it is recommended you use the uhd implementation as a reference

4. Add your <new_driver>_device_finder to the device_managers init_internal and find functions. Use the uhd version as
   an example.

5.Your new driver should be ready to go!

## Virtual Functionality

- Mantis includes support for virtual sdrs. This can be enabled/disabled/forced at run time using the ALLOW_VIRTUAL and
  FORCE_VIRTUAL flags in the mantis-config.json file
- Virtual sdrs operate in 2 modes: file and socket. It is important to note that file always overrides, so if you input
  both a file path and a network host the channel will default to file mode
- File Behavior:
  - Tx: Will open file (params.virtual_msdr_params.filename (or BASE_FILENAME_tx_channel_<channel_num>_init_<
      curr_sdr_init>.FILE_EXTENSION
  - Rx: Will open file (params.virtual_msdr_params.filename (or BASE_FILENAME_rx_channel_<channel_num>_init_<
      curr_sdr_init>.FILE_EXTENSION. It is important that this file exists BEFORE the system starts up
- Socket Behavior:
  - Tx/Rx: will open a socket of type SOCKET_MODE and attempt to bind/connect _AT CHANNEL CONSTRUCTION TIME_ on DEFAULT_VIRTUAL_HOST at port DEFAULT_VIRTUAL_PORT++, these can be overridden with the params.socket_params fields before channel creation  

## Installed Binaries

A full library install will install the "mantis" binary for your convenience.

Functionality includes finding devices, tx/rx from/to a file, and transmitting a cw.
Run ```mantis --help``` for more details.

## Examples

Examples in both c++ and python can be found in the examples dir

## Config File

Mantis has a JSON config file, mantis-config.json which installs to ~/.config/mantis/
Users may define a different config file path with the MANTIS_CONFIG_FILE env var

### Options

- ALLOW_VIRTUAL (bool): allow use of virtual msdrs
- FORCE_VIRTUAL (bool): force the use of virtual msdrs even when physical sdrs are attached
- USE_CONFIG_MSDRS (bool): allow the use of the msdrs defined in the VIRTUAL_MSDRS section of the json (detailed bellow)
- DEFAULT_VIRTUAL_TX_MODE (str, "SOCKET" or "FILE"): defines default virtual tx channel mode
- DEFAULT_VIRTUAL_RX_MODE (str, "SOCKET" or "FILE"): defines default virtual tx channel mode
- DEFAULT_VIRTUAL_HOST (str, ip -- i.e. "0.0.0.0"): defines default host for virtual rx/tx socket channels
- BASE_FILENAME (str): the base filename for all default virtual rx/tx channels. If filename is not provided in the
  params themselves, file format is BASE_FILENAME_<rx/tx>\_channel_\<channe_num>\_init_\<sdr_init>\.FILE_EXTENSION
- DEFAULT_VIRTUAL_PORT (int): default port for rx/tx socket channels, will increment by 1 after use. i.e. channel 1
  DEFAULT_VIRTUAL_PORT, channel 2 DEFAULT_VIRTUAL_PORT + 1, etc...
- FILE_EXTENSION (str): file extension, i.e. ".32fc", ".bin"
- MONITOR_THREAD_SLEEP_TIME (int): defines sleep time for sdr healthcheck thread in milliseconds
- RECONNECTION_THREAD_SLEEP_TIME (int): defines sleep time for sdr reconnection thread in milliseconds (unused as of the
  time of writing)
- CPU_SAMPLE_FORMAT (str, "fc32" or "fc64"): defines working sample format fc32 for complex float, fc64 for complex
  double
- OTW_SDR_FORMAT (str): defines the over the wire data format, i.e. "sc16"
- SOCKET_TYPE (int, 1 or 2): defines the socket type. 1 for TCP, 2 for UDP
- VIRTUAL_MSDRS (list of jsons): declare custom virtual sdrs. These will be created at device_manager startup. more
  details bellow

#### VIRTUAL_MSDRS

List of jsons defining custom virtual sdrs
each sdr json may contain:

- NAME
- TYPE
- SERIAL
- PRODUCT
- ADDR
- MASTER_CLOCK_RATE
- SEND_BUFF_SIZE
- RECV_BUFF_SIZE
- CLOCK_SOURCE
- TIME_SOURCE
- FPGA

- 2 arrays of channels
  - TX_CHANNELS
  - RX_CHANNELS

Each channel may contain:

- MODE (str, "FILE" or "SOCKET"): channel type
- FILENAME (str): full filename to use
- HOST (str, ip -- i.e. "0.0.0.0"): channel host (if in socket mode)
- PORT (int): channel port (if in socket mode)
