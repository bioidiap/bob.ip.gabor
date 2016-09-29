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

static auto JetStatistics_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".JetStatistics",
  "Computes statistics of a list of Gabor jets",
  "``JetStatistics`` compute the mean and variances of absolute and phase values of the given set of Gabor :py:class:`Jet`\\s. "
  "Usually, they are used to capture the average structure of a given key point in several images. "
  "Since the jet statistics are computed using disparity correction of the training jets, they don't have to be 100% aligned to the keypoint location. "
  "Similarly to the Gabor jet :py:class:`Similarity`, it allows to compute disparities of a given Gabor jet to the average location that this ``JetStatistics`` incorporate.\n\n"
  "The statistics of Gabor jets is computed element-wise. "
  "The mean and variance of the absolute values is computed assuming univariate Gaussian distribution of :py:attr:`Jet.abs` from all given Gabor jets. "
  "The mean phase is computed differently, i.e., by computing the average jet (using the according :py:class:`Jet` constructor, while the phase variance is computed by computing the variances of :py:attr:`Jet.phase` to the mean phase. "
  "Finally, the Statistics of a given :py:class:`Jet` can be computed using the :py:meth:`log_likelihood` function. "
  "More details about the Gabor jet statistics can be found in section 3.3.3 of [Guenther2011]_."
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Creates a JetStatistics from a list of Gabor jets, or reads it from file",
    "When creating from a list of Gabor jets, all jets must have the same length and they should be extracted using the same :py:class:`Transform` class. "
    "To be able to compute the disparity, the ``gwt`` parameter has to be set to that :py:class:`Transform` class. "
    "This can be done here in the constructor, or by setting :py:attr:`gwt`.\n\n"
    "Particularly, when ``save_gwt = False`` was set in the :py:meth:`save` function and, hence, it is not read by the constructor taking the :py:class:`bob.io.base.HDF5File`, it will not be available (and :py:attr:`gwt` is ``None``).",
    true
  )
  .add_prototype("jets, [gwt]", "")
  .add_prototype("hdf5", "")
  .add_parameter("jets", "[:py:class:`bob.ip.gabor.Jet`]", "The list of Gabor jets to compute statistics from, must all be extracted using the same :py:class:`Transform` class")
  .add_parameter("gwt", ":py:class:`bob.ip.gabor.Transform` or ``None``", "[Default: ``None``] The Gabor wavelet family with which the Gabor jets were extracted")
  .add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for reading")
);

static int PyBobIpGaborJetStatistics_init(PyBobIpGaborJetStatisticsObject* self, PyObject* args, PyObject* kwargs) {

BOB_TRY
  char** kwlist1 = JetStatistics_doc.kwlist(0);
  char** kwlist2 = JetStatistics_doc.kwlist(1);

  // two ways to call
  PyObject* k = Py_BuildValue("s", kwlist2[0]);
  auto k_ = make_safe(k);
  if (
    (kwargs && PyDict_Contains(kwargs, k)) ||
    (args && PyTuple_Size(args) == 1 && PyBobIoHDF5File_Check(PyTuple_GetItem(args, 0)))
  ){
    PyBobIoHDF5FileObject* hdf5;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"O&", kwlist2, &PyBobIoHDF5File_Converter, &hdf5)) return -1;

    auto hdf5_ = make_safe(hdf5);

    self->cxx.reset(new bob::ip::gabor::JetStatistics(*hdf5->f));
  } else {
    PyObject* jets;
    PyObject* gwt=0;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", kwlist1, &jets, &gwt)) return -1;
    std::vector<boost::shared_ptr<bob::ip::gabor::Jet>> data;
    PyObject* iterator = PyObject_GetIter(jets);
    if (!iterator) {
      PyErr_Format(PyExc_TypeError, "The given list of Gabor jets is not iterable.");
      return -1;
    }
    auto iterator_ = make_safe(iterator);
    int i = 0;
    while (PyObject* it = PyIter_Next(iterator)) {
      auto it_ = make_safe(it);
      if (!PyBobIpGaborJet_Check(it)){
        PyErr_Format(PyExc_TypeError, "`%s' requires all elements of the `to_average` parameter to be of type bob.ip.gabor.Jet, but element %d isn't", Py_TYPE(self)->tp_name, i);
        return -1;
      }
      data.push_back(reinterpret_cast<PyBobIpGaborJetObject*>(it)->cxx);
      ++i;
    }

    if (gwt && gwt != Py_None){
      // check for transform type
      if (!PyBobIpGaborTransform_Check(gwt)){
        PyErr_Format(PyExc_TypeError, "The given 'gwt' object is not of type bob.ip.gabor.Transform");
        return -1;
      }
      self->cxx.reset(new bob::ip::gabor::JetStatistics(data, reinterpret_cast<PyBobIpGaborTransformObject*>(gwt)->cxx));
    } else {
      self->cxx.reset(new bob::ip::gabor::JetStatistics(data));
    }
  }
  return 0;
BOB_CATCH_MEMBER("cannot initialize", -1)
}

static void PyBobIpGaborJetStatistics_delete(PyBobIpGaborJetStatisticsObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}

int PyBobIpGaborJetStatistics_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobIpGaborJetStatistics_Type));
}

static PyObject* PyBobIpGaborJetStatistics_RichCompare(PyBobIpGaborJetStatisticsObject* self, PyObject* other, int op) {
BOB_TRY
  if (!PyBobIpGaborJetStatistics_Check(other)) {
    PyErr_Format(PyExc_TypeError, "cannot compare `%s' with `%s'", Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
    return 0;
  }
  auto other_ = reinterpret_cast<PyBobIpGaborJetStatisticsObject*>(other);
  switch (op) {
    case Py_EQ:
      if (*self->cxx==*other_->cxx) Py_RETURN_TRUE; else Py_RETURN_FALSE;
    case Py_NE:
      if (*self->cxx==*other_->cxx) Py_RETURN_FALSE; else Py_RETURN_TRUE;
    default:
      Py_INCREF(Py_NotImplemented);
      return Py_NotImplemented;
  }
BOB_CATCH_MEMBER("RichCompare", 0)
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/
static auto gwt_doc = bob::extension::VariableDoc(
  "gwt",
  ":py:class:`bob.ip.gabor.Transform` or ``None``",
  "The Gabor transform class with which the Gabor jets were extracted; can be ``None`` if the transform class is not stored in this object."
);
PyObject* PyBobIpGaborJetStatistics_getGwt(PyBobIpGaborJetStatisticsObject* self, void*){
BOB_TRY
  auto gwt = self->cxx->gwt();
  if (!gwt)
    Py_RETURN_NONE;
  PyBobIpGaborTransformObject* transform = (PyBobIpGaborTransformObject*)PyBobIpGaborTransform_Type.tp_alloc(&PyBobIpGaborTransform_Type, 0);
  transform->cxx = gwt;
  return Py_BuildValue("N", transform);
BOB_CATCH_MEMBER("gwt", 0)
}

int PyBobIpGaborJetStatistics_setGwt(PyBobIpGaborJetStatisticsObject* self, PyObject* value, void*){
BOB_TRY
  // check fore None
  if (value == Py_None){
    // reset gwt
    self->cxx->gwt(boost::shared_ptr<bob::ip::gabor::Transform>());
    return 0;
  }
  // check for transform type
  if (!PyBobIpGaborTransform_Check(value)){
    PyErr_Format(PyExc_TypeError, "The given object is not of type bob.ip.gabor.Transform");
    return -1;
  }
  // set transform
  PyBobIpGaborTransformObject* transform = (PyBobIpGaborTransformObject*)value;
  self->cxx->gwt(transform->cxx);
  return 0;
BOB_CATCH_MEMBER("gwt", -1)
}


static auto meanAbs_doc = bob::extension::VariableDoc(
  "mean_abs",
  "array(float,1D)",
  "The mean of absolute values of the Gabor Jets, read only"
);
PyObject* PyBobIpGaborJetStatistics_meanAbs(PyBobIpGaborJetStatisticsObject* self, void*){
BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->meanAbs());
BOB_CATCH_MEMBER("mean_abs", 0)
}

static auto varAbs_doc = bob::extension::VariableDoc(
  "var_abs",
  "array(float,1D)",
  "The variances of absolute values of the Gabor jets, read only"
);
PyObject* PyBobIpGaborJetStatistics_varAbs(PyBobIpGaborJetStatisticsObject* self, void*){
BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->varAbs());
BOB_CATCH_MEMBER("var_abs", 0)
}

static auto meanPhase_doc = bob::extension::VariableDoc(
  "mean_phase",
  "array(float,1D)",
  "The mean of phase values of the Gabor jets, read only"
);
PyObject* PyBobIpGaborJetStatistics_meanPhase(PyBobIpGaborJetStatisticsObject* self, void*){
BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->meanPhase());
BOB_CATCH_MEMBER("mean_phase", 0)
}

static auto varPhase_doc = bob::extension::VariableDoc(
  "var_phase",
  "array(float,1D)",
  "The variances of phase values of the Gabor jets, read only"
);
PyObject* PyBobIpGaborJetStatistics_varPhase(PyBobIpGaborJetStatisticsObject* self, void*){
BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->varPhase());
BOB_CATCH_MEMBER("var_phase", 0)
}

static PyGetSetDef PyBobIpGaborJetStatistics_getseters[] = {
  {
    gwt_doc.name(),
    (getter)PyBobIpGaborJetStatistics_getGwt,
    (setter)PyBobIpGaborJetStatistics_setGwt,
    gwt_doc.doc(),
    0
  },
  {
    meanAbs_doc.name(),
    (getter)PyBobIpGaborJetStatistics_meanAbs,
    0,
    meanAbs_doc.doc(),
    0
  },
  {
    varAbs_doc.name(),
    (getter)PyBobIpGaborJetStatistics_varAbs,
    0,
    varAbs_doc.doc(),
    0
  },
  {
    meanPhase_doc.name(),
    (getter)PyBobIpGaborJetStatistics_meanPhase,
    0,
    meanPhase_doc.doc(),
    0
  },
  {
    varPhase_doc.name(),
    (getter)PyBobIpGaborJetStatistics_varPhase,
    0,
    varPhase_doc.doc(),
    0
  },
  {0}  /* Sentinel */
};


/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/

static auto disparity_doc = bob::extension::FunctionDoc(
  "disparity",
  "Computes the disparity for the given Gabor jet",
  "The disparity calculation works only, when these statistics were calculated from Gabor jets of the same object at the same landmark. "
  "Then, the disparity estimates a distance vector from the given ``jet`` towards this location. "
  "The disparity estimation works well only in a limited area around the correct offset.\n\n"
  "For more information on how the disparity is computed, please read Appendix B.2 of [Guenther2011]_.",
  true
)
.add_prototype("jet", "disp")
.add_parameter("jet", ":py:class:`bob.ip.gabor.Jet`", "The Gabor jet to compute the disparity for")
.add_return("disp", "(float,float)", "The disparity for the given jet")
;

static PyObject* PyBobIpGaborJetStatistics_disparity(PyBobIpGaborJetStatisticsObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = disparity_doc.kwlist();

  PyBobIpGaborJetObject* jet;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBobIpGaborJet_Type, &jet)) return 0;

  auto disp = self->cxx->disparity(jet->cxx);
  return Py_BuildValue("dd", disp[0], disp[1]);
BOB_CATCH_MEMBER("disparity", 0)
}

static auto logLikelihood_doc = bob::extension::FunctionDoc(
  "log_likelihood",
  "Computes the log-likelihood for the given Gabor jet",
  "This function estimates the log-likelihood of the given Gabor jet to fit to these statistics. "
  "It computes the disparity of the given ``jet``, and estimates the probability of the absolute values and the disparity corrected phase values.\n\n"
  "When the Gabor jet should have been extracted with sub-pixel accuracy (which obviously isn't possible), you can set this offset here, so that it is removed from the disparity estimate. "
  "In fact, you can directly pass the sub-pixel location of the Gabor jet, and the offset will be estimated automatically. "
  "This value can be left out, when not required.\n\n"
  ".. note::\n\n  The function `__call__` is a synonym for this function."
  ,
  true
)
.add_prototype("jet, [estimate_phase], [offset]", "score")
.add_parameter("jet", ":py:class:`bob.ip.gabor.Jet`", "The Gabor jet to compute the likelihood for")
.add_parameter("estimate_phase", "bool", "[Default: ``True``] Should the phase be included into the estimation?")
.add_parameter("offset", "(float, float)", "[Default: ``(0,0)``] Sub-pixel location offset, where the Gabor jet should have been extracted")
.add_return("score", "float", "The likelihood score for the given jet")
;

static PyObject* PyBobIpGaborJetStatistics_logLikelihood(PyBobIpGaborJetStatisticsObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = logLikelihood_doc.kwlist();

  PyBobIpGaborJetObject* jet;
  PyObject* phase = 0;
  blitz::TinyVector<double,2> offset(0.,0.);

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|O(dd)", kwlist, &PyBobIpGaborJet_Type, &jet, &phase, &offset[0], &offset[1])) return 0;

  double score = self->cxx->logLikelihood(jet->cxx, !phase || PyObject_IsTrue(phase), offset);
  return Py_BuildValue("d", score);
BOB_CATCH_MEMBER("log_likelihood", 0)
}


static auto save_doc = bob::extension::FunctionDoc(
  "save",
  "Saves the JetStatistics to the given HDF5 file",
  "If several ``JetStatistics`` with the same :py:class:`Transform` are written to the same file, it might be useful to write the transform only once. "
  "In this case, you can set the ``save_gwt`` parameter to ``False``.",
  true
)
.add_prototype("hdf5, [save_gwt]")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for writing")
.add_parameter("save_gwt", "bool", "[Default: ``True``] Should the Gabor wavelet transform class be written to the file as well?")
;

static PyObject* PyBobIpGaborJetStatistics_save(PyBobIpGaborJetStatisticsObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  // get list of arguments
  char** kwlist = save_doc.kwlist();
  PyBobIoHDF5FileObject* file;
  PyObject* gwt = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&|O", kwlist, PyBobIoHDF5File_Converter, &file, &gwt)) return 0;

  auto file_ = make_safe(file);
  self->cxx->save(*file->f, !gwt or PyObject_IsTrue(gwt));
  Py_RETURN_NONE;
BOB_CATCH_MEMBER("save", 0)
}


static PyMethodDef PyBobIpGaborJetStatistics_methods[] = {
  {
    disparity_doc.name(),
    (PyCFunction)PyBobIpGaborJetStatistics_disparity,
    METH_VARARGS|METH_KEYWORDS,
    disparity_doc.doc()
  },
  {
    logLikelihood_doc.name(),
    (PyCFunction)PyBobIpGaborJetStatistics_logLikelihood,
    METH_VARARGS|METH_KEYWORDS,
    logLikelihood_doc.doc()
  },
  {
    save_doc.name(),
    (PyCFunction)PyBobIpGaborJetStatistics_save,
    METH_VARARGS|METH_KEYWORDS,
    save_doc.doc()
  },
  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the Gabor wavelet type struct; will be initialized later
PyTypeObject PyBobIpGaborJetStatistics_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobIpGaborJetStatistics(PyObject* module)
{

  // initialize the JetStatistics type struct
  PyBobIpGaborJetStatistics_Type.tp_name = JetStatistics_doc.name();
  PyBobIpGaborJetStatistics_Type.tp_basicsize = sizeof(PyBobIpGaborJetStatisticsObject);
  PyBobIpGaborJetStatistics_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobIpGaborJetStatistics_Type.tp_doc = JetStatistics_doc.doc();

  // set the functions
  PyBobIpGaborJetStatistics_Type.tp_new = PyType_GenericNew;
  PyBobIpGaborJetStatistics_Type.tp_init = reinterpret_cast<initproc>(PyBobIpGaborJetStatistics_init);
  PyBobIpGaborJetStatistics_Type.tp_dealloc = reinterpret_cast<destructor>(PyBobIpGaborJetStatistics_delete);
  PyBobIpGaborJetStatistics_Type.tp_richcompare = reinterpret_cast<richcmpfunc>(PyBobIpGaborJetStatistics_RichCompare);
  PyBobIpGaborJetStatistics_Type.tp_methods = PyBobIpGaborJetStatistics_methods;
  PyBobIpGaborJetStatistics_Type.tp_getset = PyBobIpGaborJetStatistics_getseters;
  PyBobIpGaborJetStatistics_Type.tp_call = reinterpret_cast<ternaryfunc>(PyBobIpGaborJetStatistics_logLikelihood);

  // check that everyting is fine
  if (PyType_Ready(&PyBobIpGaborJetStatistics_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobIpGaborJetStatistics_Type);
  return PyModule_AddObject(module, "JetStatistics", (PyObject*)&PyBobIpGaborJetStatistics_Type) >= 0;
}
