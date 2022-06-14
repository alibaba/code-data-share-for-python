import random
import string
import unittest
import sys

from tests import CdsTestMixin, random_float, is_shared

from . import assert_archive_created

# From https://stackoverflow.com/a/21666621
include_ranges = [
    (0x0021, 0x0021),
    (0x0023, 0x0026),
    (0x0028, 0x007E),
    (0x00A1, 0x00AC),
    (0x00AE, 0x00FF),
    (0x0100, 0x017F),
    (0x0180, 0x024F),
    (0x2C60, 0x2C7F),
    (0x16A0, 0x16F0),
    (0x0370, 0x0377),
    (0x037A, 0x037E),
    (0x0384, 0x038A),
    (0x038C, 0x038C),
]
unicode_chars = [chr(c) for (start, end) in include_ranges for c in range(start, end + 1)]

# only string literals contain [a-zA-Z0-9_]* will be interned, see `intern_string_constants`
internable_chars = string.ascii_letters + string.digits + '_'


class ShareObjectTestMixin(CdsTestMixin):
    def run_serialize_test(self, value, oracle=None):
        s = repr(value)
        if oracle is None:
            oracle = s

        self.assert_python_source_ok(
            'import _cds;'
            f'_cds._create_archive("{self.TEST_ARCHIVE}");'
            f'_cds._move_in({s})',
            PYCDSMODE='MANUALLY',
        )

        out = self.assert_python_source_ok(
            'import _cds;'
            f'_cds._load_archive("{self.TEST_ARCHIVE}");'
            'print(repr(_cds._get_obj()))',
            PYCDSMODE='MANUALLY',
        )
        self.assertEqual(oracle, out.out.decode().strip())


class ShareObjectTest(ShareObjectTestMixin, unittest.TestCase):
    @assert_archive_created
    def test_base(self):
        self.run_serialize_test(None)
        self.run_serialize_test(True)
        self.run_serialize_test(False)
        self.run_serialize_test(...)

    @assert_archive_created
    def test_bytes(self):
        self.run_serialize_test(''.join(random.choice(string.printable) for _ in range(10000)).encode())

    @assert_archive_created
    def test_long(self):
        for i in (0, 1, 10, 100, 2 ** 100):
            self.run_serialize_test(i)
            if i != 0:
                self.run_serialize_test(-i)

    @assert_archive_created
    def test_float(self):
        self.run_serialize_test(0.0)
        self.run_serialize_test(-0.0)
        for _ in range(10):
            r = random_float()
            self.run_serialize_test(r)
            self.run_serialize_test(-r)

    @assert_archive_created
    def test_complex(self):
        for _ in range(10):
            self.run_serialize_test(random_float() + random_float() * 1j)

    @assert_archive_created
    def test_string_ascii(self):
        self.run_serialize_test("")
        self.run_serialize_test(string.printable)
        self.run_serialize_test(''.join(random.choice(string.printable) for _ in range(10000)))

    @assert_archive_created
    def test_string_unicode(self):
        for _ in range(10):
            self.run_serialize_test(''.join(random.choice(unicode_chars) for _ in range(10000)))

    @assert_archive_created
    def test_tuple(self):
        self.run_serialize_test(())
        self.run_serialize_test((1,))

    @assert_archive_created
    def test_deep_tuple(self):
        t = (1, b'2', '3')
        # The parser before "PEP 617 - New PEG parser for CPython" is weaker,
        # we need to avoid parser stack overflow below 3.9
        if sys.version_info > (3, 9):  # version > 3.9, include 3.9
            loop_depth = 100
        else:
            loop_depth = 70

        for i in range(loop_depth):
            t = (t, i, b'1')
        self.run_serialize_test(t)


class ShareCodeTest(CdsTestMixin, unittest.TestCase):
    @assert_archive_created
    def test_archive_lambda(self):
        def test_archive_code(o):
            r = repr(o)

            self.assert_python_source_ok(
                'import _cds;'
                f'_cds._create_archive("{self.TEST_ARCHIVE}");'
                f'_cds._move_in((lambda: print(repr({r}))).__code__)',
                PYCDSMODE='MANUALLY',
            )

            out = self.assert_python_source_ok(
                'import _cds;'
                f'_cds._load_archive("{self.TEST_ARCHIVE}");'
                'exec(_cds._get_obj())',
                PYCDSMODE='MANUALLY',
            )

            self.assertEqual(r, out.out.decode().strip())

        test_archive_code("a" * 100)
        test_archive_code(0)
        test_archive_code(b'')

    @assert_archive_created
    @unittest.skipUnless(sys.version_info.minor >= 11, 'test adaptive interpreter since 11')
    def test_dis_show_caches(self):
        def test_archive_code(o):
            r = repr(o)

            orig_dis_out = self.assert_python_source_ok(
                'import _cds;'
                f'_cds._create_archive("{self.TEST_ARCHIVE}");'
                f'c = (lambda: print(repr({r}))).__code__;'
                f'_cds._move_in(c);'
                f'import dis;'
                f'dis.dis(c);'
                f'dis.dis(c, show_caches=True)',
                PYCDSMODE='MANUALLY',
            )

            archived_dis_out = self.assert_python_source_ok(
                'import _cds;'
                f'_cds._load_archive("{self.TEST_ARCHIVE}");'
                f'c = _cds._get_obj();'
                f'import dis;'
                'dis.dis(c);'
                'dis.dis(c, show_caches=True)',
                PYCDSMODE='MANUALLY',
            )
            self.assertEqual(orig_dis_out.out.decode(), archived_dis_out.out.decode())

        test_archive_code("a" * 100)
        test_archive_code(0)
        test_archive_code(b'')


class InternStringTest(CdsTestMixin, unittest.TestCase):
    def create_archive_with_string(self, s, interned: bool):
        self.assert_python_source_ok(
            'import _cds, sys;'
            f'_cds._create_archive("{self.TEST_ARCHIVE}");'
            f's = {repr(s)};'
            f'{"sys.intern(s);" if interned else ""}'
            '_cds._move_in(s)',
            PYCDSMODE='MANUALLY',
        )

    def get_string_ids(self, s):
        _code, out, _err = self.assert_python_source_ok(
            'import _cds;'
            f'_cds._load_archive("{self.TEST_ARCHIVE}");'
            f's1 = _cds._get_obj(); s2 = eval({repr(repr(s))});'
            'print(repr((hex(id(s1)), hex(id(s2)))))',
            PYCDSMODE='MANUALLY',
        )
        return eval(out)

    def run_check(self, s, interned, should_eq, should_be_in_shared):
        self.create_archive_with_string(s, interned=interned)
        id_heap, id_lit = self.get_string_ids(s)
        if should_eq:
            self.assertEqual(id_heap, id_lit, s)
        if should_be_in_shared:
            self.assertTrue(is_shared(id_heap), id_heap)

    @assert_archive_created
    def test_share_explicitly_interned_str(self):
        for _ in range(10):
            s = ''.join(random.choice(internable_chars) for _ in range(100))
            self.run_check(s, True, True, True)
            self.run_check(s, False, False, True)

    @assert_archive_created
    def test_share_already_interned_str(self):
        for already_interned_str in ('sys', 'os'):
            self.run_check(already_interned_str, True, True, False)
            self.run_check(already_interned_str, False, True, False)
