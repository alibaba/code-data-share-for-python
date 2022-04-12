code-data-share-for-python
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. image:: https://img.shields.io/pypi/pyversions/code-data-share-for-python
.. image:: https://img.shields.io/pypi/implementation/code-data-share-for-python
.. image:: https://img.shields.io/badge/platform-linux--64%20%7C%20osx--64-lightgrey

Introduction
============

The main goal of code-data-share-for-python is to improve the efficiency
of importing python module, including time and memory footprint,
by persistence of code object of imported modules
with a memory-mapped file.

Basic Usage
===========================

code-data-share-for-python provides mechanism to
determine imported packages,
dump a memory mapped file from the list of packages
and import packages directly from the memory mapped file.


Install
-------
::

    pip install code-data-share-for-python

Determine the imported packages
-------------------------------
::

    # tracer: list imported packages to mod.lst
    PYCDSMODE=TRACE PYCDSLIST=mod.lst python -c 'import json'

Create the memory-mapped file
-----------------------------
::

    # dumper: create archive named mod.img from mod.lst
    python -c 'import cds.dump; import cds.dump; cds.dump.run_dump("mod.lst", "mod.img")

Import packages from archive
----------------------------
::

    # replayer
    PYCDSMODE=SHARE PYCDSARCHIVE=mod.img python -c 'import json'

Design & Implementation
=======================

There are several factors that block simply memcpy a CPython object to mapped memory.

1. `hash randomization in Python`_.
Hash randomization affects memory layout of hash-based container, e.g. dict,
so we don't support memory-mapping these types.
Instead user could use tuples and construct a dict on demand.

.. _hash randomization in Python: https://docs.python.org/3.3/using/cmdline.html#cmdoption-R

2. ASLR.
CPython uses a ``PyTypeObject* ob_type`` field to know the type of each object in python.
Due to ASLR, the addresses of same type object (e.g. ``PyLongObject``) and constants (e.g. ``Py_None``)
are randomly biased in every python instances.
To know the type of archived object,
addresses of some constants will be stored in the archive,
and type pointer will be updated based on ASLR shift.

3. GC

GC issues happen during archive dumping and loading.
All archived objects are untracked by ``PyObject_GC_UnTrack``,
to prevent being deallocated at the end of dumping stage,
their RCs are also biased by 1 to prevent being deallocated in the replayer.

PS: Looking forward to the `PEP 683 - Immortal Objects`_.

.. _PEP 683 - Immortal Objects: https://peps.python.org/pep-0683/

Building and Testing
====================

We use ``nox`` to build and test package,
it's the only dependency and will manage essential requirements (which can be found in `pyproject.toml`).
For local installs, ``nox`` is not necessary and pip will handle those dependencies.

Build from source
-----------------

For local test and development,
``pip install <local_package_dir>`` will just work,
e.g.:

::

    git clone https://github.com/alibaba/code-data-share-for-python.git
    pip install code-data-share-for-python

Build wheels
-------------

Build wheels of different python requires conda
to install support pythons.::

    # require conda
    nox -s build_wheel

If conda is not available,
wheel can also be generated with the ``build`` package:
::

    pip install build
    python -m build --wheel

Tests
-----

Tests can be run against the current python with venv,
or all support python versions with conda.

Functional tests
^^^^^^^^^^^^^^^^

Functional tests are under `tests/` and can be run via:
::

    nox -s tests_venv_current
    nox -s tests_multiple_conda

with venv and conda as backend, respectively.

Performance tests
^^^^^^^^^^^^^^^^^

Since pyperformance relies on venv
and seems to have issue with nested venv,
performance tests only use conda backend.

Currently, we measure two kinds of scenario:

1. startup time with specific packages imported, which are subjectively selected from PyPI: ``nox -s test_import_third_party``;
2. pyperformance test w./w.o. cds: ``nox -s pyperformance``
    - We use a fork of pyperformance (https://github.com/oraluben/pyperformance/tree/cds) to run test with cds, a long
      term plan is to support original pyperformance to reuse their test cases.
    - pyperformance does not support custom setup required by cds, it might be feasible to specify a custom
      fork ``pyperf`` and do not change test cases themselves.

AppCDS in Java
==============

The design is inspired by the Application Class-Data Sharing (AppCDS_) feature,
introduced in OpenJDK.
AppCDS allows a set of application classes to be pre-processed into a shared archive file,
which can then be memory-mapped at runtime to reduce startup time and memory footprint.

.. _AppCDS: https://openjdk.java.net/jeps/310
