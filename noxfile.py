"""
Ref: https://github.com/scikit-build/scikit-build/blob/master/noxfile.py
"""
import os
import platform
import typing as t

import nox
from nox.command import CommandFailed
from nox.virtualenv import CondaEnv

nox.options.default_venv_backend = 'conda'

SUPPORTED_PYTHONS = ['3.8', '3.9', '3.10', '3.11']

OS = platform.system()
RELEASE = platform.release()
PYCDS_ROOT = os.path.dirname(__file__)

DISABLE_SITE_HOOK_KEY = 'DISABLE_SITE_HOOK'
CDS_PYPERFORMANCE = 'git+https://github.com/oraluben/pyperformance.git@cds'


def _py_version(session: nox.Session):
    """
    :return: "3.9", "3.10", ...
    """
    out: str = session.run('python', '-c', 'import sys; print(f"{sys.version_info[0]}.{sys.version_info[1]}")',
                           silent=True)
    out = out.strip()
    return out


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
    Package('aiohttp'),
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
    Package('seaborn', conda=True),
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
        session.run('python', '-c', f'import cds.dump; cds.dump.run_dump("{lst}", "{img}")')
        session.run('python', '-c', package.import_stmt, env={'PYCDSMODE': 'SHARE', 'PYCDSARCHIVE': img})


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
        session.run('python', '-c', f'import cds.dump; cds.dump.run_dump("{lst}", "{img}")', log=False)
        session.run('python', '-c', package.import_stmt, env={'PYCDSMODE': 'SHARE', 'PYCDSARCHIVE': img}, log=False)
        logger.info(f'finish generating CDS archive for {package.name}')

        raw_out = f'perf-import-{session.python}-raw'
        cds_out = f'perf-import-{session.python}-cds'

        session.run('pyperf', 'command', '--fast', f'--append={raw_out}.json', f'--name={package.name}',
                    'python', '-c', package.import_stmt)
        session.run('pyperf', 'command', '--fast', f'--append={cds_out}.json', f'--name={package.name}',
                    '--inherit-environ=PYCDSMODE,PYCDSARCHIVE',
                    'python', '-c', package.import_stmt,
                    env={'PYCDSMODE': 'SHARE', 'PYCDSARCHIVE': img})


def _pyperformance(session: nox.Session, pyperformance_args=None):
    session.install(CDS_PYPERFORMANCE)

    configs = [
        (os.path.realpath(f'pyperformance-{_py_version(session)}-{config_name}.json'), config_args)
        for (config_name, config_args) in [
            ('raw', []),
            ('cds-site', ['--install-cds', PYCDS_ROOT]),
            ('cds', ['--install-cds', PYCDS_ROOT, '--enable-cds']),
        ]
    ]

    tmp = session.create_tmp()
    session.chdir(tmp)

    if pyperformance_args is None:
        pyperformance_args = ['pyperformance', 'run']

    cmd_exc = None
    for out, args in configs:
        if os.path.exists(out):
            session.run('mv', out, out + '.old')
        try:
            session.run(*(pyperformance_args + args), f'--out={out}')
        except CommandFailed as e:
            if cmd_exc is None:
                cmd_exc = e

    if cmd_exc is not None:
        raise cmd_exc


@nox.session(venv_backend='venv')
def pyperformance_current(session: nox.Session):
    _pyperformance(session, ['pyperformance', 'run', '--fast'])


@nox.session(python=SUPPORTED_PYTHONS)
def pyperformance(session: nox.Session):
    _pyperformance(session, ['pyperformance', 'run', '--fast'])


@nox.session(python=SUPPORTED_PYTHONS)
def pyperformance_looong(session: nox.Session):
    _pyperformance(session, ['pyperformance', 'run', '--rigorous'])


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
