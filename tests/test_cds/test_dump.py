import unittest
from pathlib import Path

from tests import CdsTestMixin, assert_archive_created, assert_name_list_created


class DumpTest(CdsTestMixin, unittest.TestCase):
    @assert_name_list_created
    def test_trace(self):
        self.assert_python_source_ok(f'import cds; cds.trace("{self.NAME_LIST}"); import json')

        self.assertIn('json', [line.strip() for line in Path(self.NAME_LIST).read_text().split('\n')])

    @assert_name_list_created
    @assert_archive_created
    def test_dump_archive_from_list(self):
        with open(self.NAME_LIST, 'w') as f:
            print('json', file=f)

        self.assert_python_ok('-m', 'cds.dump', self.NAME_LIST, self.TEST_ARCHIVE)
