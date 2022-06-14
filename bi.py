#!/usr/bin/env python3
import os.path
from subprocess import run

CONF = 'tmp-bisect-build'

INSTALL = os.path.abspath('../install')
ROOT = '../pycds'

if __name__ == '__main__':
    run(['rm', '-rf', INSTALL])
    run(['./configure', f'--prefix={INSTALL}', '--with-pydebug',
         '--with-openssl=/opt/homebrew/opt/openssl@3'])

    run(['make', 'clean'], check=True)
    run(['make', '-j'], check=True)
    run(['make', 'install'], check=True)

    run(['rm', '-rf', 'venv'], cwd=ROOT)
    run([f'{INSTALL}/bin/python3', '-m', 'venv', 'venv'], check=True, cwd=ROOT)

    run(['./venv/bin/pip3', 'install', 'nox'], check=True, cwd=ROOT)
    r = run(['./venv/bin/nox', '-s', 'tests_venv_current'], cwd=ROOT)
    assert r.returncode == 0
