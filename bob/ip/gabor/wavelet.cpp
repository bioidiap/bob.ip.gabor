/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Tue 14 Jan 2014 14:26:09 CET
 *
 * @brief Bindings for a GaborWaveletTransform
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

/**********************************************
 * Implementation of Gabor wavelet class *
 **********************************************/

static auto wavelet_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".Wavelet",
  "A class that represents a Gabor wavelet in frequency domain",
  "The implementation is according to [Guenther2011]_, where all details "
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Creates a Gabor wavelet in frequency domain for the given parametrization",
    "The Gabor wavelet :math:`\\check\\psi_{\\vec k}` is created in frequency domain as a Gaussian that is shifted from the origin of the frequency domain:\n\n"
    ".. math::\n"
    "   \\check\\psi_{\\vec k}(\\vec \\omega) = k^{\\lambda} \\left\\{ e^{-\\frac{\\sigma^2(\\vec \\omega - \\vec k)^2}{2\\vec k^2}} - {e^{-\\frac{\\sigma^2(\\vec \\omega^2 + \\vec k^2)}{2\\vec k^2}}}\\right\\}\n\n"
    ".. [Guenther2011] *Manuel Guenther*. **Statistical Gabor graph based techniques for the detection, recognition, classification and visualization of human faces**, PhD thesis, Technical University of Ilmenau, 2011",
    true
  )
  .add_prototype("resolution, frequency, [sigma], [power_of_k], [dc_free], [epsilon]", "")
  .add_parameter("resolution", "(int,int)", "The resolution (height, width) of the Gabor wavelet; this must be the same resolution as the image that the Gabor wavelet is applied on later")
  .add_parameter("frequency", "(float,float)", "The location :math:`\\vec k = (ky, kx)` of the Gabor wavelet in frequency domain; the values should be limited between :math:`-\\pi` and :math:`\\pi`")
  .add_parameter("sigma", "float", "[default: :math:`2\\dot\\pi`] The spatial resolution :math:`\\sigma` of the Gabor wavelet")
  .add_parameter("power_of_k", "float", "[default: 0] The :math:`\\lambda` factor to regularize the Gabor wavelet prefactor to generate comparable results for images, see Appendix C of [Guenther2011]_")
  .add_parameter("dc_free", "bool", "[default: True] Should the Gabor wavelet be without DC factor (i.e. should the integral under the wavelet in spatial domain vanish)?")
  .add_parameter("epsilon", "float", "[default: 1e-10] For speed reasons: all wavelet pixels in frequency domain with an absolute value below this should be considered as 0")
);



static int PyBobIpGaborWavelet_init(PyBobIpGaborWaveletObject* self, PyObject* args, PyObject* kwargs) {

  char*  kwlist[] = {c("resolution"), c("frequency"), c("sigma"), c("power_of_k"), c("dc_free"), c("epsilon"), NULL};

  blitz::TinyVector<int,2> r;
  blitz::TinyVector<double,2> k;
  double sigma = 2.*M_PI, pow_k = 0., eps=1e-10;
  bool dc = true;

  if (
    !PyArg_ParseTupleAndKeywords(
      args, kwargs,
      "(ii)(dd)|ddO!d", kwlist,
      &r[0], &r[1], &k[0], &k[1], &sigma, pow_k,
      &PyBool_Type, &dc,
      &eps
    )
  ){
    wavelet_doc.print_usage();
    return -1;
  }

  self->cxx.reset(new bob::ip::gabor::Wavelet(r, k, sigma, pow_k, dc, eps));

  return 0;

}

static void PyBobIpGaborWavelet_delete(PyBobIpGaborWaveletObject* self) {
  Py_TYPE(self)->tp_free((PyObject*)self);
}

int PyBobIpGaborGaborWavelet_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobIpGaborWaveletType));
}


static auto getWavelet_doc = bob::extension::VariableDoc(
  "wavelet",
  "2D-array complex",
  "The image representation of the Gabor wavelet in frequency domain",
  "The image representation is generated on the fly (since the original data format is different), the data format is float. "
  "To obtain the image representation in spatial domain, please perform a :py:func:`bob.ip.ifft()` on the returned image."
);

PyObject* PyBobIpGaborWavelet_getWavelet(PyBobIpGaborWaveletObject* self, void*){
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->waveletImage());
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


#if 0
#if PY_VERSION_HEX >= 0x03000000
#  define PYOBJECT_STR PyObject_Str
#else
#  define PYOBJECT_STR PyObject_Unicode
#endif

PyObject* PyBobIpGaborGaborWavelet_Repr(PyBobIpGaborWaveletObject* self) {

  /**
   * Expected output:
   *
   * <bob.learn.linear.Machine float64@(3, 2) [act: f(z) = tanh(z)]>
   */

  using bob::machine::IdentityActivation;

  static const std::string identity_str = IdentityActivation().str();

  auto weights = make_safe(PyBobLearnLinearMachine_getWeights(self, 0));
  if (!weights) return 0;
  auto dtype = make_safe(PyObject_GetAttrString(weights.get(), "dtype"));
  auto dtype_str = make_safe(PYOBJECT_STR(dtype.get()));
  auto shape = make_safe(PyObject_GetAttrString(weights.get(), "shape"));
  auto shape_str = make_safe(PYOBJECT_STR(shape.get()));

  PyObject* retval = 0;

  if (self->cxx->getActivation()->str() == identity_str) {
    retval = PyUnicode_FromFormat("<%s %U@%U>",
        Py_TYPE(self)->tp_name, dtype_str.get(), shape_str.get());
  }

  else {
    retval = PyUnicode_FromFormat("<%s %s@%s [act: %s]>",
        Py_TYPE(self)->tp_name, dtype_str.get(), shape_str.get(),
        self->cxx->getActivation()->str().c_str());
  }

#if PYTHON_VERSION_HEX < 0x03000000
  if (!retval) return 0;
  PyObject* tmp = PyObject_Str(retval);
  Py_DECREF(retval);
  retval = tmp;
#endif

  return retval;

}

PyObject* PyBobIpGaborGaborWavelet_Str(PyBobIpGaborWaveletObject* self) {

  /**
   * Expected output:
   *
   * bob.learn.linear.Machine (float64) 3 inputs, 2 outputs [act: f(z) = C*z]
   *  subtract: [ 0.   0.5  0.5]
   *  divide: [ 0.5  1.   1. ]
   *  bias: [ 0.3 -3. ]
   *  [[ 0.4  0.1]
   *  [ 0.4  0.2]
   *  [ 0.2  0.7]]
   */

  using bob::machine::IdentityActivation;

  static const std::string identity_str = IdentityActivation().str();

  boost::shared_ptr<PyObject> act;
  if (self->cxx->getActivation()->str() != identity_str) {
    act = make_safe(PyUnicode_FromFormat(" [act: %s]",
          self->cxx->getActivation()->str().c_str()));
  }
  else act = make_safe(PyUnicode_FromString(""));

  boost::shared_ptr<PyObject> sub;
  if (blitz::any(self->cxx->getInputSubtraction())) {
    auto t = make_safe(PyBobLearnLinearMachine_getInputSubtraction(self, 0));
    auto t_str = make_safe(PYOBJECT_STR(t.get()));
    sub = make_safe(PyUnicode_FromFormat("\n subtract: %U", t_str.get()));
  }
  else sub = make_safe(PyUnicode_FromString(""));

  boost::shared_ptr<PyObject> div;
  if (blitz::any(self->cxx->getInputDivision())) {
    auto t = make_safe(PyBobLearnLinearMachine_getInputDivision(self, 0));
    auto t_str = make_safe(PYOBJECT_STR(t.get()));
    div = make_safe(PyUnicode_FromFormat("\n divide: %U", t_str.get()));
  }
  else div = make_safe(PyUnicode_FromString(""));

  boost::shared_ptr<PyObject> bias;
  if (blitz::any(self->cxx->getBiases())) {
    auto t = make_safe(PyBobLearnLinearMachine_getBiases(self, 0));
    auto t_str = make_safe(PYOBJECT_STR(t.get()));
    bias = make_safe(PyUnicode_FromFormat("\n bias: %U", t_str.get()));
  }
  else bias = make_safe(PyUnicode_FromString(""));

  auto weights = make_safe(PyBobLearnLinearMachine_getWeights(self, 0));
  if (!weights) return 0;
  auto weights_str = make_safe(PYOBJECT_STR(weights.get()));
  auto dtype = make_safe(PyObject_GetAttrString(weights.get(), "dtype"));
  auto dtype_str = make_safe(PYOBJECT_STR(dtype.get()));
  auto shape = make_safe(PyObject_GetAttrString(weights.get(), "shape"));

  PyObject* retval = PyUnicode_FromFormat("%s (%U) %" PY_FORMAT_SIZE_T "d inputs, %" PY_FORMAT_SIZE_T "d outputs%U%U%U%U\n %U",
    Py_TYPE(self)->tp_name, dtype_str.get(),
    PyNumber_AsSsize_t(PyTuple_GET_ITEM(shape.get(), 0), PyExc_OverflowError),
    PyNumber_AsSsize_t(PyTuple_GET_ITEM(shape.get(), 1), PyExc_OverflowError),
    act.get(), sub.get(), div.get(), bias.get(), weights_str.get());

#if PYTHON_VERSION_HEX < 0x03000000
  if (!retval) return 0;
  PyObject* tmp = PyObject_Str(retval);
  Py_DECREF(retval);
  retval = tmp;
#endif

  return retval;

}
#endif

static auto wavelet_transform = bob::extension::FunctionDoc(
  "transform",
  "This function transforms the given input image to the output image",
  "Both images need to have the same resolution and need to be of complex type. "
  "The resolution must be the same as it was defined in the constructor. "
  "The transform is defined as:\n"
  ".. math::\n\n"
  "   \\forall \\omega : \\mathcal T_{\\vec k}(\\vec \\omega) = \\mathcal I(\\vec \\omega) \\dot \\psi_{\\vec k}(\\vec \\omega)",
  true
)
.add_prototype("input,output")
.add_prototype("input", "output")
.add_parameter("input", "array_like (complex, 2D)", "The image in frequency domain that should be transformed")
.add_parameter("output", "array_like (complex, 2D)", "The image in frequency domain that should contain the transformed image; must have the same size as ``input``")
.add_return("output", "array_like (complex, 2D)", "The image in frequency domain that will contain the transformed image; will have the same size as ``input``")
;

static PyObject* PyBobIpGaborGaborWavelet_transform(PyBobIpGaborWaveletObject* self, PyObject* args, PyObject* kwds) {

  static char* kwlist[] = {c("input"), c("output"), 0};

  PyBlitzArrayObject* input = 0;
  PyBlitzArrayObject* output = 0;

  if (
    !PyArg_ParseTupleAndKeywords(args, kwds, "O&|O&", kwlist,
      &PyBlitzArray_Converter, &input,
      &PyBlitzArray_OutputConverter, &output
    )
  ){
    wavelet_transform.print_usage();
    return 0;
  }

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
  try {
    self->cxx->transform(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,2>(input),
        *PyBlitzArrayCxx_AsBlitz<std::complex<double>,2>(output));
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "%s cannot transform input: unknown exception caught", Py_TYPE(self)->tp_name);
    return 0;
  }

  Py_INCREF(output);
  return PyBlitzArray_NUMPY_WRAP(reinterpret_cast<PyObject*>(output));

}

static PyMethodDef PyBobIpGaborWavelet_methods[] = {
  {
    wavelet_transform.name(),
    (PyCFunction)PyBobIpGaborGaborWavelet_transform,
    METH_VARARGS|METH_KEYWORDS,
    wavelet_transform.doc()
  },
  {0} /* Sentinel */
};


// Define the Gabor wavelet type struct; will be initialized later
PyTypeObject PyBobIpGaborWaveletType = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobIpGaborWavelet(PyObject* module)
{

  // initialize the Gabor wavelet type struct
  PyBobIpGaborWaveletType.tp_name = wavelet_doc.name();
  PyBobIpGaborWaveletType.tp_basicsize = sizeof(PyBobIpGaborWaveletObject);
  PyBobIpGaborWaveletType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobIpGaborWaveletType.tp_doc = wavelet_doc.doc();

  // set the functions
  PyBobIpGaborWaveletType.tp_new = PyType_GenericNew;
  PyBobIpGaborWaveletType.tp_init = reinterpret_cast<initproc>(PyBobIpGaborWavelet_init);
  PyBobIpGaborWaveletType.tp_dealloc = reinterpret_cast<destructor>(PyBobIpGaborWavelet_delete);
  PyBobIpGaborWaveletType.tp_methods = PyBobIpGaborWavelet_methods;
  PyBobIpGaborWaveletType.tp_getset = PyBobIpGaborWavelet_getseters;

  // check that everyting is fine
  if (PyType_Ready(&PyBobIpGaborWaveletType) < 0)
    return false;

  // add the type to the module
  Py_INCREF(&PyBobIpGaborWaveletType);
  return PyModule_AddObject(module, "Wavelet", (PyObject*)&PyBobIpGaborWaveletType) >= 0;
}

