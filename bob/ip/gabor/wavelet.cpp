/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Tue Jun  3 16:59:16 CEST 2014
 *
 * @brief Bindings for a Gabor wavelet
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

static auto wavelet_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".Wavelet",
  "A class that represents a Gabor wavelet in frequency domain",
  "The implementation is according to [Guenther2011]_, where the whole procedure is explained in more detail.\n\n"
  ".. note::\n\n  In this class, all input and output images are considered to be in frequency domain."
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Creates a Gabor wavelet in frequency domain for the given parametrization",
    "The Gabor wavelet :math:`\\check\\psi_{\\vec k}` is created in frequency domain as a Gaussian that is shifted from the origin of the frequency domain:\n\n"
    ".. math::\n"
    "   \\check\\psi_{\\vec k}(\\vec \\omega) = k^{\\lambda} \\left\\{ e^{-\\frac{\\sigma^2(\\vec \\omega - \\vec k)^2}{2\\vec k^2}} - {e^{-\\frac{\\sigma^2(\\vec \\omega^2 + \\vec k^2)}{2\\vec k^2}}}\\right\\}",
    true
  )
  .add_prototype("resolution, frequency, [sigma], [power_of_k], [dc_free], [epsilon]", "")
  .add_parameter("resolution", "(int, int)", "The resolution (height, width) of the Gabor wavelet; this must be the same resolution as the image that the Gabor wavelet is applied on later")
  .add_parameter("frequency", "(float, float)", "The location :math:`\\vec k = (ky, kx)` of the Gabor wavelet in frequency domain; the values should be limited between :math:`-\\pi` and :math:`\\pi`")
  .add_parameter("sigma", "float", "[default: :math:`2\\pi`] The spatial resolution :math:`\\sigma` of the Gabor wavelet")
  .add_parameter("power_of_k", "float", "[default: 0] The :math:`\\lambda` factor to regularize the Gabor wavelet prefactor to generate comparable results for images, see Appendix C of [Guenther2011]_")
  .add_parameter("dc_free", "bool", "[default: True] Should the Gabor wavelet be without DC factor (i.e. should the integral under the wavelet in spatial domain vanish)?")
  .add_parameter("epsilon", "float", "[default: 1e-10] For speed reasons: all wavelet pixels in frequency domain with an absolute value below this should be considered as 0")
);


static int PyBobIpGaborWavelet_init(PyBobIpGaborWaveletObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = wavelet_doc.kwlist();

  blitz::TinyVector<int,2> r(-1,-1);
  blitz::TinyVector<double,2> k;
  double sigma = 2.*M_PI, pow_k = 0., eps=1e-10;
  PyObject* dc = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(ii)(dd)|ddO!d", kwlist, &r[0], &r[1], &k[0], &k[1], &sigma, &pow_k, &PyBool_Type, &dc, &eps)) return -1;
  self->cxx.reset(new bob::ip::gabor::Wavelet(r, k, sigma, pow_k, !dc || PyObject_IsTrue(dc), eps));
  return 0;
BOB_CATCH_MEMBER("Wavelet constructor", -1)
}

static void PyBobIpGaborWavelet_delete(PyBobIpGaborWaveletObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}

int PyBobIpGaborWavelet_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobIpGaborWavelet_Type));
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

static auto getWavelet_doc = bob::extension::VariableDoc(
  "wavelet",
  "2D-array complex",
  "The image representation of the Gabor wavelet in frequency domain",
  "The image representation is generated on the fly (since the original data format is different), the data format is float. "
  "To obtain the image representation in spatial domain, please perform a :py:func:`bob.sp.ifft()` on the returned image."
);

PyObject* PyBobIpGaborWavelet_getWavelet(PyBobIpGaborWaveletObject* self, void*){
BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->waveletImage());
BOB_CATCH_MEMBER("wavelet", 0)
}


static PyGetSetDef PyBobIpGaborWavelet_getseters[] = {
    {
      getWavelet_doc.name(),
      (getter)PyBobIpGaborWavelet_getWavelet,
      0,
      getWavelet_doc.doc(),
      0
    },
    {0}  /* Sentinel */
};



/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/

static auto wavelet_transform = bob::extension::FunctionDoc(
  "transform",
  "This function transforms the given input image to the output image",
  "Both images are considered to be in frequency domain and need to have the same resolution and to be of complex type. "
  "The resolution must be the same as the one defined in the constructor. "
  "The transform is defined as:\n\n"
  ".. math::\n\n"
  "   \\forall \\vec \\omega : \\mathcal T_{\\vec k}(\\vec \\omega) = \\mathcal I(\\vec \\omega) \\cdot \\psi_{\\vec k}(\\vec \\omega)\n\n"
  ".. note::\n\n  The function `__call__` is a synonym for this function.",
  true
)
.add_prototype("input, [output]", "output")
.add_parameter("input", "array_like (complex, 2D)", "The image in frequency domain that should be transformed")
.add_parameter("output", "array_like (complex, 2D)", "The image in frequency domain that should contain the transformed image; if given, must have the same size as ``input``")
.add_return("output", "array_like (complex, 2D)", "The image in frequency domain that will contain the transformed image; will have the same size as ``input``; is identical to the ``output`` parameter, if given")
;

static PyObject* PyBobIpGaborWavelet_transform(PyBobIpGaborWaveletObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = wavelet_transform.kwlist();

  PyBlitzArrayObject* input;
  PyBlitzArrayObject* output = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&|O&", kwlist, &PyBlitzArray_Converter, &input, &PyBlitzArray_OutputConverter, &output)) return 0;

  //protects acquired resources through this scope
  auto input_ = make_safe(input);
  auto output_ = make_xsafe(output);

  if (input->type_num != NPY_COMPLEX128) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 128-bit complex arrays for input array `input'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (output && output->type_num != NPY_COMPLEX128) {
    PyErr_Format(PyExc_TypeError, "`%s' only supports 128-bit complex arrays for output array `output'", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (input->ndim != 2) {
    PyErr_Format(PyExc_TypeError, "`%s' only accepts 2-dimensional arrays (not %" PY_FORMAT_SIZE_T "dD arrays)", Py_TYPE(self)->tp_name, input->ndim);
    return 0;
  }
  if (input->shape[0] != self->cxx->m_y_resolution || input->shape[1] != self->cxx->m_x_resolution){
    PyErr_Format(PyExc_RuntimeError, "The shape of the input image should be (%d,%d), but is (%" PY_FORMAT_SIZE_T "d,%" PY_FORMAT_SIZE_T "d)", self->cxx->m_y_resolution, self->cxx->m_x_resolution, input->shape[0], input->shape[1]);
    return 0;
  }

  if (output){
    if (output->ndim != 2) {
      PyErr_Format(PyExc_RuntimeError, "`%s' only accepts 2-dimensional arrays (not %" PY_FORMAT_SIZE_T "dD arrays)", Py_TYPE(self)->tp_name, output->ndim);
      return 0;
    }
    if (output->shape[0] != self->cxx->m_y_resolution || output->shape[1] != self->cxx->m_x_resolution){
      PyErr_Format(PyExc_RuntimeError, "The shape of the output image should be (%d,%d), but is (%" PY_FORMAT_SIZE_T "d,%" PY_FORMAT_SIZE_T "d)", self->cxx->m_y_resolution, self->cxx->m_x_resolution, output->shape[0], output->shape[1]);
      return 0;
    }
  }

  /** if ``output`` was not pre-allocated, do it now **/
  if (!output) {
    Py_ssize_t osize[2] = {self->cxx->m_y_resolution, self->cxx->m_x_resolution};
    output = (PyBlitzArrayObject*)PyBlitzArray_SimpleNew(NPY_COMPLEX128, input->ndim, osize);
    output_ = make_safe(output);
  }

  /** all basic checks are done, can call the machine now **/
  self->cxx->transform(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,2>(input), *PyBlitzArrayCxx_AsBlitz<std::complex<double>,2>(output));

  return PyBlitzArray_AsNumpyArray(output, 0);
BOB_CATCH_MEMBER("transform", 0)
}

static PyMethodDef PyBobIpGaborWavelet_methods[] = {
  {
    wavelet_transform.name(),
    (PyCFunction)PyBobIpGaborWavelet_transform,
    METH_VARARGS|METH_KEYWORDS,
    wavelet_transform.doc()
  },
  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the Gabor wavelet type struct; will be initialized later
PyTypeObject PyBobIpGaborWavelet_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobIpGaborWavelet(PyObject* module)
{
  // initialize the Gabor wavelet type struct
  PyBobIpGaborWavelet_Type.tp_name = wavelet_doc.name();
  PyBobIpGaborWavelet_Type.tp_basicsize = sizeof(PyBobIpGaborWaveletObject);
  PyBobIpGaborWavelet_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobIpGaborWavelet_Type.tp_doc = wavelet_doc.doc();

  // set the functions
  PyBobIpGaborWavelet_Type.tp_new = PyType_GenericNew;
  PyBobIpGaborWavelet_Type.tp_init = reinterpret_cast<initproc>(PyBobIpGaborWavelet_init);
  PyBobIpGaborWavelet_Type.tp_dealloc = reinterpret_cast<destructor>(PyBobIpGaborWavelet_delete);
  PyBobIpGaborWavelet_Type.tp_methods = PyBobIpGaborWavelet_methods;
  PyBobIpGaborWavelet_Type.tp_getset = PyBobIpGaborWavelet_getseters;
  PyBobIpGaborWavelet_Type.tp_call = reinterpret_cast<ternaryfunc>(PyBobIpGaborWavelet_transform);

  // check that everyting is fine
  if (PyType_Ready(&PyBobIpGaborWavelet_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobIpGaborWavelet_Type);
  return PyModule_AddObject(module, "Wavelet", (PyObject*)&PyBobIpGaborWavelet_Type) >= 0;
}
