from pathlib import Path

from skbuild import setup

readme = Path('README.rst').read_text()

# scikit-build/scikit-build#521 to workaround `pip install .`
for i in (Path(__file__).resolve().parent / "_skbuild").rglob("CMakeCache.txt"):
    i.unlink()

setup(
    name='code-data-share',
    version='0.0.2',
    packages=['cds'],
    package_dir={'': 'src'},
    python_requires='>=3.8.0',
    tests_require=['pytest'],
    long_description_content_type='text/x-rst; charset=UTF-8',
    long_description=readme,
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Operating System :: MacOS :: MacOS X',
        # 'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: Implementation :: CPython',
    ],
)
