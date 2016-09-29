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


/******************************************************************/
/************ Constructor Section *********************************/
/******************************************************************/

static auto Transform_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".Transform",
  "A class that represents a family of Gabor wavelets that can be used to perform a Gabor wavelet transform",
  "The implementation is according to [Guenther2011]_, where the whole procedure is explained in more detail.\n\n"
  ".. note::\n\n  In opposition to the :py:class:`Wavelet` class, here all input and output images are considered to be in spatial domain."
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Creates a Gabor wavelet transform for the given parametrization",
    "The family of Gabor wavelets :math:`\\check\\psi_{\\vec k_j}` is created by considering several center frequencies :math:`\\vec k_j`:\n\n"
    ".. math::\n"
    "   \\vec k_j = k_{\\zeta} \\left\\{ \\begin{array}{c} \\cos \\vartheta_{\\nu} \\\\ \\sin \\vartheta_{\\nu} \\end{array}\\right\\}\n\n"
    "where :math:`k_{\\zeta} = k_{max} \\cdot k_{fac}^{\\zeta}` with :math:`\\zeta = \\{0,\\dots,\\zeta_{max}-1\\}` and :math:`\\vartheta_{\\nu} = \\frac{\\nu \\cdot 2\\pi}{\\nu_{max}}` and :math:`\\nu = \\{0,\\dots,\\nu_{max}-1\\}`",
    true
  )
  .add_prototype("[number_of_scales], [number_of_directions], [sigma], [k_max], [k_fac], [power_of_k], [dc_free], [epsilon]", "")
  .add_prototype("hdf5", "")
  .add_parameter("number_of_scales", "int", "[default: 5] The number of scales :math:`\\zeta_{max}` of Gabor wavelets that should be created")
  .add_parameter("number_of_directions", "int", "[default: 8] The number of directions :math:`\\nu_{max}` of Gabor wavelets that should be created")
  .add_parameter("sigma", "float", "[default: :math:`2\\pi`] The spatial resolution :math:`\\sigma` of the Gabor wavelets")
  .add_parameter("k_max", "float", "[default: :math:`\\pi/2`] The highest frequency (the lowest scale) of Gabor wavelets; the default will resemble to the smallest wavelength of 4 pixels")
  .add_parameter("k_fac", "float", "[default: :math:`\\sqrt{\\frac12}`] The (logarithmic) distance between two scales of Gabor wavelets")
  .add_parameter("power_of_k", "float", "[default: 0] The :math:`\\lambda` factor to regularize the Gabor wavelet prefactor to generate comparable results for images, see Appendix C of [Guenther2011]_")
  .add_parameter("dc_free", "bool", "[default: True] Should the Gabor wavelet be without DC factor (i.e. should the integral under the wavelet in spatial domain vanish)?")
  .add_parameter("epsilon", "float", "[default: 1e-10] For speed reasons: all wavelet pixels in frequency domain with an absolute value below this should be considered as 0")
  .add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for reading to load the parametrization of the Gabor wavelet transform from")
);

static int PyBobIpGaborTransform_init(PyBobIpGaborTransformObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist1 = Transform_doc.kwlist(1);
  char** kwlist2 = Transform_doc.kwlist(0);

  // two ways to call
  PyObject* k = Py_BuildValue("s", kwlist1[0]);
  auto k_ = make_safe(k);
  if (
    (kwargs && PyDict_Contains(kwargs, k)) ||
    (args && PyTuple_Size(args) == 1 && PyBobIoHDF5File_Check(PyTuple_GetItem(args, 0)))
  ){
    PyBobIoHDF5FileObject* hdf5;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist1, &PyBobIoHDF5File_Converter, &hdf5)) return -1;
    auto hdf5_ = make_safe(hdf5);

    self->cxx.reset(new bob::ip::gabor::Transform(*hdf5->f));
  } else {
    int scales = 5, directions = 8;
    double sigma = 2.*M_PI, k_max = M_PI/2., k_fac = sqrt(.5), pow_k = 0., eps=1e-10;
    PyObject* dc = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|iiddddO!d", kwlist2, &scales, &directions, &sigma, &k_max, &k_fac, &pow_k, &PyBool_Type, &dc, &eps)) return -1;
    self->cxx.reset(new bob::ip::gabor::Transform(scales, directions, sigma, k_max, k_fac, pow_k, !dc || PyObject_IsTrue(dc), eps));
  }
  return 0;
BOB_CATCH_MEMBER("Transform constructor", -1)
}

static void PyBobIpGaborTransform_delete(PyBobIpGaborTransformObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}

int PyBobIpGaborTransform_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobIpGaborTransform_Type));
}

static PyObject* PyBobIpGaborTransform_RichCompare(PyBobIpGaborTransformObject* self, PyObject* other, int op) {
BOB_TRY
  if (!PyBobIpGaborTransform_Check(other)) {
    PyErr_Format(PyExc_TypeError, "cannot compare `%s' with `%s'", Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
    return 0;
  }
  auto other_ = reinterpret_cast<PyBobIpGaborTransformObject*>(other);
  switch (op) {
    case Py_EQ:
      if (*self->cxx==*other_->cxx) Py_RETURN_TRUE; else Py_RETURN_FALSE;
    case Py_NE:
      if (*self->cxx==*other_->cxx) Py_RETURN_FALSE; else Py_RETURN_TRUE;
    default:
      Py_INCREF(Py_NotImplemented);
      return Py_NotImplemented;
  }
  return 0;
BOB_CATCH_MEMBER("RichCompare", 0)
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

static auto numberOfWavelets_doc = bob::extension::VariableDoc(
  "number_of_wavelets",
  "int",
  "The number of Gabor wavelets (i.e, the number of directions times the number of scales) of this class"
);
PyObject* PyBobIpGaborTransform_numberOfWavelets(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  return Py_BuildValue("i", self->cxx->numberOfWavelets());
BOB_CATCH_MEMBER("abs", 0)
}

static auto numberOfScales_doc = bob::extension::VariableDoc(
  "number_of_scales",
  "int",
  "The number of scales (levels) of Gabor wavelets"
);
PyObject* PyBobIpGaborTransform_numberOfScales(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  return Py_BuildValue("i", self->cxx->numberOfScales());
BOB_CATCH_MEMBER("number_of_scales", 0)
}

static auto numberOfDirections_doc = bob::extension::VariableDoc(
  "number_of_directions",
  "int",
  "The number of directions (orientations) of Gabor wavelets"
);
PyObject* PyBobIpGaborTransform_numberOfDirections(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  return Py_BuildValue("i", self->cxx->numberOfDirections());
BOB_CATCH_MEMBER("number_of_directions", 0)
}

static auto sigma_doc = bob::extension::VariableDoc(
  "sigma",
  "float",
  "The extend of the Gabor wavelets"
);
PyObject* PyBobIpGaborTransform_sigma(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  return Py_BuildValue("d", self->cxx->sigma());
BOB_CATCH_MEMBER("sigma", 0)
}

static auto k_max_doc = bob::extension::VariableDoc(
  "k_max",
  "float",
  "The highest frequency of Gabor wavelets"
);
PyObject* PyBobIpGaborTransform_k_max(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  return Py_BuildValue("d", self->cxx->k_max());
BOB_CATCH_MEMBER("k_max", 0)
}

static auto k_fac_doc = bob::extension::VariableDoc(
  "k_fac",
  "float",
  "The logarithmic distance between two levels of Gabor wavelets"
);
PyObject* PyBobIpGaborTransform_k_fac(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  return Py_BuildValue("d", self->cxx->k_fac());
BOB_CATCH_MEMBER("k_fac", 0)
}

static auto power_of_k_doc = bob::extension::VariableDoc(
  "power_of_k",
  "float",
  "The adaptation of the Gabor wavelet scales to get homogeneous values"
);
PyObject* PyBobIpGaborTransform_power_of_k(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  return Py_BuildValue("d", self->cxx->pow_of_k());
BOB_CATCH_MEMBER("power_of_k", 0)
}

static auto dc_free_doc = bob::extension::VariableDoc(
  "dc_free",
  "bool",
  "Are the Gabor wavelets DC free?"
);
PyObject* PyBobIpGaborTransform_dc_free(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  return Py_BuildValue("O", self->cxx->dc_free() ? Py_True: Py_False);
BOB_CATCH_MEMBER("dc_free", 0)
}

static auto waveletFrequencies_doc = bob::extension::VariableDoc(
  "wavelet_frequencies",
  "[(float, float), ...]",
  "The central frequencies of the Gabor wavelets, in the same order as in :py:attr:`wavelets`"
);
PyObject* PyBobIpGaborTransform_waveletFrequencies(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  // get the data
  auto data = self->cxx->waveletFrequencies();
  // populate a list
  PyObject* list = PyList_New(data.size());
  for (Py_ssize_t i = 0; i < (Py_ssize_t)data.size(); ++i){
    PyList_SET_ITEM(list, i, Py_BuildValue("(dd)", data[i][0], data[i][1]));
  }
  return list;
BOB_CATCH_MEMBER("wavelet_frequencies", 0)
}

static auto wavelets_doc = bob::extension::VariableDoc(
  "wavelets",
  "[:py:class:`Wavelet`]",
  "The list of Gabor wavelets used in this transform",
  ".. note::\n\n  "
  "The wavelets will be generated either by a call to :py:func:`generate_wavelets` or by a call to :py:func:`transform`. "
  "Before one of these functions is called, no wavelet will be generated."
);
PyObject* PyBobIpGaborTransform_wavelets(PyBobIpGaborTransformObject* self, void*){
BOB_TRY
  // get the data
  auto wavelets = self->cxx->wavelets();
  // populate a list
  PyObject* list = PyList_New(wavelets.size());
  for (Py_ssize_t i = 0; i < (Py_ssize_t)wavelets.size(); ++i){
    PyBobIpGaborWaveletObject* wavelet = (PyBobIpGaborWaveletObject*)PyBobIpGaborWavelet_Type.tp_alloc(&PyBobIpGaborWavelet_Type, 0);
    wavelet->cxx = wavelets[i];
    PyList_SET_ITEM(list, i, Py_BuildValue("N", wavelet));
  }
  return list;
BOB_CATCH_MEMBER("wavelets", 0)
}


static PyGetSetDef PyBobIpGaborTransform_getseters[] = {
  {
    numberOfWavelets_doc.name(),
    (getter)PyBobIpGaborTransform_numberOfWavelets,
    0,
    numberOfWavelets_doc.doc(),
    0
  },
  {
    numberOfScales_doc.name(),
    (getter)PyBobIpGaborTransform_numberOfScales,
    0,
    numberOfScales_doc.doc(),
    0
  },
  {
    numberOfDirections_doc.name(),
    (getter)PyBobIpGaborTransform_numberOfDirections,
    0,
    numberOfDirections_doc.doc(),
    0
  },
  {
    sigma_doc.name(),
    (getter)PyBobIpGaborTransform_sigma,
    0,
    sigma_doc.doc(),
    0
  },
  {
    k_max_doc.name(),
    (getter)PyBobIpGaborTransform_k_max,
    0,
    k_max_doc.doc(),
    0
  },
  {
    k_fac_doc.name(),
    (getter)PyBobIpGaborTransform_k_fac,
    0,
    k_fac_doc.doc(),
    0
  },
  {
    power_of_k_doc.name(),
    (getter)PyBobIpGaborTransform_power_of_k,
    0,
    power_of_k_doc.doc(),
    0
  },
  {
    dc_free_doc.name(),
    (getter)PyBobIpGaborTransform_dc_free,
    0,
    dc_free_doc.doc(),
    0
  },
  {
    waveletFrequencies_doc.name(),
    (getter)PyBobIpGaborTransform_waveletFrequencies,
    0,
    waveletFrequencies_doc.doc(),
    0
  },
  {
    wavelets_doc.name(),
    (getter)PyBobIpGaborTransform_wavelets,
    0,
    wavelets_doc.doc(),
    0
  },
  {0}  /* Sentinel */
};


/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/

static auto transform_doc = bob::extension::FunctionDoc(
  "transform",
  "This function transforms the given input image to the output trafo image",
  "The input image must be of two dimensions and might be of any supported type: ``uint8``, ``float`` or ``complex``, whereas the output needs to be of 3 dimensions with ``complex`` type and of shape (:py:attr:`number_of_wavelets`, input.shape[0], input.shape[1]). "
  "The transform is defined as:\n\n"
  ".. math::\n\n"
  "   \\forall j \\forall \\vec \\omega : \\mathcal T_{\\vec k_j}(\\vec \\omega) = \\mathcal I(\\vec \\omega) \\cdot \\psi_{\\vec k_j}(\\vec \\omega)\n\n"
  "Both the input image and the output are expected to be in spatial domain, so **don't** perform an FFT on the input image before calling this function.\n\n"
  ".. note::\n\n  The function `__call__` is a synonym for this function.",
  true
)
.add_prototype("input, [output]", "output")
.add_parameter("input", "array_like (2D)", "The image in spatial domain that should be transformed")
.add_parameter("output", "array_like (complex, 3D)", "The transformed image in spatial domain that should contain the transformed image; if given, must have shape (:py:attr:`number_of_wavelets`, input.shape[0], input.shape[1])")
.add_return("output", "array_like (complex, 3D)", "The transformed image in spatial domain that will contain the transformed image; will have shape (:py:attr:`number_of_wavelets`, input.shape[0], input.shape[1]); identical to the ``output`` parameter, if given")
;

static PyObject* PyBobIpGaborTransform_transform(PyBobIpGaborTransformObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = transform_doc.kwlist();

  PyBlitzArrayObject* input = 0;
  PyBlitzArrayObject* output = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&|O&", kwlist, &PyBlitzArray_Converter, &input, &PyBlitzArray_OutputConverter, &output)) return 0;

  auto input_ = make_safe(input);
  auto output_ = make_xsafe(output);

  if (output && output->type_num != NPY_COMPLEX128) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 128-bit complex arrays for output array `output'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (input->ndim != 2) {
    PyErr_Format(PyExc_TypeError, "`%s' only accepts 2-dimensional arrays (not %" PY_FORMAT_SIZE_T "dD arrays)", Py_TYPE(self)->tp_name, input->ndim);
    return 0;
  }

  if (output){
    if (output->ndim != 3) {
      PyErr_Format(PyExc_RuntimeError, "`%s' only accepts 2-dimensional arrays (not %" PY_FORMAT_SIZE_T "dD arrays)", Py_TYPE(self)->tp_name, output->ndim);
      return 0;
    }
    if (output->shape[0] != self->cxx->numberOfWavelets() || output->shape[1] != input->shape[0] || output->shape[2] != input->shape[1]){
      PyErr_Format(PyExc_RuntimeError, "The shape of the output image should be (%d,%" PY_FORMAT_SIZE_T "d,%" PY_FORMAT_SIZE_T "d), but is (%" PY_FORMAT_SIZE_T "d,%" PY_FORMAT_SIZE_T "d,%" PY_FORMAT_SIZE_T "d)", self->cxx->numberOfWavelets(), input->shape[0], input->shape[0], output->shape[0], output->shape[1], output->shape[2]);
      return 0;
    }
  }

  /** if ``output`` was not pre-allocated, do it now **/
  if (!output) {
    Py_ssize_t osize[3] = {self->cxx->numberOfWavelets(), input->shape[0], input->shape[1]};
    output = (PyBlitzArrayObject*)PyBlitzArray_SimpleNew(NPY_COMPLEX128, 3, osize);
    output_ = make_safe(output);
  }

  switch (input->type_num){
    case NPY_UINT8:
      self->cxx->transform(*PyBlitzArrayCxx_AsBlitz<uint8_t,2>(input),
          *PyBlitzArrayCxx_AsBlitz<std::complex<double>,3>(output));
      break;
    case NPY_FLOAT64:
      self->cxx->transform(*PyBlitzArrayCxx_AsBlitz<double,2>(input),
          *PyBlitzArrayCxx_AsBlitz<std::complex<double>,3>(output));
      break;
    case NPY_COMPLEX128:
      self->cxx->transform(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,2>(input),
          *PyBlitzArrayCxx_AsBlitz<std::complex<double>,3>(output));
      break;
    default:
      PyErr_Format(PyExc_RuntimeError, "`%s' only supports arrays of type uint8, float and complex for array `input'", Py_TYPE(self)->tp_name);
      return 0;
  }
  return PyBlitzArray_AsNumpyArray(output, 0);
BOB_CATCH_MEMBER("transform", 0)
}


static auto generateWavelets_doc = bob::extension::FunctionDoc(
  "generate_wavelets",
  "This function generates the Gabor wavelets for the given image resolution",
  "This function is only available for convenience, it **does not** need to be called before :py:func:`transform` is called.",
  true
)
.add_prototype("height, width")
.add_parameter("height", "int", "The height of the image to generate the wavelets for")
.add_parameter("width", "int", "The width of the image to generate the wavelets for")
;

static PyObject* PyBobIpGaborTransform_generateWavelets(PyBobIpGaborTransformObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = generateWavelets_doc.kwlist();

  int height, width;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ii", kwlist, &height, &width)) return 0;
  self->cxx->generateWavelets(height, width);
  Py_RETURN_NONE;
BOB_CATCH_MEMBER("generate_wavelets", 0)
}


static auto load_doc = bob::extension::FunctionDoc(
  "load",
  "Loads the parametrization of the Gabor wavelet transform from the given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file opened for reading")
;

static PyObject* PyBobIpGaborTransform_load(PyBobIpGaborTransformObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  // get list of arguments
  char** kwlist = load_doc.kwlist();
  PyBobIoHDF5FileObject* file;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist, PyBobIoHDF5File_Converter, &file)) return 0;

  auto file_ = make_safe(file);
  self->cxx->load(*file->f);
  Py_RETURN_NONE;
BOB_CATCH_MEMBER("load", 0)
}


static auto save_doc = bob::extension::FunctionDoc(
  "save",
  "Saves the parametrization of this Gabor wavelet transform to the given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for writing")
;

static PyObject* PyBobIpGaborTransform_save(PyBobIpGaborTransformObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  // get list of arguments
  char** kwlist = save_doc.kwlist();
  PyBobIoHDF5FileObject* file;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist, PyBobIoHDF5File_Converter, &file)) return 0;

  auto file_ = make_safe(file);
  self->cxx->save(*file->f);
  Py_RETURN_NONE;
BOB_CATCH_MEMBER("save", 0)
}


static PyMethodDef PyBobIpGaborTransform_methods[] = {
  {
    transform_doc.name(),
    (PyCFunction)PyBobIpGaborTransform_transform,
    METH_VARARGS|METH_KEYWORDS,
    transform_doc.doc()
  },
  {
    generateWavelets_doc.name(),
    (PyCFunction)PyBobIpGaborTransform_generateWavelets,
    METH_VARARGS|METH_KEYWORDS,
    generateWavelets_doc.doc()
  },
  {
    load_doc.name(),
    (PyCFunction)PyBobIpGaborTransform_load,
    METH_VARARGS|METH_KEYWORDS,
    load_doc.doc()
  },
  {
    save_doc.name(),
    (PyCFunction)PyBobIpGaborTransform_save,
    METH_VARARGS|METH_KEYWORDS,
    save_doc.doc()
  },
  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the Gabor wavelet type struct; will be initialized later
PyTypeObject PyBobIpGaborTransform_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobIpGaborTransform(PyObject* module)
{

  // initialize the Transform type struct
  PyBobIpGaborTransform_Type.tp_name = Transform_doc.name();
  PyBobIpGaborTransform_Type.tp_basicsize = sizeof(PyBobIpGaborTransformObject);
  PyBobIpGaborTransform_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobIpGaborTransform_Type.tp_doc = Transform_doc.doc();

  // set the functions
  PyBobIpGaborTransform_Type.tp_new = PyType_GenericNew;
  PyBobIpGaborTransform_Type.tp_init = reinterpret_cast<initproc>(PyBobIpGaborTransform_init);
  PyBobIpGaborTransform_Type.tp_dealloc = reinterpret_cast<destructor>(PyBobIpGaborTransform_delete);
  PyBobIpGaborTransform_Type.tp_methods = PyBobIpGaborTransform_methods;
  PyBobIpGaborTransform_Type.tp_getset = PyBobIpGaborTransform_getseters;
  PyBobIpGaborTransform_Type.tp_call = reinterpret_cast<ternaryfunc>(PyBobIpGaborTransform_transform);
  PyBobIpGaborTransform_Type.tp_richcompare = reinterpret_cast<richcmpfunc>(PyBobIpGaborTransform_RichCompare);

  // check that everyting is fine
  if (PyType_Ready(&PyBobIpGaborTransform_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobIpGaborTransform_Type);
  return PyModule_AddObject(module, "Transform", (PyObject*)&PyBobIpGaborTransform_Type) >= 0;
}
