"""
mantis utility functions
"""
from __future__ import annotations
__all__: list[str] = ['get_sample_size', 'perror', 'pinfo', 'pwarn']
def get_sample_size() -> int:
    """
    Get the current sample size in bytes according to the CPU_SAMPLE_FORMAT config variable. Will throw a runtime error if the CPU_SAMPLE_FORMAT is not supported.
    """
def perror(arg0: str) -> None:
    """
    Print an error message with mantis formatting
    """
def pinfo(arg0: str) -> None:
    """
    Print an info message with mantis formatting
    """
def pwarn(arg0: str) -> None:
    """
    Print a warning message with mantis formatting
    """
