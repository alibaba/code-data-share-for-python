import logging
import os
import random

from test.support.script_helper import assert_python_ok, assert_python_failure

import typing as t


def assert_python_source_ok(src, **env_vars):
    return assert_python_ok('-c', src, **env_vars)


def assert_python_source_failure(src, **env_vars):
    return assert_python_failure('-c', src, **env_vars)


class UtilMixin:
    exists = staticmethod(os.path.exists)
    assert_python_ok = staticmethod(assert_python_ok)
    assert_python_failure = staticmethod(assert_python_failure)
    assert_python_source_ok = staticmethod(assert_python_source_ok)
    assert_python_source_failure = staticmethod(assert_python_source_failure)

    assertExists = lambda self, file: self.assertTrue(self.exists(file), f'{file} should exists but not.')
    assertNotExists = lambda self, file: self.assertFalse(self.exists(file), f'{file} should not exists but present.')

    @staticmethod
    def hook_installed():
        try:
            assert_python_source_ok('import sys; assert "cds" in sys.modules')
            return True
        except AssertionError:
            return False


class CdsTestMixin(UtilMixin):
    TEST_ARCHIVE = 'test.img'
    NAME_LIST = 'test.lst'

    def _del_archive(self):
        if os.path.exists(self.TEST_ARCHIVE):
            os.remove(self.TEST_ARCHIVE)
        if os.path.exists(self.NAME_LIST):
            os.remove(self.NAME_LIST)

    setUp = _del_archive
    tearDown = _del_archive


def random_branch():
    return random.choice([True, False])


def random_float():
    f = random.random()
    if f == 0.0:
        return f
    if random_branch():
        f = 1 / f
    return f


def is_shared(address: t.Union[str, int]):
    if isinstance(address, int):
        address = hex(address)
    return len(address) == len('0x280000000') and address.startswith('0x28')


logging.getLogger().setLevel(logging.DEBUG)
