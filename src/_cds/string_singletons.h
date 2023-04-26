#ifndef PYCDS_STRING_SINGLETONS_H
#define PYCDS_STRING_SINGLETONS_H

#define LIT(lit_name)                                                   \
    (PyObject *)(&_PyRuntime.static_objects.singletons.strings.literals \
                      ._py_##lit_name)

HANDLE_LITERAL(LIT(anon_dictcomp))
HANDLE_LITERAL(LIT(anon_genexpr))
HANDLE_LITERAL(LIT(anon_lambda))
HANDLE_LITERAL(LIT(anon_listcomp))
HANDLE_LITERAL(LIT(anon_module))
HANDLE_LITERAL(LIT(anon_setcomp))
HANDLE_LITERAL(LIT(anon_string))
HANDLE_LITERAL(LIT(anon_unknown))
HANDLE_LITERAL(LIT(close_br))
HANDLE_LITERAL(LIT(dbl_close_br))
HANDLE_LITERAL(LIT(dbl_open_br))
HANDLE_LITERAL(LIT(dbl_percent))
HANDLE_LITERAL(LIT(dot))
HANDLE_LITERAL(LIT(dot_locals))
HANDLE_LITERAL(LIT(empty))
HANDLE_LITERAL(LIT(json_decoder))
HANDLE_LITERAL(LIT(list_err))
HANDLE_LITERAL(LIT(newline))
HANDLE_LITERAL(LIT(open_br))
HANDLE_LITERAL(LIT(percent))
HANDLE_LITERAL(LIT(shim_name))
HANDLE_LITERAL(LIT(utf_8))

for (size_t i = 0; i < 128; ++i) {
    HANDLE_ASCII(
        (PyObject *)&_PyRuntime.static_objects.singletons.strings.ascii[i]);
    HANDLE_LATIN1(
        (PyObject *)&_PyRuntime.static_objects.singletons.strings.latin1[i]);
}

#endif
