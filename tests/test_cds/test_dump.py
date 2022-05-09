import unittest
from pathlib import Path

from tests import CdsTestMixin
from . import CDSCase


class DumpTest(CdsTestMixin, unittest.TestCase):
    def test_trace(self):
        with CDSCase(self, self.NAME_LIST, self.TEST_ARCHIVE) as cds:
            cds.run_trace('import json')
            cds.verify_files(check_archive=False)

            cds.verify(lambda _: self.assertIn(
                'json',
                [line.strip() for line in Path(self.NAME_LIST).read_text().split('\n')]))

    def test_dump_archive_from_list(self):
        with open(self.NAME_LIST, 'w') as f:
            print('json', file=f)

        with CDSCase(self, self.NAME_LIST, self.TEST_ARCHIVE, clear_list=False) as cds:
            cds.run_dump()
