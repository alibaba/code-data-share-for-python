import unittest

from tests import CdsTestMixin
from . import CDSCase


class ImportTest(CdsTestMixin, unittest.TestCase):
    def test_simple_trace(self):
        with CDSCase(self, init_env={'PYTHONPATH': 'tests/workspace'}) as cds:
            cds.run_trace('import mod2.mod2_2')
            cds.verify_files(check_archive=False)
