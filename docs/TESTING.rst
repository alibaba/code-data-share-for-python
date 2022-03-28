Tests of code-data-share-for-python
=====
Performance tests
--------

Currently, we measure two kinds of scenario:

1. startup time with specific packages imported, which are subjectively selected from PyPI, in nox session ``test_import_third_party``;
2. pyperformance test w./w.o. cds.
    - We use a fork of pyperformance (https://github.com/oraluben/pyperformance/tree/cds) to run test with cds, a long
      term plan is to support original pyperformance to reuse their test cases.
    - pyperformance does not support custom setup required by cds, it might be feasible to specify a custom
      fork ``pyperf`` and do not change test cases themselves.
