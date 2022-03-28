# Stub file for C extension _cds
import typing as t


# dumper

def _create_archive(archive: str): ...


def _move_in(obj): ...


# loader

def _load_archive(archive: str): ...


def _get_obj(): ...


# utils

def _set_mode(mode: int): ...


class CDSException(RuntimeError): ...


flags: _flags


# noinspection PyPep8Naming
class _flags:
    mode: int
    verbose: int
