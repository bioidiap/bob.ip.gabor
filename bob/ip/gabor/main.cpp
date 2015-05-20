/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Tue Jun  3 17:44:24 CEST 2014
 *
 * @brief Bindings to bob::ip color converters
 */

#define BOB_IP_GABOR_MODULE
#include <bob.ip.gabor/api.h>

#ifdef NO_IMPORT_ARRAY
#undef NO_IMPORT_ARRAY
#endif
#include <bob.blitz/capi.h>
#include <bob.blitz/cleanup.h>
#include <bob.io.base/api.h>


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

extern bool init_BobIpGaborWavelet(PyObject* module);
extern bool init_BobIpGaborTransform(PyObject* module);
extern bool init_BobIpGaborJet(PyObject* module);
extern bool init_BobIpGaborSimilarity(PyObject* module);
extern bool init_BobIpGaborGraph(PyObject* module);

int PyBobIpGabor_APIVersion = BOB_IP_GABOR_API_VERSION;

static PyObject* create_module (void) {

# if PY_VERSION_HEX >= 0x03000000
  PyObject* module = PyModule_Create(&module_definition);
# else
  PyObject* module = Py_InitModule3(BOB_EXT_MODULE_NAME, NULL, module_docstr);
# endif

  if (!module) return NULL;

  auto module_ = make_safe(module); ///< protects against early returns

  if (!init_BobIpGaborWavelet(module)) return NULL;
  if (!init_BobIpGaborTransform(module)) return NULL;
  if (!init_BobIpGaborJet(module)) return NULL;
  if (!init_BobIpGaborSimilarity(module)) return NULL;
  if (!init_BobIpGaborGraph(module)) return NULL;

  // C-API bindings

  static void* PyBobIpGabor_API[PyBobIpGabor_API_pointers];

  /**************
   * Versioning *
   **************/

  PyBobIpGabor_API[PyBobIpGabor_APIVersion_NUM] = (void *)&PyBobIpGabor_APIVersion;


  /****************
   * Type objects *
   ***************/
  PyBobIpGabor_API[PyBobIpGaborWavelet_Type_NUM] = (void *)&PyBobIpGaborWavelet_Type;
  PyBobIpGabor_API[PyBobIpGaborTransform_Type_NUM] = (void *)&PyBobIpGaborTransform_Type;
  PyBobIpGabor_API[PyBobIpGaborJet_Type_NUM] = (void *)&PyBobIpGaborJet_Type;
  PyBobIpGabor_API[PyBobIpGaborSimilarity_Type_NUM] = (void *)&PyBobIpGaborSimilarity_Type;
  PyBobIpGabor_API[PyBobIpGaborTransform_Type_NUM] = (void *)&PyBobIpGaborTransform_Type;

  /*******************
   * Check functions *
   ******************/

  PyBobIpGabor_API[PyBobIpGaborWavelet_Check_NUM] = (void *)&PyBobIpGaborWavelet_Check;
  PyBobIpGabor_API[PyBobIpGaborTransform_Check_NUM] = (void *)&PyBobIpGaborTransform_Check;
  PyBobIpGabor_API[PyBobIpGaborJet_Check_NUM] = (void *)&PyBobIpGaborJet_Check;
  PyBobIpGabor_API[PyBobIpGaborSimilarity_Check_NUM] = (void *)&PyBobIpGaborSimilarity_Check;
  PyBobIpGabor_API[PyBobIpGaborTransform_Check_NUM] = (void *)&PyBobIpGaborTransform_Check;


#if PY_VERSION_HEX >= 0x02070000

  /* defines the PyCapsule */

  PyObject* c_api_object = PyCapsule_New((void *)PyBobIpGabor_API,
      BOB_EXT_MODULE_PREFIX "." BOB_EXT_MODULE_NAME "._C_API", 0);

#else

  PyObject* c_api_object = PyCObject_FromVoidPtr((void *)PyBobIpGabor_API, 0);

#endif

  if (!c_api_object) return 0;

  if (PyModule_AddObject(module, "_C_API", c_api_object) < 0) return 0;


  // imports dependencies

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
