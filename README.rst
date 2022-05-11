code-data-share-for-python
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. image:: https://img.shields.io/pypi/pyversions/code-data-share
   :target: https://pypi.org/project/code-data-share/
.. image:: https://img.shields.io/pypi/implementation/code-data-share
   :target: https://pypi.org/project/code-data-share/
.. image:: https://img.shields.io/badge/platform-linux--64%20%7C%20osx--64-lightgrey
   :target: https://pypi.org/project/code-data-share/

Introduction
============

The main goal of code-data-share-for-python is to improve the efficiency
of importing python module, including time and memory footprint,
by persistence of code object of imported modules
with a memory-mapped file.

For more information not included in this README,
e.g. detailed user and developer guide, performance results,
please refer to our `wiki page`_.

.. _wiki page: https://github.com/alibaba/code-data-share-for-python/wiki

Basic Usage
===========================

code-data-share-for-python provides mechanism to
determine imported packages,
dump a memory mapped file from the list of packages
and import packages directly from the memory mapped file.


Install
-------
::

    pip install code-data-share

Determine the imported packages
-------------------------------
::

    # tracer: list imported packages to mod.lst
    PYCDSMODE=TRACE PYCDSLIST=mod.lst python -c 'import json'

Create the memory-mapped file
-----------------------------
::

    # dumper: create archive named mod.img from mod.lst
    python -c 'import cds.dump; cds.dump.run_dump("mod.lst", "mod.img")'

Import packages from archive
----------------------------
::

    # replayer
    PYCDSMODE=SHARE PYCDSARCHIVE=mod.img python -c 'import json'

AppCDS in Java
==============

The design is inspired by the Application Class-Data Sharing (AppCDS_) feature,
introduced in OpenJDK.
AppCDS allows a set of application classes to be pre-processed into a shared archive file,
which can then be memory-mapped at runtime to reduce startup time and memory footprint.

.. _AppCDS: https://openjdk.java.net/jeps/310
