/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief Bindings for a Gabor wavelet transform
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#define BOB_IP_GABOR_MODULE
#include <bob.ip.gabor/api.h>

#include <bob.blitz/cppapi.h>
#include <bob.blitz/cleanup.h>
#include <bob.io.base/api.h>
#include <bob.extension/documentation.h>


static inline char* c(const char* o){return const_cast<char*>(o);}

#if PY_VERSION_HEX >= 0x03000000
#define PyInt_Check PyLong_Check
#endif

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
    "* The first constructor will create an uninitialized Gabor jet of the desired size\n"
    "* The second constructor will extract a Gabor jet from the given Gabor transformed image at the specified location\n"
    "* The third constructor will create a Gabor jet from a list of complex values\n"
    "* The fourth constructor will average the complex values of the given Gabor jets\n"
    "* The fifth constructor will load the Gabor jet from the given :py:class:`bob.io.base.HDF5File`\n"
    "* The last constructor will copy the information stored in the given :py:class:`Jet`\n",
    true
  )
  .add_prototype("[length]", "")
  .add_prototype("trafo_image, position, [normalize]", "")
  .add_prototype("complex, [normalize]", "")
  .add_prototype("to_average, [normalize]", "")
  .add_prototype("hdf5", "")
  .add_prototype("jet", "")
  .add_parameter("length", "int", "[default: 0] Creates an empty Gabor jet of the given length")
  .add_parameter("trafo_image", "array_like(complex, 3D)", "The result of the Gabor wavelet transform, i.e., of :py:func:`bob.ip.gabor.Transform.transform`")
  .add_parameter("position", "(int, int)", "The position, where the Gabor jet should be extracted")
  .add_parameter("complex", "array_like(complex, 3D)", "The complex-valued representation of a Gabor jet")
  .add_parameter("to_average", "[:py:class:`bob.ip.gabor.Jet`]", "Computes the average of the given Gabor jets")
  .add_parameter("normalize", "bool", "[default: True] Should the newly generated Gabor jet be normalized to unit Euclidean length?")
  .add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for reading to load the Gabor jet from")
  .add_parameter("jet", ":py:class:`bob.ip.gabor.Jet`", "The Gabor jet to copy-construct")
);

static int PyBobIpGaborJet_init(PyBobIpGaborJetObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist0 = Jet_doc.kwlist(0); // length
  char** kwlist1 = Jet_doc.kwlist(4); // hdf5
  char** kwlist2 = Jet_doc.kwlist(3); // to_average
  char** kwlist3 = Jet_doc.kwlist(2); // complex
  char** kwlist4 = Jet_doc.kwlist(1); // trafo_image
  char** kwlist5 = Jet_doc.kwlist(5); // jet

  Py_ssize_t nargs = (args?PyTuple_Size(args):0) + (kwargs?PyDict_Size(kwargs):0);

  int which = -1;

  switch (nargs){
    case 0:{
      which = 0;
      break;
    }

    case 1:{
      // one argument; might be length, hdf5, complex or to_average
      if (args && PyTuple_Size(args) == 1){
        PyObject* v = PyTuple_GET_ITEM(args, 0);
        if (PyInt_Check(v)) which = 0;
        else if (PyBobIoHDF5File_Check(v)) which = 1;
        else if (PyList_Check(v) || PyTuple_Check(v) || PyIter_Check(v)) which = 2;
        else if (PyBlitzArray_Check(v) || PyArray_Check(v)) which = 3;
        else if (PyBobIpGaborJet_Check(v)) which = 5;
        else{
          PyErr_Format(PyExc_RuntimeError, "`%s' constructor with called unknown first parameter", Py_TYPE(self)->tp_name);
          return -1;
        }
      } else {
        // called via dict
        PyObject* k[] = {Py_BuildValue("s", kwlist0[0]), Py_BuildValue("s", kwlist1[0]), Py_BuildValue("s", kwlist2[0]), Py_BuildValue("s", kwlist3[0]), Py_BuildValue("s", kwlist5[0])};
        auto k0_ = make_safe(k[0]), k1_ = make_safe(k[1]), k2_ = make_safe(k[2]), k3_ = make_safe(k[3]), k4_ = make_safe(k[4]);
        if (PyDict_Contains(kwargs, k[0])) which = 0;
        else if (PyDict_Contains(kwargs, k[1])) which = 1;
        else if (PyDict_Contains(kwargs, k[2])) which = 2;
        else if (PyDict_Contains(kwargs, k[3])) which = 3;
        else if (PyDict_Contains(kwargs, k[4])) which = 5;
        else{
          PyErr_Format(PyExc_RuntimeError, "`%s' constructor called with unknown first parameter", Py_TYPE(self)->tp_name);
          return -1;
        }
      }
      break;
    }

    case 2:{
      // two arguments; might be to_average, complex or trafo_image
      if (args && PyTuple_Size(args) >= 1){
        PyObject* v = PyTuple_GET_ITEM(args, 0);
        if (PyList_Check(v) || PyTuple_Check(v) || PyIter_Check(v)) which = 2;
        else if (PyBlitzArray_Check(v) || PyArray_Check(v)){
          // can be complex or trafo image
          if (PyTuple_Size(args) == 2){
            PyObject* v2 = PyTuple_GET_ITEM(args, 1);
            if (PyTuple_Check(v2) || PyList_Check(v2)) which = 4;
            else if (PyBool_Check(v2)) which = 3;
            else{
              PyErr_Format(PyExc_RuntimeError, "`%s' constructor called with unknown second parameter", Py_TYPE(self)->tp_name);
              return -1;
            }
          } else { // args != 2
            PyObject* k[] = {Py_BuildValue("s", kwlist3[1]), Py_BuildValue("s", kwlist4[1])};
            auto k0_ = make_safe(k[0]), k1_ = make_safe(k[1]);
            if (PyDict_Contains(kwargs, k[0])) which = 3;
            else if (PyDict_Contains(kwargs, k[1])) which = 4;
            else{
              PyErr_Format(PyExc_RuntimeError, "`%s' constructor called  with unknown second parameter", Py_TYPE(self)->tp_name);
              return -1;
            }
          }
        }else{
          PyErr_Format(PyExc_RuntimeError, "`%s' constructor with unknown first parameter", Py_TYPE(self)->tp_name);
          return -1;
        }
      } else { // only dict
        PyObject* k[] = {Py_BuildValue("s", kwlist2[0]), Py_BuildValue("s", kwlist3[0]), Py_BuildValue("s", kwlist4[0]), Py_BuildValue("s", kwlist3[1]), Py_BuildValue("s", kwlist4[1])};
        auto k0_ = make_safe(k[0]), k1_ = make_safe(k[1]), k2_ = make_safe(k[2]), k3_ = make_safe(k[3]), k4_ = make_safe(k[4]);
        if (PyDict_Contains(kwargs, k[0]) && PyDict_Contains(kwargs, k[3])) which = 2;
        else if (PyDict_Contains(kwargs, k[1]) && PyDict_Contains(kwargs, k[3])) which = 3;
        else if (PyDict_Contains(kwargs, k[2]) && PyDict_Contains(kwargs, k[4])) which = 4;
        else{
          PyErr_Format(PyExc_RuntimeError, "`%s' constructor called  with unknown keyword arguments", Py_TYPE(self)->tp_name);
          return -1;
        }
      }
      break;
    }

    case 3:
      which = 4;
      break;

    default:
      PyErr_Format(PyExc_RuntimeError, "`%s' constructor called with too many arguments", Py_TYPE(self)->tp_name);
      return -1;
  } // switch

  // now, there are five ways to call
  switch (which){
    case 0:{// length
      int len = 0;
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", kwlist0, &len)){
        return -1;
      }
      self->cxx.reset(new bob::ip::gabor::Jet(len));
      return 0;
    }
    case 1: { // HDF5
      PyBobIoHDF5FileObject* hdf5;
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist1, &PyBobIoHDF5File_Converter, &hdf5)){
        return -1;
      }
      auto hdf5_ = make_safe(hdf5);
      self->cxx.reset(new bob::ip::gabor::Jet(*hdf5->f));
      return 0;
    }
    case 2:{ // averaging
      PyObject* jets,* norm = 0;
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O!", kwlist2, &jets, &PyBool_Type, &norm)){
        return -1;
      }
      std::vector<boost::shared_ptr<bob::ip::gabor::Jet>> data;
      PyObject* iterator = PyObject_GetIter(jets);
      if (!iterator) return -1;
      auto iterator_ = make_safe(iterator);
      int i = 0;
      while (PyObject* it = PyIter_Next(iterator)) {
        auto it_ = make_safe(it);
        if (!PyBobIpGaborJet_Check(it)){
          PyErr_Format(PyExc_RuntimeError, "`%s' requires all elements of the `to_average` parameter to be of type bob.ip.gabor.Jet, but element %d isn't", Py_TYPE(self)->tp_name, i);
          return -1;
        }
        data.push_back(reinterpret_cast<PyBobIpGaborJetObject*>(it)->cxx);
        ++i;
      }
      self->cxx.reset(new bob::ip::gabor::Jet(data, !norm || PyObject_IsTrue(norm)));
      return 0;
    }
    case 3:{ // complex
      PyBlitzArrayObject* data;
      PyObject* norm = 0;
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&|O!", kwlist3, &PyBlitzArray_Converter, &data, &PyBool_Type, &norm)){
        return -1;
      }
      auto _ = make_safe(data);
      if (data->type_num != NPY_COMPLEX128 || data->ndim != 1) {
        PyErr_Format(PyExc_TypeError, "`%s' only supports 128-bit complex 1D arrays for property `complex'", Py_TYPE(self)->tp_name);
        return -1;
      }
      self->cxx.reset(new bob::ip::gabor::Jet(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,1>(data), !norm || PyObject_IsTrue(norm)));
      return 0;
    }
    case 4:{ // jet_image and position
      PyBlitzArrayObject* data;
      PyObject* norm = 0;
      blitz::TinyVector<int,2> pos;
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&(ii)|O!", kwlist4, &PyBlitzArray_Converter, &data, &pos[0], &pos[1], &PyBool_Type, &norm)){
        return -1;
      }
      auto _ = make_safe(data);
      if (data->type_num != NPY_COMPLEX128 || data->ndim != 3) {
        PyErr_Format(PyExc_TypeError, "`%s' only supports 128-bit complex 3D arrays for property `trafo_image'", Py_TYPE(self)->tp_name);
        return -1;
      }
      self->cxx.reset(new bob::ip::gabor::Jet(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,3>(data), pos, !norm || PyObject_IsTrue(norm)));
      return 0;
    }
    case 5:{
      // copy-construct
      PyBobIpGaborJetObject* jet;
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist1, &PyBobIpGaborJet_Type, &jet)){
        return -1;
      }
      self->cxx.reset(new bob::ip::gabor::Jet(*jet->cxx));
      return 0;
    }
    default:
      PyErr_Format(PyExc_TypeError, "`%s' unknown error during construction", Py_TYPE(self)->tp_name);
      return -1;
  } // switch
BOB_CATCH_MEMBER("Jet constructor", -1)
}

static void PyBobIpGaborJet_delete(PyBobIpGaborJetObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}

int PyBobIpGaborJet_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobIpGaborJet_Type));
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

static auto abs_doc = bob::extension::VariableDoc(
  "abs",
  "array(float,1D)",
  "The list of absolute values of the Gabor jet\n\n"
  ".. note::\n\n  These values cannot be modified. Use :py:attr:`jet` instead."
);
PyObject* PyBobIpGaborJet_abs(PyBobIpGaborJetObject* self, void*){
BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->abs());
BOB_CATCH_MEMBER("abs", 0)
}

static auto phase_doc = bob::extension::VariableDoc(
  "phase",
  "array(float,1D)",
  "The list of phase values of the Gabor jet\n\n"
  ".. note::\n\n  These values cannot be modified. Use :py:attr:`jet` instead."
);
PyObject* PyBobIpGaborJet_phase(PyBobIpGaborJetObject* self, void*){
BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->phase());
BOB_CATCH_MEMBER("phase", 0)
}

static auto jet_doc = bob::extension::VariableDoc(
  "jet",
  "array(float,2D)",
  "The absolute and phase values of the Gabor jet",
  "The absolute values are stored in the first row ``jet[0,:]``, while the phase values are stored in the second row ``jet[1,:]``\n\n"
  ".. note::\n\n  Use this function to modify the Gabor jet, if required."
);
PyObject* PyBobIpGaborJet_jet(PyBobIpGaborJetObject* self, void*){
  return PyBlitzArrayCxx_AsNumpy(self->cxx->jet());
}

static auto complex_doc = bob::extension::VariableDoc(
  "complex",
  "array(complex,1D)",
  "The complex representation of the Gabor jet",
  ".. note::\n\n  The complex representation is generated on the fly and is not stored anywhere in the object."
);
PyObject* PyBobIpGaborJet_complex(PyBobIpGaborJetObject* self, void*){
BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->complex());
BOB_CATCH_MEMBER("complex", 0)
}

static auto length_doc = bob::extension::VariableDoc(
  "length",
  "int",
  "The number of elements in the Gabor jet\n\n"
  ".. note:: You can also use the `len(jet)` function to get the length of the Gabor jet"
);
PyObject* PyBobIpGaborJet_length(PyBobIpGaborJetObject* self, void*){
BOB_TRY
  return Py_BuildValue("i", self->cxx->length());
BOB_CATCH_MEMBER("length", 0)
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
  {
    length_doc.name(),
    (getter)PyBobIpGaborJet_length,
    0,
    length_doc.doc(),
    0
  },
  {0}  /* Sentinel */
};

/******************************************************************/
/************ Special Members Section *****************************/
/******************************************************************/

Py_ssize_t PyBobIpGaborJet_len(PyObject* self){
  return reinterpret_cast<PyBobIpGaborJetObject*>(self)->cxx->length();
}

static PySequenceMethods PyBobIpGaborJet_sequence_methods = {
  PyBobIpGaborJet_len,                  /* sq_length */
  0                                     /* Sentinel */
};


/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/

static auto normalize_doc = bob::extension::FunctionDoc(
  "normalize",
  "Normalizes the Gabor jet to unit Euclidean length and returns its old length",
  ".. note::\n\n  Only the absolute values :py:attr:`abs` are affected by the normalization",
  true
)
.add_prototype("", "old_norm")
.add_return("old_norm", "float", "The Euclidean length of the Gabor jet **before** normalization")
;

static PyObject* PyBobIpGaborJet_normalize(PyBobIpGaborJetObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = normalize_doc.kwlist();

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "", kwlist)) return 0;

  double norm = self->cxx->normalize();
  return Py_BuildValue("d", norm);
BOB_CATCH_MEMBER("normalize", 0)
}

static auto init_doc = bob::extension::FunctionDoc(
  "init",
  "Initializes the Gabor jet with the given complex-valued data",
  0,
  true
)
.add_prototype("complex, [normalize]")
.add_parameter("complex", "array_like(complex,1D)", "The vector of complex data to initialize the Gabor jet with")
.add_parameter("normalize", "bool", "[default: True] Should the newly generated Gabor jet be normalized to unit Euclidean length?")
;
static PyObject* PyBobIpGaborJet_init_(PyBobIpGaborJetObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = init_doc.kwlist();

  PyBlitzArrayObject* data;
  PyObject* norm = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&|O!", kwlist, &PyBlitzArray_Converter, &data, &PyBool_Type, &norm)) return 0;

  auto _ = make_safe(data);
  if (data->type_num != NPY_COMPLEX128 || data->ndim != 1) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 128-bit complex 1D arrays for property `complex'", Py_TYPE(self)->tp_name);
    return 0;
  }
  self->cxx->init(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,1>(data), !norm || PyObject_IsTrue(norm));
  Py_RETURN_NONE;
BOB_CATCH_MEMBER("init", 0)
}

static auto extract_doc = bob::extension::FunctionDoc(
  "extract",
  "Initializes the Gabor jet with the given complex-valued data extracted from the given trafo image at the given position",
  0,
  true
)
.add_prototype("trafo_image, position, [normalize]")
.add_parameter("trafo_image", "array_like(complex, 3D)", "The result of the Gabor wavelet transform, i.e., of :py:func:`bob.ip.gabor.Transform.transform`")
.add_parameter("position", "(int, int)", "The position, where the Gabor jet should be extracted")
.add_parameter("normalize", "bool", "[default: True] Should the newly generated Gabor jet be normalized to unit Euclidean length?")
;
static PyObject* PyBobIpGaborJet_extract(PyBobIpGaborJetObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = extract_doc.kwlist();

  PyBlitzArrayObject* data;
  PyObject* norm = 0;
  blitz::TinyVector<int,2> pos;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&(ii)|O!", kwlist, &PyBlitzArray_Converter, &data, &pos[0], &pos[1], &PyBool_Type, &norm)) return 0;

  auto _ = make_safe(data);
  if (data->type_num != NPY_COMPLEX128 || data->ndim != 3) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 128-bit complex 3D arrays for property `trafo_image'", Py_TYPE(self)->tp_name);
    return 0;
  }
  self->cxx->extract(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,3>(data), pos, !norm || PyObject_IsTrue(norm));
  Py_RETURN_NONE;
BOB_CATCH_MEMBER("extract", 0)
}


static auto load_doc = bob::extension::FunctionDoc(
  "load",
  "Loads the Gabor jet from the given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file opened for reading")
;
static PyObject* PyBobIpGaborJet_load(PyBobIpGaborJetObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  // get list of arguments
  char** kwlist = load_doc.kwlist();
  PyBobIoHDF5FileObject* file = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist, PyBobIoHDF5File_Converter, &file)) return 0;

  auto file_ = make_safe(file);
  self->cxx->load(*file->f);
  Py_RETURN_NONE;
BOB_CATCH_MEMBER("load", 0)
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
BOB_TRY
  // get list of arguments
  char** kwlist = save_doc.kwlist();
  PyBobIoHDF5FileObject* file = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist, PyBobIoHDF5File_Converter, &file)) return 0;

  auto file_ = make_safe(file);
  self->cxx->save(*file->f);
  Py_RETURN_NONE;
BOB_CATCH_MEMBER("save", 0)
}


static PyMethodDef PyBobIpGaborJet_methods[] = {
  {
    normalize_doc.name(),
    (PyCFunction)PyBobIpGaborJet_normalize,
    METH_VARARGS|METH_KEYWORDS,
    normalize_doc.doc()
  },
  {
    init_doc.name(),
    (PyCFunction)PyBobIpGaborJet_init_,
    METH_VARARGS|METH_KEYWORDS,
    init_doc.doc()
  },
  {
    extract_doc.name(),
    (PyCFunction)PyBobIpGaborJet_extract,
    METH_VARARGS|METH_KEYWORDS,
    extract_doc.doc()
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
PyTypeObject PyBobIpGaborJet_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobIpGaborJet(PyObject* module)
{

  // initialize the Jet type struct
  PyBobIpGaborJet_Type.tp_name = Jet_doc.name();
  PyBobIpGaborJet_Type.tp_basicsize = sizeof(PyBobIpGaborJetObject);
  PyBobIpGaborJet_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobIpGaborJet_Type.tp_doc = Jet_doc.doc();

  // set the functions
  PyBobIpGaborJet_Type.tp_new = PyType_GenericNew;
  PyBobIpGaborJet_Type.tp_init = reinterpret_cast<initproc>(PyBobIpGaborJet_init);
  PyBobIpGaborJet_Type.tp_dealloc = reinterpret_cast<destructor>(PyBobIpGaborJet_delete);
  PyBobIpGaborJet_Type.tp_methods = PyBobIpGaborJet_methods;
  PyBobIpGaborJet_Type.tp_getset = PyBobIpGaborJet_getseters;
  PyBobIpGaborJet_Type.tp_as_sequence = &PyBobIpGaborJet_sequence_methods;

  // check that everyting is fine
  if (PyType_Ready(&PyBobIpGaborJet_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobIpGaborJet_Type);
  return PyModule_AddObject(module, "Jet", (PyObject*)&PyBobIpGaborJet_Type) >= 0;
}
