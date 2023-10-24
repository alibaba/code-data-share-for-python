"""
Ref: https://github.com/scikit-build/scikit-build/blob/master/noxfile.py
"""
import os
import platform
import shutil
import typing as t

import nox
from nox.command import CommandFailed
from nox.virtualenv import CondaEnv

nox.options.default_venv_backend = 'conda'

SUPPORTED_PYTHONS = ['3.8', '3.9', '3.10', '3.11', '3.12']

OS = platform.system()
RELEASE = platform.release()
PYCDS_ROOT = os.path.dirname(__file__)

GA = os.environ.get('GITHUB_ACTIONS') == 'true'

CDS_PYPERFORMANCE = 'git+https://github.com/oraluben/pyperformance.git@cds-dev'


def _clean_nox():
    from nox.virtualenv import shutil

    shutil.rmtree(os.path.join(PYCDS_ROOT, '.nox'), ignore_errors=True)


def ci_session_cleanup():
    if OS == 'Windows' and GA:
        _clean_nox()


def _py_version(session: nox.Session):
    """
    :return: "3.9", "3.10", ...
    """
    out: str = session.run('python', '-c', 'import sys; print(f"{sys.version_info[0]}.{sys.version_info[1]}")',
                           silent=True)
    out = out.strip()
    return out


def _perf_config() -> t.List[str]:
    result = []
    _run = os.environ.get('RUN', '').lower()
    if _run in ('fast', 'short'):
        result.append('--fast')
    elif _run in ('long', 'rigorous'):
        result.append('--rigorous')
    return result


def _self_tests(session: nox.Session):
    session.install(".")
    session.install("pytest")
    session.run('pytest', )


class Package(t.NamedTuple):
    name: str
    module: t.Union[t.List[str], str, None] = None
    conda: bool = False
    skip: t.Optional[t.Callable[[str], bool]] = None

    def __repr__(self):
        return self.name

    @property
    def import_stmt(self):
        mods = self.module
        if mods is None:
            mods = self.name
        elif isinstance(mods, list):
            mods = ', '.join(mods)
        return f"""import {mods}"""

    def install_in(self, session: nox.Session):
        if self.conda:
            assert isinstance(session.virtualenv, CondaEnv)
            session.conda_install(self.name)
        else:
            session.install(self.name)

    def should_skip(self, py_version):
        if self.skip:
            return self.skip(py_version)
        return False


@nox.session(venv_backend='venv')
def tests_venv_current(session: nox.Session):
    """
    Run test on current CPython.
    """

    _self_tests(session)


@nox.session(python=SUPPORTED_PYTHONS)
def tests_multiple_conda(session: nox.Session):
    """
    Regression on supported version of CPython.
    """

    _self_tests(session)


PACKAGES = (
    Package('boto3'),
    Package('requests'),
    Package('numpy'),
    Package('pyparsing'),
    Package('sqlalchemy'),
    Package('werkzeug'),
    Package('aiohttp', skip=lambda _py: _py in ('3.12',)),
    Package('google-cloud-storage', module='google.cloud.storage'),
    Package('flask'),
    Package('azure-core', module='azure.core'),
    Package('jsonschema'),

    # Pillow is a package with C extension, and do not involve much CPython's internal library.
    # So this should be a good test for cds'ing C extensions.
    Package('Pillow', module=['PIL.Image']),

    Package('scipy', conda=True),

    # conda-provided tf might require pypy and this is not what we want,
    # and pypi only provides tf for CPython <= 3.11
    Package('tensorflow', skip=lambda _py: _py in ('3.12',)),
    Package('seaborn', conda=True, skip=lambda _py: OS == 'Windows'),
    Package('azureml-core', module='azureml.core'),

    Package('opencv-python', module='cv2')
)

for py in SUPPORTED_PYTHONS:
    @nox.session(name=f'test_import_third_party-{py}', tags=['test_import_third_party'], python=py)
    @nox.parametrize('package', [package for package in PACKAGES if not package.should_skip(py)])
    def test_import_third_party(session: nox.Session, package):
        """
        Test import with / without PyCDS.
        """
        session.install('.')

        package.install_in(session)

        session.run('python', '-c', package.import_stmt)

        tmp = session.create_tmp()

        lst = os.path.join(tmp, 'test.lst')
        img = os.path.join(tmp, 'test.img')

        session.run('python', '-c', package.import_stmt, env={'PYCDSMODE': 'TRACE', 'PYCDSLIST': lst})
        session.run('python', '-c', f'import cds.dump; cds.dump.run_dump({repr(lst)}, {repr(img)})')
        session.run('python', '-c', package.import_stmt, env={'PYCDSMODE': 'SHARE', 'PYCDSARCHIVE': img})

        ci_session_cleanup()


    @nox.session(name=f'test_import_third_party_perf-{py}', tags=['test_import_third_party_perf'], python=py)
    @nox.parametrize('package', [package for package in PACKAGES if not package.should_skip(py)])
    def test_import_third_party_perf(session: nox.Session, package):
        """
        Benchmark import statements w./w.o. CDS.

        Generates perf-{py-ver}-{cds|raw}.json.
        See `perf.sh`.
        """

        from nox.logger import logger
        session.install('pyperf', '.')

        package.install_in(session)

        tmp = session.create_tmp()

        lst = os.path.join(tmp, 'test.lst')
        img = os.path.join(tmp, 'test.img')

        logger.info(f'start generating CDS archive for {package.name}')
        session.run('python', '-c', package.import_stmt, env={'PYCDSMODE': 'TRACE', 'PYCDSLIST': lst}, log=False)
        session.run('python', '-c', f'import cds.dump; cds.dump.run_dump({repr(lst)}, {repr(img)})', log=False)
        session.run('python', '-c', package.import_stmt, env={'PYCDSMODE': 'SHARE', 'PYCDSARCHIVE': img}, log=False)
        logger.info(f'finish generating CDS archive for {package.name}')

        raw_out = f'perf-import-{session.python}-{OS.lower()}-raw'
        cds_out = f'perf-import-{session.python}-{OS.lower()}-cds'

        base_cmd = ['pyperf', 'command'] + _perf_config() + [f'--name={package.name}']

        session.run(*base_cmd, f'--append={raw_out}.json',
                    'python', '-c', package.import_stmt)
        session.run(*base_cmd, f'--append={cds_out}.json', '--inherit-environ=PYCDSMODE,PYCDSARCHIVE',
                    'python', '-c', package.import_stmt,
                    env={'PYCDSMODE': 'SHARE', 'PYCDSARCHIVE': img})

        ci_session_cleanup()


@nox.session(venv_backend='venv')
def pyperformance(session: nox.Session):
    session.install(CDS_PYPERFORMANCE)

    configs = [
        (os.path.realpath(f'pyperformance-{_py_version(session)}-{OS.lower()}-{config_name}.json'), config_args)
        for (config_name, config_args) in [
            ('raw', []),
            ('cds-site', ['--install-cds', PYCDS_ROOT]),
            ('cds', ['--install-cds', PYCDS_ROOT, '--enable-cds']),
        ]
    ]

    tmp = session.create_tmp()
    session.chdir(tmp)

    pyperformance_cmd = ['pyperformance', 'run'] + _perf_config()

    cmd_exc = None
    for out, args in configs:
        if os.path.exists(out):
            shutil.move(out, out + '.old')
        try:
            session.run(*(pyperformance_cmd + args), f'--out={out}')
        except CommandFailed as e:
            if cmd_exc is None:
                cmd_exc = e

    if cmd_exc is not None:
        raise cmd_exc


@nox.session(venv_backend='venv')
def build_sdist(session: nox.Session):
    """
    Make an SDist.
    """

    session.install("build")
    session.run("python", "-m", "build", "--sdist")


@nox.session(python=SUPPORTED_PYTHONS)
def build_wheel(session: nox.Session):
    """
    Make a wheel.
    """

    session.install("build")
    session.run("python", "-m", "build", "--wheel")


@nox.session(venv_backend='venv')
def render_readme(session: nox.Session):
    session.install('docutils')

    session.run('rst2html.py', '--report=2', '--halt=2', 'README.rst', 'README.html')
