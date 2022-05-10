import logging
import os
import unittest

import typing as t
from functools import partial

from tests import assert_python_source_ok


class CDSCase:
    """
    This is used to describe tests for the automatically CDS mode (trace, dump, share).

    CDS can be used via two mechanisms:
    1. PYCDSMODE={TRACE, SHARE}, and PYCDSLIST / PYCDSARCHIVE;
    2. PYCDSMODE=MANUALLY, and run `cds.trace(<list>)` / `cds.share(<archive>)`.
    """

    def __init__(self, test: unittest.TestCase,
                 package_list='test.lst', archive='test.img', clear_list=True, clear_archive=True,
                 default_verbose=0,
                 init_env=None,
                 ):
        self.test = test

        self.logger = logging.getLogger(self.test._testMethodName)

        self.list = package_list
        self.archive = archive
        self.clear = (clear_list, clear_archive)

        self.default_verbose = default_verbose

        self._seq = []
        self._res = []

        self.init_env = init_env if init_env is not None else {}

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        for is_manually in (True, False):
            self._res.clear()
            if self.clear[0] and os.path.exists(self.list):
                os.remove(self.list)
            if self.clear[1] and os.path.exists(self.archive):
                os.remove(self.archive)
            self.logger.info(f'Running {"manual" if is_manually else "auto"} mode.')

            for step in self._seq:
                try:
                    self._run_step(**step, manually=is_manually)
                except Exception as e:
                    self.logger.exception(e)
                    raise

    def _run_step(self, action, program=None, verbose=None, manually=False, save_output=False, **kwargs):
        if action == 'verify':
            kwargs['verifier'](self._res)
            return

        env = {}
        env.update(self.init_env)
        if verbose is None and self.default_verbose:
            verbose = self.default_verbose
        if verbose is not None:
            env['PYCDSVERBOSE'] = str(verbose)

        if action in ('trace', 'share'):
            if manually:
                env.pop('PYCDSVERBOSE', None)
                prefix = None
                if action == 'trace':
                    prefix = f'import cds; cds.trace("{self.list}"); '
                elif action == 'share':
                    prefix = f'import cds; cds.share("{self.archive}"); '

                if verbose:
                    prefix = f'import _cds; _cds._set_verbose({verbose}); ' + prefix

                program = prefix + program
                env['PYCDSMODE'] = 'MANUALLY'
            else:
                if action == 'trace':
                    env.update({'PYCDSMODE': 'TRACE', 'PYCDSLIST': self.list})
                elif action == 'share':
                    env.update({'PYCDSMODE': 'SHARE', 'PYCDSARCHIVE': self.archive})
        elif action == 'dump':
            program = f'import cds.dump; cds.dump.run_dump("{self.list}", "{self.archive}")'

        self.logger.debug(f'running python `{program}` with env `{env}`')
        return_code, out, err = assert_python_source_ok(program, **env)
        if save_output:
            self._res.append((return_code, out.decode(), err.decode()))

    def run_trace(self, program: str, verbose=None, save_output=False):
        self._seq.append({'action': 'trace', 'program': program, 'verbose': verbose, 'save_output': save_output})

    def run_dump(self, verbose=None, save_output=False):
        self._seq.append({'action': 'dump', 'verbose': verbose, 'save_output': save_output})

    def run_share(self, program: str, verbose=None, save_output=False):
        self._seq.append({'action': 'share', 'program': program, 'verbose': verbose, 'save_output': save_output})

    def verify(self, verifier: t.Callable[[t.List[t.Tuple[int, str, str]]], None]):
        self._seq.append({'action': 'verify', 'verifier': verifier})

    def verify_files(self, check_list=True, check_archive=True):
        self.verify(lambda _: partial(self._verify_files_step, check_list, check_archive))

    def _verify_files_step(self, check_list, check_archive):
        if check_list:
            self.test.assertTrue(os.path.exists(self.list))
        if check_archive:
            self.test.assertTrue(os.path.exists(self.archive))
