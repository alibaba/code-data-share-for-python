import unittest

from tests import CdsTestMixin

from . import CDSCase


class CDSTest(CdsTestMixin, unittest.TestCase):
    def test_basic_cds(self):
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
