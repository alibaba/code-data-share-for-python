import functools
import unittest

import typing as t

from .. import CdsTestMixin


def assert_archive_created(f: t.Callable):
    @functools.wraps(f)
    def inner(self: unittest.TestCase):
        self.assertIsInstance(self, CdsTestMixin)
        self.assertNotExists(self.TEST_ARCHIVE)

        f(self)

        self.assertExists(self.TEST_ARCHIVE)

    return inner
