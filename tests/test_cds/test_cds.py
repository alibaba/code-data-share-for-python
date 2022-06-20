import re
import sys
import unittest

from tests import CdsTestMixin

from . import CDSCase


class CDSTest(CdsTestMixin, unittest.TestCase):
    def test_basic_cds1(self):
        with CDSCase(self, self.NAME_LIST, self.TEST_ARCHIVE) as cds:
            cds.run_trace('import enum')
            cds.run_dump()
            cds.verify_files()
            cds.run_share('import enum', verbose=2, save_output=True)

            cds.verify(lambda outputs: self.assertIn("exec_module cached 'enum'", outputs[0][2], outputs))

    def test_basic_cds2(self):
        with open(self.NAME_LIST, 'w') as f:
            print('enum', file=f)

        with CDSCase(self, self.NAME_LIST, self.TEST_ARCHIVE, clear_list=False) as cds:
            cds.run_dump()
            cds.verify_files()
            cds.run_share('import enum')

    def test_basic_collections(self):
        with open(self.NAME_LIST, 'w') as f:
            print('collections', file=f)

        with CDSCase(self, self.NAME_LIST, self.TEST_ARCHIVE, clear_list=False) as cds:
            cds.run_dump()
            cds.verify_files()
            cds.run_share('import collections')

    def test_basic_datetime(self):
        with open(self.NAME_LIST, 'w') as f:
            print('datetime', file=f)

        with CDSCase(self, self.NAME_LIST, self.TEST_ARCHIVE, clear_list=False) as cds:
            cds.run_dump()
            cds.verify_files()
            cds.run_share('import datetime')

    @unittest.skipUnless(sys.version_info.minor >= 11, 'test adaptive interpreter since 11')
    def test_deopt_caches(self):
        """
        This test intends to test if all CACHE opcode is de-optimized correctly (aka. set oparg to 0).
        However, it seems the python module `dis` always return 0 for CACHE ops.
        So this test is not working as expected now, leave it here for future.
        """

        with open(self.NAME_LIST, 'w') as f:
            print('collections', file=f)

        def check_deopt_verifier(_):
            out = self.assert_python_source_ok(
                f'import _cds;'
                f'_cds._load_archive("{self.TEST_ARCHIVE}");'
                f'd = _cds._get_obj();'
                f'import dis;'
                f'[dis.dis(i[1][3], show_caches=True) for i in d]', PYCDSMODE='MANUALLY')

            dis_out: str = out.out.decode()
            dis_lines = [i for i in dis_out.split('\n') if i and 'CACHE' in i]
            for i in dis_lines:
                m = re.search(r'CACHE\s+0', i)
                self.assertIsNotNone(m, i)

        with CDSCase(self, self.NAME_LIST, self.TEST_ARCHIVE, clear_list=False) as cds:
            cds.run_dump()
            cds.verify_files()

            cds.verify(check_deopt_verifier)
