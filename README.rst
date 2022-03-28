code-data-share-for-python
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. image:: https://shields.io/badge/python-3.9%20%7C%203.10-blue

Introduction
============

The main goal of code-data-share-for-python is to improve the efficiency
of importing python module, including time and memory footprint.

The design is inspired by the Application Class-Data Sharing (AppCDS_) feature,
introduced in OpenJDK.
AppCDS allows a set of application classes to be pre-processed into a shared archive file,
which can then be memory-mapped at runtime to reduce startup time and memory footprint.

.. _AppCDS: https://openjdk.java.net/jeps/310

code-data-share-for-python use memory mapping to store and share data likely to not change.
Detail design and implementation will be elaborate below.

Design & Implementation
=======================

WIP

Testing
=======

See `docs/TESTING.rst`_

.. _docs/TESTING.rst: docs/TESTING.rst
