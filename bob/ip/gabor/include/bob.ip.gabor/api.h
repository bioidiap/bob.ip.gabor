/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date   Tue Nov 18 17:54:06 CET 2014
 *
 * @brief Python API for bob::ip::gabor
 */

#ifndef BOB_IP_GABOR_H
#define BOB_IP_GABOR_H

/* Define Module Name and Prefix for other Modules
   Note: We cannot use BOB_EXT_* macros here, unfortunately */
#define BOB_IP_GABOR_PREFIX    "bob.ip.gabor"
#define BOB_IP_GABOR_FULL_NAME "bob.ip.gabor._library"

#include <Python.h>

#include <bob.ip.gabor/config.h>
#include <bob.ip.gabor/Wavelet.h>
#include <bob.ip.gabor/Transform.h>
#include <bob.ip.gabor/Jet.h>
#include <bob.ip.gabor/Similarity.h>
#include <bob.ip.gabor/Graph.h>
#include <bob.ip.gabor/JetStatistics.h>

#include <boost/shared_ptr.hpp>

/*******************
 * C API functions *
 *******************/

/* Enum defining entries in the function table */
enum _PyBobIpGabor_ENUM{
  PyBobIpGabor_APIVersion_NUM = 0,
  // Bindings for bob.ip.gabor.Wavelet
  PyBobIpGaborWavelet_Type_NUM,
  PyBobIpGaborWavelet_Check_NUM,
  // Bindings for bob.ip.gabor.Transform
  PyBobIpGaborTransform_Type_NUM,
  PyBobIpGaborTransform_Check_NUM,
  // Bindings for bob.ip.gabor.Jet
  PyBobIpGaborJet_Type_NUM,
  PyBobIpGaborJet_Check_NUM,
  // Bindings for bob.ip.gabor.imilarity
  PyBobIpGaborSimilarity_Type_NUM,
  PyBobIpGaborSimilarity_Check_NUM,
  // Bindings for bob.ip.gabor.Graph
  PyBobIpGaborGraph_Type_NUM,
  PyBobIpGaborGraph_Check_NUM,
  // Bindings for bob.ip.gabor.JetStatistics
  PyBobIpGaborJetStatistics_Type_NUM,
  PyBobIpGaborJetStatistics_Check_NUM,
  // Total number of C API pointers
  PyBobIpGabor_API_pointers
};


// Gabor wavelet
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Wavelet> cxx;
} PyBobIpGaborWaveletObject;


// Gabor wavelet transform
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Transform> cxx;
} PyBobIpGaborTransformObject;

// Gabor jet
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Jet> cxx;
} PyBobIpGaborJetObject;


// Gabor jet similarity
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Similarity> cxx;
} PyBobIpGaborSimilarityObject;


// Gabor graph
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Graph> cxx;
} PyBobIpGaborGraphObject;

// Gabor jet statistics
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::JetStatistics> cxx;
} PyBobIpGaborJetStatisticsObject;


#ifdef BOB_IP_GABOR_MODULE

  /* This section is used when compiling `bob.ip.gabor' itself */

  /**************
   * Versioning *
   **************/

  extern int PyBobIpGabor_APIVersion;

  /****************
   * Type objects *
   ***************/

  extern PyTypeObject PyBobIpGaborWavelet_Type;
  extern PyTypeObject PyBobIpGaborTransform_Type;
  extern PyTypeObject PyBobIpGaborJet_Type;
  extern PyTypeObject PyBobIpGaborSimilarity_Type;
  extern PyTypeObject PyBobIpGaborGraph_Type;
  extern PyTypeObject PyBobIpGaborJetStatistics_Type;

  /*******************
   * Check functions *
   ******************/

  int PyBobIpGaborWavelet_Check(PyObject* o);
  int PyBobIpGaborTransform_Check(PyObject* o);
  int PyBobIpGaborJet_Check(PyObject* o);
  int PyBobIpGaborSimilarity_Check(PyObject* o);
  int PyBobIpGaborGraph_Check(PyObject* o);
  int PyBobIpGaborJetStatistics_Check(PyObject* o);

#else

  /* This section is used in modules that use `bob.ip.gabor's' C-API */

#  if defined(NO_IMPORT_ARRAY)
  extern void **PyBobIpGabor_API;
#  else
#    if defined(PY_ARRAY_UNIQUE_SYMBOL)
  void **PyBobIpGabor_API;
#    else
  static void **PyBobIpGabor_API=NULL;
#    endif
#  endif

  /**************
   * Versioning *
   **************/

# define PyBobIpGabor_APIVersion (*(int *)PyBobIpGabor_API[PyBobIpGabor_APIVersion_NUM])


  /****************
   * Type objects *
   ***************/

#define PyBobIpGaborWavelet_Type (*(PyTypeObject *)PyBobIpGabor_API[PyBobIpGaborWavelet_Type_NUM])
#define PyBobIpGaborTransform_Type (*(PyTypeObject *)PyBobIpGabor_API[PyBobIpGaborTransform_Type_NUM])
#define PyBobIpGaborJet_Type (*(PyTypeObject *)PyBobIpGabor_API[PyBobIpGaborJet_Type_NUM])
#define PyBobIpGaborSimilarity_Type (*(PyTypeObject *)PyBobIpGabor_API[PyBobIpGaborSimilarity_Type_NUM])
#define PyBobIpGaborTransform_Type (*(PyTypeObject *)PyBobIpGabor_API[PyBobIpGaborTransform_Type_NUM])
#define PyBobIpGaborJetStatistics_Type (*(PyTypeObject *)PyBobIpGabor_API[PyBobIpGaborJetStatistics_Type_NUM])


  /*******************
   * Check functions *
   ******************/
#define PyBobIpGaborWavelet_Check (*(int (*)(PyObject*)) PyBobIpGabor_API[PyBobIpGaborWavelet_Check_NUM])
#define PyBobIpGaborTransform_Check (*(int (*)(PyObject*)) PyBobIpGabor_API[PyBobIpGaborTransform_Check_NUM])
#define PyBobIpGaborJet_Check (*(int (*)(PyObject*)) PyBobIpGabor_API[PyBobIpGaborJet_Check_NUM])
#define PyBobIpGaPyBobIpGaborSimilarity_Check (*(int (*)(PyObject*)) PyBobIpGabor_API[PyBobIpGaborSimilarity_Check_NUM])
#define PyBobIpGaborGraph_Check (*(int (*)(PyObject*)) PyBobIpGabor_API[PyBobIpGaborGraph_Check_NUM])
#define PyBobIpGaborJetStatistics_Check (*(int (*)(PyObject*)) PyBobIpGabor_API[PyBobIpGaborJetStatistics_Check_NUM])


# if !defined(NO_IMPORT_ARRAY)

  /**
   * Returns -1 on error, 0 on success.
   */
  static int import_bob_ip_gabor(void) {

    PyObject *c_api_object;
    PyObject *module;

    module = PyImport_ImportModule(BOB_IP_GABOR_FULL_NAME);

    if (!module) return -1;

    c_api_object = PyObject_GetAttrString(module, "_C_API");

    if (!c_api_object) {
      Py_DECREF(module);
      return -1;
    }

#   if PY_VERSION_HEX >= 0x02070000
    if (PyCapsule_CheckExact(c_api_object)) {
      PyBobIpGabor_API = (void **)PyCapsule_GetPointer(c_api_object,
          PyCapsule_GetName(c_api_object));
    }
#   else
    if (PyCObject_Check(c_api_object)) {
      PyBobIpGabor_API = (void **)PyCObject_AsVoidPtr(c_api_object);
    }
#   endif

    Py_DECREF(c_api_object);
    Py_DECREF(module);

    if (!PyBobIpGabor_API) {
      PyErr_SetString(PyExc_ImportError, "cannot find C/C++ API "
#   if PY_VERSION_HEX >= 0x02070000
          "capsule"
#   else
          "cobject"
#   endif
          " at `" BOB_IP_GABOR_FULL_NAME "._C_API'");
      return -1;
    }

    /* Checks that the imported version matches the compiled version */
    int imported_version = *(int*)PyBobIpGabor_API[PyBobIpGabor_APIVersion_NUM];

    if (BOB_IP_GABOR_API_VERSION != imported_version) {
      PyErr_Format(PyExc_ImportError, BOB_IP_GABOR_FULL_NAME " import error: you compiled against API version 0x%04x, but are now importing an API with version 0x%04x which is not compatible - check your Python runtime environment for errors", BOB_IP_GABOR_API_VERSION, imported_version);
      return -1;
    }

    /* If you get to this point, all is good */
    return 0;

  }

# endif //!defined(NO_IMPORT_ARRAY)

#endif /* BOB_IP_GABOR_MODULE */

#endif /* BOB_IP_GABOR_H */
