/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief Bindings for a Gabor wavelet transform
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#define BOB_IP_GABOR_MODULE
#include "main.h"
#include <bob.blitz/cppapi.h>
#include <bob.blitz/cleanup.h>
#include <bob.io.base/api.h>
#include <bob.extension/documentation.h>


static inline char* c(const char* o){return const_cast<char*>(o);}


/******************************************************************/
/************ Constructor Section *********************************/
/******************************************************************/

static auto Jet_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".Jet",
  "A Gabor jet contains the responses of all Gabor wavelets of the Gabor wavelet family at a certain position in the image",
  "The Gabor jet represents the local texture at a certain offset point of an image that it was extracted from. "
  "Commonly, the complex-valued Gabor jet is stored as a vector of absolute values and a vector of phase values. "
  "Also, usually the Gabor jet is normalized to unit Euclidean length."
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Creates a Gabor jet from various sources of data",
    "The family of Gabor wavelets :math:`\\check\\psi_{\\vec k_j}` is created by considering several center frequencies :math:`\\vec k_j`:\n\n"
    ".. math::\n"
    "   \\vec k_j = k_{\\zeta} \\left\\{ \\begin{array}{c} \\cos \\vartheta_{\\nu} \\\\ \\sin \\vartheta_{\\nu} \\end{array}\\right\\}\n\n"
    "where :math:`k_{\\zeta} = k_{max} \\cdot k_{fac}^{\\zeta}` with :math:`\\zeta = \\{0,\\dots,\\zeta_{max}-1\\}` and :math:`\\vartheta_{\\nu} = \\frac{\\nu \\cdot 2\\pi}{\\nu_{max}}` and :math:`\\nu = \\{0,\\dots,\\nu_{max}-1\\}`",
    true
  )
  .add_prototype("trafo_image, position, [normalize]", "")
  .add_prototype("complex, [normalize]", "")
  .add_prototype("hdf5", "")
  .add_parameter("trafo_image", "array_like(complex, 3D)", "The result of the Gabor wavelet transform, i.e., of :py:func:`bob.ip.gabor.Transform.transform`")
  .add_parameter("position", "(int, int)", "The position, where the Gabor jet should be extracted")
  .add_parameter("complex", "array_like(complex, 3D)", "The complex-valued representation of a Gabor jet")
  .add_parameter("normalize", "bool", "[default: True] Should the newly generated Gabor jet be normalized to unit Euclidean length?")
  .add_parameter("hdf5", ":py:class:`bob.io.base.HD5Ffile`", "An HDF5 file open for reading to load the Gabor jet from")
);

static int PyBobIpGaborJet_init(PyBobIpGaborJetObject* self, PyObject* args, PyObject* kwargs) {

  char* kwlist1[] = {c("hdf5"), NULL};
  char* kwlist2[] = {c("trafo_image"), c("position"), c("normalize"), NULL};
  char* kwlist3[] = {c("complex"), c("normalize"), NULL};

  try{
    // three ways to call
    PyObject* k = Py_BuildValue("s", kwlist1[0]);
    auto k_ = make_safe(k);
    if (
      (kwargs && PyDict_Contains(kwargs, k)) ||
      (args && PyTuple_Size(args) == 1 && PyBobIoHDF5File_Check(PyTuple_GetItem(args, 0)))
    ){
      PyBobIoHDF5FileObject* hdf5;
      if (
        !PyArg_ParseTupleAndKeywords(
          args, kwargs,
          "O&", kwlist1,
          &PyBobIoHDF5File_Converter, &hdf5
        )
      ){
        Jet_doc.print_usage();
        return -1;
      }
      auto hdf5_ = make_safe(hdf5);

      self->cxx.reset(new bob::ip::gabor::Jet(*hdf5->f));
    } else {
      PyObject* k2 = Py_BuildValue("s", kwlist2[1]);
      PyObject* k3 = Py_BuildValue("s", kwlist2[2]);
      auto k2_ = make_safe(k2), k3_ = make_safe(k3);
      PyObject* norm = 0;
      PyBlitzArrayObject* data = 0;

      if (
        PyDict_Size(kwargs) == 3 ||
        PyDict_Size(kwargs) == 2 && PyDict_Contains(kwargs, k2) && PyDict_Contains(kwargs, k3) ||
        PyDict_Size(kwargs) == 1 && PyDict_Contains(kwargs, k2) && PySequence_Check(PyList_GetItem(args, 0))||
        !kwargs &&  PySequence_Check(PyList_GetItem(args, 1))
      ){
        blitz::TinyVector<int,2> pos;
        // second variant
        if (
          !PyArg_ParseTupleAndKeywords(
            args, kwargs,
            "O&(ii)|O!", kwlist2,
            &PyBlitzArray_Converter, &data,
            &pos[0], &pos[1],
            &PyBool_Type, &norm
          )
        ){
          Jet_doc.print_usage();
          return -1;
        }
        auto _ = make_safe(data);
        if (data->type_num != NPY_COMPLEX128 || data->ndim != 3) {
          PyErr_Format(PyExc_TypeError, "`%s' only supports 128-bit complex 3D arrays for property `trafo_image'", Py_TYPE(self)->tp_name);
          Jet_doc.print_usage();
          return -1;
        }
        self->cxx.reset(new bob::ip::gabor::Jet(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,3>(data), pos, PyObject_IsTrue(norm)));
      } else {
        // third variant
        if (
          !PyArg_ParseTupleAndKeywords(
            args, kwargs,
            "O&|O!", kwlist3,
            &PyBlitzArray_Converter, &data,
            &PyBool_Type, &norm
          )
        ){
          Jet_doc.print_usage();
          return -1;
        }
        auto _ = make_safe(data);
        if (data->type_num != NPY_COMPLEX128 || data->ndim != 1) {
          PyErr_Format(PyExc_TypeError, "`%s' only supports 128-bit complex 1D arrays for property `complex'", Py_TYPE(self)->tp_name);
          Jet_doc.print_usage();
          return -1;
        }
        self->cxx.reset(new bob::ip::gabor::Jet(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,1>(data), PyObject_IsTrue(norm)));
      }
    }
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return -1;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot create Gabor jet: unknown exception caught", Py_TYPE(self)->tp_name);
    return -1;
  }

  return 0;
}

static void PyBobIpGaborJet_delete(PyBobIpGaborJetObject* self) {
  Py_TYPE(self)->tp_free((PyObject*)self);
}

int PyBobIpGaborJet_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobIpGaborJetType));
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

static auto abs_doc = bob::extension::VariableDoc(
  "abs",
  "array(float,1D)",
  "The list of absolute values of the Gabor jet"
);
PyObject* PyBobIpGaborJet_abs(PyBobIpGaborJetObject* self, void*){
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->abs());
}

static auto phase_doc = bob::extension::VariableDoc(
  "phase",
  "array(float,1D)",
  "The list of phase values of the Gabor jet"
);
PyObject* PyBobIpGaborJet_phase(PyBobIpGaborJetObject* self, void*){
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->phase());
}

static auto jet_doc = bob::extension::VariableDoc(
  "jet",
  "array(float,2D)",
  "The absolute and phase values of the Gabor jet",
  "The absolute values are stored in the first row ``jet[0,:]``, while the phase values are stored in the second row ``jet[1,:]``"
);
PyObject* PyBobIpGaborJet_jet(PyBobIpGaborJetObject* self, void*){
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->jet());
}

static auto complex_doc = bob::extension::VariableDoc(
  "complex",
  "array(complex,1D)",
  "The complex representation of the Gabor jet",
  ".. note:: The complex representation is generated on the fly and is not stored anywhere in the object."
);
PyObject* PyBobIpGaborJet_complex(PyBobIpGaborJetObject* self, void*){
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->complex());
}


static PyGetSetDef PyBobIpGaborJet_getseters[] = {
  {
    abs_doc.name(),
    (getter)PyBobIpGaborJet_abs,
    0,
    abs_doc.doc(),
    0
  },
  {
    phase_doc.name(),
    (getter)PyBobIpGaborJet_phase,
    0,
    phase_doc.doc(),
    0
  },
  {
    jet_doc.name(),
    (getter)PyBobIpGaborJet_jet,
    0,
    jet_doc.doc(),
    0
  },
  {
    complex_doc.name(),
    (getter)PyBobIpGaborJet_complex,
    0,
    complex_doc.doc(),
    0
  },
  {0}  /* Sentinel */
};


/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/

static auto normalize_doc = bob::extension::FunctionDoc(
  "normalize",
  "Normalizes the Gabor jet to unit Euclidean length and returns its old length",
  ".. note:: Only the absolute values :py:attr:`abs` are affected by the normalization",
  true
)
.add_prototype("", "old_norm")
.add_return("old_norm", "float", "The Euclidean length of the Gabor jet **before** normalization")
;

static PyObject* PyBobIpGaborJet_normalize(PyBobIpGaborJetObject* self, PyObject* args, PyObject* kwargs) {

  static char* kwlist[] = {0};


  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "", kwlist)){
    normalize_doc.print_usage();
    return 0;
  }

  try{
    double norm = self->cxx->normalize();
    return Py_BuildValue("d", norm);
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot normalize Gabor jet: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }
}

static auto load_doc = bob::extension::FunctionDoc(
  "load",
  "Loads the Gabor jet from the given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", "xbob.io.HDF5File", "An HDF5 file opened for reading")
;

static PyObject* PyBobIpGaborJet_load(PyBobIpGaborJetObject* self, PyObject* args, PyObject* kwargs) {
  // get list of arguments
  char* kwlist[] = {c("hdf5"), NULL};
  PyBobIoHDF5FileObject* file = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs,
        "O&", kwlist,
        PyBobIoHDF5File_Converter, &file
  )){
    load_doc.print_usage();
    return NULL;
  }

  auto file_ = make_safe(file);
  try{
    self->cxx->load(*file->f);
  } catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot load Gabor jet: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }
  Py_RETURN_NONE;
}


static auto save_doc = bob::extension::FunctionDoc(
  "save",
  "Saves the Gabor jet to the given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for writing")
;

static PyObject* PyBobIpGaborJet_save(PyBobIpGaborJetObject* self, PyObject* args, PyObject* kwargs) {
  // get list of arguments
  char* kwlist[] = {c("hdf5"), NULL};
  PyBobIoHDF5FileObject* file = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs,
        "O&", kwlist,
        PyBobIoHDF5File_Converter, &file
    )
  ){
    save_doc.print_usage();
    return NULL;
  }

  auto file_ = make_safe(file);
  try{
    self->cxx->save(*file->f);
  } catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot save Gabor jet: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }

  Py_RETURN_NONE;
}


static PyMethodDef PyBobIpGaborJet_methods[] = {
  {
    normalize_doc.name(),
    (PyCFunction)PyBobIpGaborJet_normalize,
    METH_VARARGS|METH_KEYWORDS,
    normalize_doc.doc()
  },
  {
    load_doc.name(),
    (PyCFunction)PyBobIpGaborJet_load,
    METH_VARARGS|METH_KEYWORDS,
    load_doc.doc()
  },
  {
    save_doc.name(),
    (PyCFunction)PyBobIpGaborJet_save,
    METH_VARARGS|METH_KEYWORDS,
    save_doc.doc()
  },
  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the Gabor wavelet type struct; will be initialized later
PyTypeObject PyBobIpGaborJetType = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobIpGaborJet(PyObject* module)
{

  // initialize the Gabor wavelet type struct
  PyBobIpGaborJetType.tp_name = Jet_doc.name();
  PyBobIpGaborJetType.tp_basicsize = sizeof(PyBobIpGaborJetObject);
  PyBobIpGaborJetType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobIpGaborJetType.tp_doc = Jet_doc.doc();

  // set the functions
  PyBobIpGaborJetType.tp_new = PyType_GenericNew;
  PyBobIpGaborJetType.tp_init = reinterpret_cast<initproc>(PyBobIpGaborJet_init);
  PyBobIpGaborJetType.tp_dealloc = reinterpret_cast<destructor>(PyBobIpGaborJet_delete);
  PyBobIpGaborJetType.tp_methods = PyBobIpGaborJet_methods;
  PyBobIpGaborJetType.tp_getset = PyBobIpGaborJet_getseters;

  // check that everyting is fine
  if (PyType_Ready(&PyBobIpGaborJetType) < 0)
    return false;

  // add the type to the module
  Py_INCREF(&PyBobIpGaborJetType);
  return PyModule_AddObject(module, "Jet", (PyObject*)&PyBobIpGaborJetType) >= 0;
}

