import unittest

from tests import CdsTestMixin, assert_name_list_created


class ImportTest(CdsTestMixin, unittest.TestCase):
    @assert_name_list_created
    def test_simple_trace(self):
        self.assert_python_source_ok(
            f"""
import cds
cds.trace("{self.NAME_LIST}")
import mod2.mod2_2
""",
            PYTHONPATH='tests/workspace')
