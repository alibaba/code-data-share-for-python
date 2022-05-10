"""
Dump archive from list,
this module could be used in either command line form (`python -m cds.dump <list> <archive>`)
or API calls (`python -c "import cds.dump; cds.dump('<list>', '<archive>')"`.

The main difference of above two form is that,
since running python module with `python -m` relies on the module `runpy`,
the module will be already imported before dumping archive,
so the first form cannot dump `runpy` and its dependencies, for now.
"""


def run_dump(class_list: str, archive: str):
    from . import _verbose, _get_bootstrap_external

    code_map = {}
    meta_map = {}

    # patch finder
    def patch_get_code(orig_get_code):
        def wrap_get_code(self, name):
            code = orig_get_code(self, name)
            if code is not None:
                _verbose(f"{self.__class__.__name__}.get_code('{name}') returns code.", 2)
                code_map[name] = code
            else:
                _verbose(f"{self.__class__.__name__}.get_code('{name}') returns None.", 2)
            return code

        return wrap_get_code

    _get_bootstrap_external().SourceFileLoader.get_code = patch_get_code(
        _get_bootstrap_external().SourceFileLoader.get_code)

    def patch_exec_module(orig_exec_module):
        def wrap_exec_module(self, module):
            # module may override __name__ (e.g. _collections_abc) so we get that before exec_module
            name = module.__name__
            _verbose(f"{self.__class__.__name__}.exec_module('{module.__name__}')", 1)
            orig_exec_module(self, module)
            package, file, path = (
                getattr(module, i, None) for i in
                ('__package__', '__file__', '__path__')
            )
            # __path__ must be an iterable of strings,
            # we convert any valid __path__ to tuple of strings.
            # Ref: https://docs.python.org/3/reference/import.html#module-path
            path = tuple(path) if path else None
            meta_map[name] = (package, file, path)
            return module

        return wrap_exec_module

    _LoaderBasics = _get_bootstrap_external()._LoaderBasics
    _LoaderBasics.exec_module = patch_exec_module(_LoaderBasics.exec_module)

    # import lib
    import importlib
    from _io import open  # if this is called before interpreter ready, io module might not available yet.
    with open(class_list) as f:
        for line in f.readlines():
            line = line.strip()
            try:
                _ = importlib.__import__(line)
            except (ModuleNotFoundError, ImportError):
                _verbose(f'no module named {line}', 1)
            except SystemExit as e:
                _verbose(f'executing module {line} triggers an SystemExit({e.code}), ignoring.', 1)
            except Exception as e:
                _verbose(f'executing module {line} triggers an {e.__class__.__name__}, traceback:', 1)
                import traceback
                _verbose(''.join(traceback.format_exception(e)), 1)

    shared_class_data = tuple([(k, v) for k, v in {
        k: (*meta_map[k], code_map[k])
        for k in code_map.keys() & meta_map.keys()
    }.items()])

    # move in
    from _cds import _create_archive, _move_in
    _create_archive(archive)
    _move_in(shared_class_data)

    exit(0)


if __name__ == '__main__':
    import sys

    try:
        run_dump(*sys.argv[1:])
    except ValueError:
        pass
