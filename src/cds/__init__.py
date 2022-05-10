import _io
import os
import sys

import _cds

_bootstrap_external = None


def _get_bootstrap_external():
    global _bootstrap_external
    if _bootstrap_external is None:
        # noinspection PyUnresolvedReferences,PyProtectedMember
        from importlib import _bootstrap_external
    return _bootstrap_external


def _verbose(msg, verbosity=1):
    # Do not dynamically import flags here,
    # this function will be called in import progress,
    # which might cause cyclic import.
    if _cds.flags.verbose >= verbosity:
        print(f'[CDS] {msg}', file=sys.stderr, flush=True)


def trace(class_list: str):
    from _cds import _set_mode
    _set_mode(1)

    from _io import open
    f = open(class_list, 'a+')

    def patch_get_code(orig_get_code):
        def wrap_get_code(self, name):
            code = orig_get_code(self, name)
            print(name, file=f, flush=True)
            return code

        return wrap_get_code

    SourceLoader = _get_bootstrap_external().SourceLoader
    SourceLoader.get_code = patch_get_code(SourceLoader.get_code)


def share(archive: str):
    from _cds import _load_archive, _get_obj

    _load_archive(archive)

    CDSFinder.init(dict(_get_obj()))

    sys.meta_path.insert(0, CDSFinder)


class CDSFinder:
    # class-level cache
    shared_module: dict = None
    _bootstrap = None
    _bootstrap_external = _get_bootstrap_external()

    @classmethod
    def init(cls, shared_module: dict):
        cls.shared_module = shared_module
        # noinspection PyUnresolvedReferences,PyProtectedMember
        import importlib._bootstrap as b
        cls._bootstrap = b

    # Start of importlib.abc.MetaPathFinder interface.
    @classmethod
    def find_spec(cls, fullname, path, target=None):
        if (
                cls.shared_module is None or
                fullname not in cls.shared_module
        ):
            _verbose(f"find_spec missed '{fullname}'", 1)
            return None
        # t0 = time.time()

        # notice: if one package got re-imported via cds,
        # we assume it has some magic import logic,
        # e.g. opencv: https://github.com/opencv/opencv/blob/c8228e5789510ec26378eceb17311dd7bddf0b33/modules/python/package/cv2/__init__.py#L151-L155
        # so we only use cds in the first time to fall back to python's import.
        package, file, path, code = cls.shared_module.pop(fullname)
        loader = cls(fullname, code, file)
        is_package = path is not None
        spec = cls._bootstrap.ModuleSpec(fullname, loader, origin=file,
                                         is_package=is_package)
        if file:
            spec.has_location = True
        if is_package:
            spec.submodule_search_locations = list(path)
        # _cds_verbose('[find_spec] ' + fullname + ': ' + str((time.time() - t0) * 1000), 2)
        return spec

    @classmethod
    def invalidate_caches(cls):
        pass

    # End of importlib.abc.MetaPathFinder interface.

    # Start of importlib.abc.Loader interface.
    def __init__(self, fullname, code, file_path):
        self.name = fullname
        self.code = code
        self.path = file_path

    def create_module(self, spec):
        return None

    def exec_module(self, module):
        _verbose(f"exec_module cached '{self.name}'", 1)
        exec(self.code, module.__dict__)

    # Start of importlib.FileLoader interface.
    # noinspection PyMethodMayBeStatic
    # Some package, e.g. azureml-core, need to load data via this method.
    # Ref: FileLoader.get_data
    def get_data(self, path):
        with _io.FileIO(path, 'r') as file:
            return file.read()

    @_bootstrap_external._check_name
    def get_resource_reader(self, module):
        if sys.version_info < (3, 10, 0):
            return None
        from importlib.readers import FileReader
        return FileReader(self)
    # End of importlib.FileLoader interface.

    # # Start of importlib.abc.InspectLoader interface.
    # def get_code(self, fullname):
    #     print('get_code', file=sys.stderr)
    # # Start of importlib.abc.ExecutionLoader interface.
    # def get_source(self, fullname):
    #     print('get_source', file=sys.stderr)
    # # End of importlib.abc.ExecutionLoader interface.
    # # End of importlib.abc.InspectLoader interface.
    # End of importlib.abc.Loader interface.


def init_from_env():
    if _cds._get_initialized():
        _verbose('cds.init_from_env re-entered', 1)
        return

    MODE_KEY = 'PYCDSMODE'

    verbosity = os.environ.get('PYCDSVERBOSE', None)
    mode = os.environ.get(MODE_KEY, None)
    if verbosity is not None:
        try:
            verbosity = int(verbosity)
            assert 0 <= verbosity <= 2
            from _cds import _set_verbose
            _set_verbose(verbosity)
        except ValueError:
            _verbose('failed to parse PYCDSVERBOSE, ignoring', 0)
        except AssertionError:
            _verbose('PYCDSVERBOSE should be [0, 2]', 0)
    if mode is not None:
        class_list = os.environ.get('PYCDSLIST')
        archive = os.environ.get('PYCDSARCHIVE')
        if mode == 'TRACE':
            if class_list is not None:
                trace(class_list)
                return
            else:
                _verbose('class list is required for tracer, ignoring.', 1)
        elif mode == 'DUMP':
            _verbose('dumper should be run with `python -m cds.dump <class_list> <archive>, ignoring.`', 1)
        elif mode == 'SHARE':
            if archive is not None:
                share(archive)
                return
            else:
                _verbose('archive is required for loader, ignoring.', 1)
        elif mode == 'MANUALLY':
            from _cds import _set_mode
            _set_mode(-1)
            return

    from _cds import _set_mode
    _set_mode(0)
