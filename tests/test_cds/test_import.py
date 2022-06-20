import unittest

from tests import CdsTestMixin
from . import CDSCase


class ImportTest(CdsTestMixin, unittest.TestCase):
    def test_simple_trace(self):
        with CDSCase(self, init_env={'PYTHONPATH': 'tests/workspace'}) as cds:
            cds.run_trace('import mod2.mod2_2')
            cds.run_dump()
            cds.run_share('import mod2.mod2_2', verbose=2, save_output=True)

            cds.verify(lambda outputs: self.assertIn("exec_module cached 'mod2.mod2_2'", outputs[0][2], outputs))

    def test_call(self):
        with CDSCase(self, init_env={'PYTHONPATH': 'tests/workspace'}) as cds:
            cds.run_trace('import mod1')
            cds.run_dump()
            cds.run_share('import mod1')
