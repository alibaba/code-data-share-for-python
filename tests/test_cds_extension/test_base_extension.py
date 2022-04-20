import unittest

from tests import CdsTestMixin


class BaseTest(CdsTestMixin, unittest.TestCase):
    def test_methods(self):
        _, out, _ = self.assert_python_source_ok('import _cds; print(repr(dir(_cds)))')

        self.assertEqual(set([i for i in eval(out) if not (i.startswith('__') and i.endswith('__'))]), {
            '_create_archive',
            '_load_archive',
            '_move_in',
            '_get_obj',
            '_set_mode',
            '_set_verbose',
            '_get_initialized',
            'CDSException',
            'flags',
        })
