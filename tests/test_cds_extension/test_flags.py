import unittest

from tests import CdsTestMixin


class TestFlags(CdsTestMixin, unittest.TestCase):
    def test_set_mode(self):
        valid_mode = [2]
        valid_mode_cannot_be_set = [i for i in range(-1, 3) if i not in valid_mode]
        invalid_mode = [i for i in range(-1, 10) if i not in valid_mode and i not in valid_mode_cannot_be_set]
        for i in valid_mode:
            self.assert_python_source_ok(f'import _cds; _cds._set_mode({i})')
        for i in valid_mode_cannot_be_set:
            self.assert_python_source_ok(
                f"""
import pytest
import _cds
with pytest.raises(_cds.CDSException) as e:
    _cds._set_mode({i})
e.match("cds already initialized")
""")
        for i in invalid_mode:
            self.assert_python_source_ok(
                f"""
import pytest
import _cds
with pytest.raises(_cds.CDSException) as e:
    _cds._set_mode({i})
e.match("invalid mode")
""")

    def test_set_verbose(self):
        valid_verbosity = [0, 1, 2]
        invalid_verbosity = [i for i in range(-10, 10) if i not in valid_verbosity]

        for i in valid_verbosity:
            self.assert_python_source_ok(f'import _cds;'
                                         f'assert _cds.flags.verbose == 0;'
                                         f'_cds._set_verbose({i});'
                                         f'assert _cds.flags.verbose == {i}')

        for i in invalid_verbosity:
            self.assert_python_source_failure(f'import _cds; _cds._set_verbose({i})')
