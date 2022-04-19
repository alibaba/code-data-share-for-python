import unittest

from tests import CdsTestMixin, assert_archive_created, assert_name_list_created


class CDSTest(CdsTestMixin, unittest.TestCase):
    @assert_name_list_created
    @assert_archive_created
    def test_basic_cds(self):
        self.assert_python_source_ok(f"""
import cds
cds.trace('{self.NAME_LIST}')
import enum
""")
        self.assert_python_source_ok(f"""
import cds.dump
cds.dump.run_dump('{self.NAME_LIST}', '{self.TEST_ARCHIVE}')
""")
        _, _, err = self.assert_python_source_ok(f"""
import cds
cds._cds._set_verbose(2)
cds.share('{self.TEST_ARCHIVE}')
import enum
""")
        self.assertIn("exec_module cached 'enum'", err.decode())

    @assert_name_list_created
    @assert_archive_created
    def test_basic_cds2(self):
        with open(self.NAME_LIST, 'w') as f:
            print('enum', file=f)
        self.assert_python_source_ok(f"""
import cds.dump
cds.dump.run_dump('{self.NAME_LIST}', '{self.TEST_ARCHIVE}')
""")
        self.assert_python_source_ok('import enum', PYCDSMODE='SHARE', PYCDSARCHIVE=self.TEST_ARCHIVE)

    # a fatal error only happened in venv
    @assert_name_list_created
    @assert_archive_created
    def test_basic_collections(self):
        with open(self.NAME_LIST, 'w') as f:
            print('collections', file=f)
        self.assert_python_source_ok(f"""
import cds.dump
cds.dump.run_dump('{self.NAME_LIST}', '{self.TEST_ARCHIVE}')
""")
        self.assert_python_source_ok('import collections', PYCDSMODE='SHARE', PYCDSARCHIVE=self.TEST_ARCHIVE)
