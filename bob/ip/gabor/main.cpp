/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Fri  4 Apr 15:02:59 2014 CEST
 *
 * @brief Bindings to bob::ip color converters
 */

#ifdef NO_IMPORT_ARRAY
#undef NO_IMPORT_ARRAY
#endif
#include "main.h"

PyDoc_STRVAR(module_docstr, "Bob's Gabor wavelet support and utilities.");

#if PY_VERSION_HEX >= 0x03000000
static PyModuleDef module_definition = {
  PyModuleDef_HEAD_INIT,
  BOB_EXT_MODULE_NAME,
  module_docstr,
  -1,
  0,
  0
};
#endif

static PyObject* create_module (void) {

# if PY_VERSION_HEX >= 0x03000000
  PyObject* module = PyModule_Create(&module_definition);
# else
  PyObject* module = Py_InitModule3(BOB_EXT_MODULE_NAME, NULL, module_docstr);
# endif

  if (!module) return NULL;

  auto module_ = make_safe(module); ///< protects against early returns

  if (PyModule_AddStringConstant(module, "__version__", BOB_EXT_MODULE_VERSION) < 0) return NULL;

  if (!init_BobIpGaborWavelet(module)) return NULL;
  if (!init_BobIpGaborWaveletTransform(module)) return NULL;

  /* imports dependencies */
  if (import_bob_blitz() < 0) {
    PyErr_Print();
    PyErr_Format(PyExc_ImportError, "cannot import `%s'", BOB_EXT_MODULE_NAME);
    return 0;
  }

  if (import_bob_io_base() < 0) {
    PyErr_Print();
    PyErr_Format(PyExc_ImportError, "cannot import `%s'", BOB_EXT_MODULE_NAME);
    return 0;
  }

  // module was initialized successfully
  Py_INCREF(module);
  return module;
}

PyMODINIT_FUNC BOB_EXT_ENTRY_NAME (void) {
# if PY_VERSION_HEX >= 0x03000000
  return
# endif
    create_module();
}
