from __future__ import annotations
import collections.abc
import typing
from . import metadata_errors
from . import utils
__all__: list[str] = ['async_mtx_metadata', 'device_manager', 'errors', 'metadata_errors', 'mrx_channel', 'mrx_metadata', 'msdr_params', 'mtx_channel', 'mtx_metadata', 'time_tag', 'utils']
class async_mtx_metadata:
    code: metadata_errors.async_mtx_metadata_error_codes
    has_time_spec: bool
    time_spec: time_tag
    def __init__(self) -> None:
        ...
    @property
    def channel(self) -> int:
        ...
    @channel.setter
    def channel(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class device_manager:
    @staticmethod
    def get_instance() -> device_manager:
        """
        Access the global singleton instance of the device manager.
        """
    def __repr__(self) -> str:
        ...
    def find(self, params: msdr_params) -> list[msdr_params]:
        """
        Returns a list of devices found on the machine.
        """
    def get_connected(self) -> list[msdr_params]:
        """
        Returns a list of params of devices currently being managed by mantis.
        """
    def get_rx_channel(self, msdr_params: msdr_params, channel_num: typing.SupportsInt | typing.SupportsIndex) -> tuple[errors.error_code, mrx_channel]:
        """
        Returns (error_code, mrx_channel) for the requested SDR and channel.
        """
    def get_tx_channel(self, msdr_params: msdr_params, channel_num: typing.SupportsInt | typing.SupportsIndex) -> tuple[errors.error_code, mtx_channel]:
        """
        Returns (error_code, mtx_channel) for the requested SDR and channel.
        """
    def init(self, num: typing.SupportsInt | typing.SupportsIndex, msdr_params: msdr_params) -> errors.error_code:
        """
        Connect and initialize a requested number of SDRs.
        """
    def init_all(self, msdr_params: msdr_params) -> errors.error_code:
        """
        Initialize all supported SDRs connected to the system.
        """
class errors:
    """
    Mantis Errors Class, Contains error codes and errno function
    """
    class error_code:
        """
        mantis error codes
        
        Members:
        
          SUCCESS
        
          NO_SDRS_CONNECTED
        
          NO_SDRS_FOUND
        
          INSUFFICIENT_SDRS_FOUND
        
          NO_MATCHING_SDR
        
          INVALID_SDR
        
          INVALID_MCR
        
          INVALID_ARGUMENT
        
          NO_TX_CHANNELS
        
          NO_RX_CHANNELS
        
          INSUFFICIENT_TX_CHANNELS
        
          INSUFFICIENT_RX_CHANNELS
        
          POWER_REF_UNAVAILABLE
        
          SOCKET_CONECTION_FAILURE
        
          TOTAL_SOCKET_SEND_FAILURE
        
          TOTAL_SOCKET_RECV_FAILURE
        
          PARTIAL_SOCKET_SEND_FAILURE
        
          PARTIAL_SOCKET_RECV_FAILURE
        
          FAILED_TO_ACCEPT_CLIENT
        
          FAILED_TO_GET_SOCKET_FD
        
          FAILED_TO_BIND
        
          FAILED_TO_LISTEN
        
          CHANNEL_IN_USE
        
          TIMEOUT
        
          INVALID_TIME_SOURCE
        """
        CHANNEL_IN_USE: typing.ClassVar[errors.error_code]  # value = <error_code.CHANNEL_IN_USE: 22>
        FAILED_TO_ACCEPT_CLIENT: typing.ClassVar[errors.error_code]  # value = <error_code.FAILED_TO_ACCEPT_CLIENT: 18>
        FAILED_TO_BIND: typing.ClassVar[errors.error_code]  # value = <error_code.FAILED_TO_BIND: 20>
        FAILED_TO_GET_SOCKET_FD: typing.ClassVar[errors.error_code]  # value = <error_code.FAILED_TO_GET_SOCKET_FD: 19>
        FAILED_TO_LISTEN: typing.ClassVar[errors.error_code]  # value = <error_code.FAILED_TO_LISTEN: 21>
        INSUFFICIENT_RX_CHANNELS: typing.ClassVar[errors.error_code]  # value = <error_code.INSUFFICIENT_RX_CHANNELS: 11>
        INSUFFICIENT_SDRS_FOUND: typing.ClassVar[errors.error_code]  # value = <error_code.INSUFFICIENT_SDRS_FOUND: 3>
        INSUFFICIENT_TX_CHANNELS: typing.ClassVar[errors.error_code]  # value = <error_code.INSUFFICIENT_TX_CHANNELS: 10>
        INVALID_ARGUMENT: typing.ClassVar[errors.error_code]  # value = <error_code.INVALID_ARGUMENT: 7>
        INVALID_MCR: typing.ClassVar[errors.error_code]  # value = <error_code.INVALID_MCR: 6>
        INVALID_SDR: typing.ClassVar[errors.error_code]  # value = <error_code.INVALID_SDR: 5>
        INVALID_TIME_SOURCE: typing.ClassVar[errors.error_code]  # value = <error_code.INVALID_TIME_SOURCE: 24>
        NO_MATCHING_SDR: typing.ClassVar[errors.error_code]  # value = <error_code.NO_MATCHING_SDR: 4>
        NO_RX_CHANNELS: typing.ClassVar[errors.error_code]  # value = <error_code.NO_RX_CHANNELS: 9>
        NO_SDRS_CONNECTED: typing.ClassVar[errors.error_code]  # value = <error_code.NO_SDRS_CONNECTED: 1>
        NO_SDRS_FOUND: typing.ClassVar[errors.error_code]  # value = <error_code.NO_SDRS_FOUND: 2>
        NO_TX_CHANNELS: typing.ClassVar[errors.error_code]  # value = <error_code.NO_TX_CHANNELS: 8>
        PARTIAL_SOCKET_RECV_FAILURE: typing.ClassVar[errors.error_code]  # value = <error_code.PARTIAL_SOCKET_RECV_FAILURE: 17>
        PARTIAL_SOCKET_SEND_FAILURE: typing.ClassVar[errors.error_code]  # value = <error_code.PARTIAL_SOCKET_SEND_FAILURE: 16>
        POWER_REF_UNAVAILABLE: typing.ClassVar[errors.error_code]  # value = <error_code.POWER_REF_UNAVAILABLE: 12>
        SOCKET_CONECTION_FAILURE: typing.ClassVar[errors.error_code]  # value = <error_code.SOCKET_CONECTION_FAILURE: 13>
        SUCCESS: typing.ClassVar[errors.error_code]  # value = <error_code.SUCCESS: 0>
        TIMEOUT: typing.ClassVar[errors.error_code]  # value = <error_code.TIMEOUT: 23>
        TOTAL_SOCKET_RECV_FAILURE: typing.ClassVar[errors.error_code]  # value = <error_code.TOTAL_SOCKET_RECV_FAILURE: 15>
        TOTAL_SOCKET_SEND_FAILURE: typing.ClassVar[errors.error_code]  # value = <error_code.TOTAL_SOCKET_SEND_FAILURE: 14>
        __members__: typing.ClassVar[dict[str, errors.error_code]]  # value = {'SUCCESS': <error_code.SUCCESS: 0>, 'NO_SDRS_CONNECTED': <error_code.NO_SDRS_CONNECTED: 1>, 'NO_SDRS_FOUND': <error_code.NO_SDRS_FOUND: 2>, 'INSUFFICIENT_SDRS_FOUND': <error_code.INSUFFICIENT_SDRS_FOUND: 3>, 'NO_MATCHING_SDR': <error_code.NO_MATCHING_SDR: 4>, 'INVALID_SDR': <error_code.INVALID_SDR: 5>, 'INVALID_MCR': <error_code.INVALID_MCR: 6>, 'INVALID_ARGUMENT': <error_code.INVALID_ARGUMENT: 7>, 'NO_TX_CHANNELS': <error_code.NO_TX_CHANNELS: 8>, 'NO_RX_CHANNELS': <error_code.NO_RX_CHANNELS: 9>, 'INSUFFICIENT_TX_CHANNELS': <error_code.INSUFFICIENT_TX_CHANNELS: 10>, 'INSUFFICIENT_RX_CHANNELS': <error_code.INSUFFICIENT_RX_CHANNELS: 11>, 'POWER_REF_UNAVAILABLE': <error_code.POWER_REF_UNAVAILABLE: 12>, 'SOCKET_CONECTION_FAILURE': <error_code.SOCKET_CONECTION_FAILURE: 13>, 'TOTAL_SOCKET_SEND_FAILURE': <error_code.TOTAL_SOCKET_SEND_FAILURE: 14>, 'TOTAL_SOCKET_RECV_FAILURE': <error_code.TOTAL_SOCKET_RECV_FAILURE: 15>, 'PARTIAL_SOCKET_SEND_FAILURE': <error_code.PARTIAL_SOCKET_SEND_FAILURE: 16>, 'PARTIAL_SOCKET_RECV_FAILURE': <error_code.PARTIAL_SOCKET_RECV_FAILURE: 17>, 'FAILED_TO_ACCEPT_CLIENT': <error_code.FAILED_TO_ACCEPT_CLIENT: 18>, 'FAILED_TO_GET_SOCKET_FD': <error_code.FAILED_TO_GET_SOCKET_FD: 19>, 'FAILED_TO_BIND': <error_code.FAILED_TO_BIND: 20>, 'FAILED_TO_LISTEN': <error_code.FAILED_TO_LISTEN: 21>, 'CHANNEL_IN_USE': <error_code.CHANNEL_IN_USE: 22>, 'TIMEOUT': <error_code.TIMEOUT: 23>, 'INVALID_TIME_SOURCE': <error_code.INVALID_TIME_SOURCE: 24>}
        def __eq__(self, other: typing.Any) -> bool:
            ...
        def __getstate__(self) -> int:
            ...
        def __hash__(self) -> int:
            ...
        def __index__(self) -> int:
            ...
        def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
            ...
        def __int__(self) -> int:
            ...
        def __ne__(self, other: typing.Any) -> bool:
            ...
        def __repr__(self) -> str:
            ...
        def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
            ...
        def __str__(self) -> str:
            ...
        @property
        def name(self) -> str:
            ...
        @property
        def value(self) -> int:
            ...
    @staticmethod
    def mantis_errno(arg0: errors.error_code) -> str:
        ...
class mrx_channel:
    def can_set_power_ref(self) -> bool:
        ...
    def get_channel_num(self) -> int:
        ...
    def get_freq(self) -> float:
        ...
    def get_gain(self) -> float:
        ...
    def get_params(self) -> msdr_params:
        ...
    def get_power_ref(self) -> tuple[float, errors.error_code]:
        ...
    def get_rate(self) -> float:
        ...
    def get_time_now(self) -> time_tag:
        ...
    def get_valid_sample_rates(self) -> list[float]:
        ...
    def healthcheck(self) -> bool:
        ...
    def invalidate(self) -> None:
        ...
    def is_valid(self) -> bool:
        ...
    def receive(self, buffer: collections.abc.Buffer, metadata: mrx_metadata) -> None:
        """
        receive(buffer: buffer, metadata: mantis.mrx_metadata) -> None
        """
    def set_clock_source(self, arg0: str) -> None:
        ...
    def set_freq(self, freq: typing.SupportsFloat | typing.SupportsIndex, lo: typing.SupportsFloat | typing.SupportsIndex | None = None) -> float:
        ...
    def set_gain(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> float:
        ...
    def set_power_ref(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> tuple[float, errors.error_code]:
        ...
    def set_rate(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> float:
        ...
    def set_time_source(self, arg0: str) -> None:
        ...
    def sync(self, time_stamp: time_tag) -> errors.error_code:
        ...
class mrx_metadata:
    code: metadata_errors.mrx_metadata_error_codes
    end_of_burst: bool
    has_time_spec: bool
    more_fragments: bool
    one_packet: bool
    out_of_sequence: bool
    start_of_burst: bool
    time_spec: time_tag
    def __init__(self) -> None:
        ...
    def get_eov_positions(self) -> list[int]:
        """
        Get EOV positions as a Python list.
        """
    def set_eov_positions(self, positions: collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex]) -> None:
        """
        Set EOV positions from a Python list.
        """
    def set_time_spec(self, time_spec: time_tag) -> None:
        """
        Sets the desired RX start time.
        """
    @property
    def fragment_offset(self) -> int:
        ...
    @fragment_offset.setter
    def fragment_offset(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def timeout(self) -> float:
        ...
    @timeout.setter
    def timeout(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class msdr_params:
    """
    struct that contains all the parameters needed to find, init, and configure an SDR.
    """
    addr: str
    clock_source: str
    fpga: str
    master_clock_rate: str
    name: str
    product: str
    recv_buff_size: str
    send_buff_size: str
    serial: str
    time_source: str
    type: str
    @staticmethod
    def compare(arg0: msdr_params, arg1: msdr_params) -> bool:
        ...
    @staticmethod
    def from_str(args: str, delimiter: str = ';') -> tuple[errors.error_code, msdr_params]:
        ...
    def __init__(self) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def get_config_args(self) -> str:
        ...
    def get_find_args(self) -> str:
        ...
class mtx_channel:
    def can_set_power_ref(self) -> bool:
        ...
    def get_channel_num(self) -> int:
        ...
    def get_freq(self) -> float:
        ...
    def get_gain(self) -> float:
        ...
    def get_params(self) -> msdr_params:
        ...
    def get_power_ref(self) -> tuple[float, errors.error_code]:
        ...
    def get_rate(self) -> float:
        ...
    def get_time_now(self) -> time_tag:
        ...
    def get_valid_sample_rates(self) -> list[float]:
        ...
    def healthcheck(self) -> bool:
        ...
    def invalidate(self) -> None:
        ...
    def is_valid(self) -> bool:
        ...
    def recv_async_message(self, message: async_mtx_metadata, timeout: typing.SupportsFloat | typing.SupportsIndex = 0.1) -> None:
        """
        recv_async_message(message: mantis.async_mtx_metadata, timeout: float = 0.1) -> None
        """
    def send(self, buffer: collections.abc.Buffer, metadata: mtx_metadata) -> None:
        """
        send(buffer: buffer, metadata: mantis.mtx_metadata) -> None
        """
    def set_clock_source(self, arg0: str) -> None:
        ...
    def set_freq(self, freq: typing.SupportsFloat | typing.SupportsIndex, lo: typing.SupportsFloat | typing.SupportsIndex | None = None) -> float:
        ...
    def set_gain(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> float:
        ...
    def set_power_ref(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> tuple[float, errors.error_code]:
        ...
    def set_rate(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> float:
        ...
    def set_time_source(self, arg0: str) -> None:
        ...
    def sync(self, time_stamp: time_tag) -> errors.error_code:
        ...
class mtx_metadata:
    end_of_burst: bool
    has_time_spec: bool
    start_of_burst: bool
    time_spec: time_tag
    def __init__(self) -> None:
        ...
    def set_time_spec(self, time_spec: time_tag) -> None:
        """
        Sets the transmission time and updates the has_time_spec flag.
        """
    @property
    def eov_positions(self) -> list[int]:
        ...
    @eov_positions.setter
    def eov_positions(self, arg1: collections.abc.Sequence[typing.SupportsInt | typing.SupportsIndex]) -> None:
        ...
    @property
    def timeout(self) -> float:
        ...
    @timeout.setter
    def timeout(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class time_tag:
    """
    Mantis time_tag class. class is split into full seconds and fractional for increased accuracy
    """
    def __add__(self, arg0: time_tag) -> time_tag:
        ...
    def __iadd__(self, arg0: time_tag) -> time_tag:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, full_secs: typing.SupportsInt | typing.SupportsIndex, frac_secs: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @typing.overload
    def __init__(self, full_secs: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __isub__(self, arg0: time_tag) -> time_tag:
        ...
    def __sub__(self, arg0: time_tag) -> time_tag:
        ...
    @property
    def frac_secs(self) -> float:
        ...
    @property
    def full_secs(self) -> int:
        ...
__version__: str = '0.0.3'
