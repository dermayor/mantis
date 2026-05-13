"""
Mantis hardware metadata error codes and utilities
"""
from __future__ import annotations
import typing
__all__: list[str] = ['async_mtx_metadata_error_codes', 'mantis_errno', 'mrx_metadata_error_codes']
class async_mtx_metadata_error_codes:
    """
    Members:
    
      BURST_ACK
    
      UNDERFLOW
    
      SEQ_ERROR
    
      LATE
    
      UNDERFLOW_IN_PACKET
    
      SEQ_ERROR_IN_BURST
    
      USER_PAYLOAD
    """
    BURST_ACK: typing.ClassVar[async_mtx_metadata_error_codes]  # value = <async_mtx_metadata_error_codes.BURST_ACK: 1>
    LATE: typing.ClassVar[async_mtx_metadata_error_codes]  # value = <async_mtx_metadata_error_codes.LATE: 8>
    SEQ_ERROR: typing.ClassVar[async_mtx_metadata_error_codes]  # value = <async_mtx_metadata_error_codes.SEQ_ERROR: 4>
    SEQ_ERROR_IN_BURST: typing.ClassVar[async_mtx_metadata_error_codes]  # value = <async_mtx_metadata_error_codes.SEQ_ERROR_IN_BURST: 32>
    UNDERFLOW: typing.ClassVar[async_mtx_metadata_error_codes]  # value = <async_mtx_metadata_error_codes.UNDERFLOW: 2>
    UNDERFLOW_IN_PACKET: typing.ClassVar[async_mtx_metadata_error_codes]  # value = <async_mtx_metadata_error_codes.UNDERFLOW_IN_PACKET: 16>
    USER_PAYLOAD: typing.ClassVar[async_mtx_metadata_error_codes]  # value = <async_mtx_metadata_error_codes.USER_PAYLOAD: 64>
    __members__: typing.ClassVar[dict[str, async_mtx_metadata_error_codes]]  # value = {'BURST_ACK': <async_mtx_metadata_error_codes.BURST_ACK: 1>, 'UNDERFLOW': <async_mtx_metadata_error_codes.UNDERFLOW: 2>, 'SEQ_ERROR': <async_mtx_metadata_error_codes.SEQ_ERROR: 4>, 'LATE': <async_mtx_metadata_error_codes.LATE: 8>, 'UNDERFLOW_IN_PACKET': <async_mtx_metadata_error_codes.UNDERFLOW_IN_PACKET: 16>, 'SEQ_ERROR_IN_BURST': <async_mtx_metadata_error_codes.SEQ_ERROR_IN_BURST: 32>, 'USER_PAYLOAD': <async_mtx_metadata_error_codes.USER_PAYLOAD: 64>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __ge__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __gt__(self, other: typing.Any) -> bool:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __le__(self, other: typing.Any) -> bool:
        ...
    def __lt__(self, other: typing.Any) -> bool:
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
class mrx_metadata_error_codes:
    """
    Members:
    
      NO_ERROR
    
      TIMEOUT
    
      LATE
    
      BROKEN_CHAIN
    
      OVERFLOW
    
      ALIGNMENT
    
      BAD_PACKET
    """
    ALIGNMENT: typing.ClassVar[mrx_metadata_error_codes]  # value = <mrx_metadata_error_codes.ALIGNMENT: 12>
    BAD_PACKET: typing.ClassVar[mrx_metadata_error_codes]  # value = <mrx_metadata_error_codes.BAD_PACKET: 15>
    BROKEN_CHAIN: typing.ClassVar[mrx_metadata_error_codes]  # value = <mrx_metadata_error_codes.BROKEN_CHAIN: 4>
    LATE: typing.ClassVar[mrx_metadata_error_codes]  # value = <mrx_metadata_error_codes.LATE: 2>
    NO_ERROR: typing.ClassVar[mrx_metadata_error_codes]  # value = <mrx_metadata_error_codes.NO_ERROR: 0>
    OVERFLOW: typing.ClassVar[mrx_metadata_error_codes]  # value = <mrx_metadata_error_codes.OVERFLOW: 8>
    TIMEOUT: typing.ClassVar[mrx_metadata_error_codes]  # value = <mrx_metadata_error_codes.TIMEOUT: 1>
    __members__: typing.ClassVar[dict[str, mrx_metadata_error_codes]]  # value = {'NO_ERROR': <mrx_metadata_error_codes.NO_ERROR: 0>, 'TIMEOUT': <mrx_metadata_error_codes.TIMEOUT: 1>, 'LATE': <mrx_metadata_error_codes.LATE: 2>, 'BROKEN_CHAIN': <mrx_metadata_error_codes.BROKEN_CHAIN: 4>, 'OVERFLOW': <mrx_metadata_error_codes.OVERFLOW: 8>, 'ALIGNMENT': <mrx_metadata_error_codes.ALIGNMENT: 12>, 'BAD_PACKET': <mrx_metadata_error_codes.BAD_PACKET: 15>}
    def __eq__(self, other: typing.Any) -> bool:
        ...
    def __ge__(self, other: typing.Any) -> bool:
        ...
    def __getstate__(self) -> int:
        ...
    def __gt__(self, other: typing.Any) -> bool:
        ...
    def __hash__(self) -> int:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __le__(self, other: typing.Any) -> bool:
        ...
    def __lt__(self, other: typing.Any) -> bool:
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
@typing.overload
def mantis_errno(arg0: async_mtx_metadata_error_codes) -> str:
    """
    Get detailed error message for an async_mtx_metadata_error_code.
    """
@typing.overload
def mantis_errno(arg0: mrx_metadata_error_codes) -> str:
    """
    Get detailed error message for an mrx_metadata_error_code.
    """
