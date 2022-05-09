import unittest

from tests import CdsTestMixin

from . import assert_archive_created


class ArchiveTest(CdsTestMixin, unittest.TestCase):
    @assert_archive_created
    def test_create_archive(self):
        self.assert_python_source_ok(f'import _cds; _cds._create_archive("{self.TEST_ARCHIVE}")', PYCDSMODE='MANUALLY')
        self.assert_python_source_ok(f'import _cds; _cds._load_archive("{self.TEST_ARCHIVE}")', PYCDSMODE='MANUALLY')

    @assert_archive_created
    def test_create_multiple_archive(self):
        self.assert_python_source_ok(
            f"""
import _cds
import pytest
with pytest.raises(_cds.CDSException) as e:
    _cds._create_archive("{self.TEST_ARCHIVE}")
    _cds._create_archive("{self.TEST_ARCHIVE}")
e.match("cds already initialized.")
""", PYCDSMODE='MANUALLY')

    @assert_archive_created
    def test_dump_not_supported_type(self):
        self.assert_python_source_ok(
            f"""
import pytest
import _cds
with pytest.raises(_cds.CDSException) as e:
    _cds._create_archive("{self.TEST_ARCHIVE}")
    _cds._move_in([])
e.match("Trying to move in a list object.")
""")

    def test_load_non_exists_archive(self):
        self.assert_python_source_ok(
            f"""
import pytest
import _cds
with pytest.raises(_cds.CDSException) as e:
    _cds._load_archive("{self.TEST_ARCHIVE}")
e.match("open mmap file failed.")
""", PYCDSMODE='MANUALLY')

    @assert_archive_created
    def test_load_null(self):
        self.assert_python_source_ok(
            f'import _cds; _cds._create_archive("{self.TEST_ARCHIVE}")')

        self.assert_python_source_ok(
            f"""
import pytest
import _cds
_cds._load_archive("{self.TEST_ARCHIVE}")
with pytest.raises(_cds.CDSException) as e:
    _cds._get_obj()
e.match("No object in heap.")
""", PYCDSMODE='MANUALLY')

    @assert_archive_created
    def test_loaded_object(self):
        self.assert_python_source_ok(
            f'import _cds; _cds._create_archive("{self.TEST_ARCHIVE}"); _cds._move_in((1, 2, 3))', PYCDSMODE='MANUALLY')

        self.assert_python_source_ok(
            f"""
import _cds
_cds._load_archive("{self.TEST_ARCHIVE}")
t1 = _cds._get_obj()
t2 = _cds._get_obj()
assert t1 == t2
assert id(t1) == id(t2)
""", PYCDSMODE='MANUALLY')
