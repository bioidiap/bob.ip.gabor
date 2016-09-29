/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief Bindings for a Gabor jet similarity
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

static auto Similarity_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".Similarity",
  "A class that computes different kind of similarity function, i.a., disparity corrected Gabor phase differences.",
  "The implementation is according to [Guenther2011]_ and [Guenther2012]_, where all similarity functions are explained in more details. "
  "There are several types of Gabor jet similarity functions implemented. "
  "Some of them make use of the absolute values :py:attr:`Jet.abs` of the Gabor jets values, which are coded as :math:`a_j` with :math:`j=1\\dots J` where :math:`J =` :py:attr:`Jet.length` :\n\n"
  "* ``'ScalarProduct'``\n\n"
  "  .. math:: S_a(\\mathcal J, \\mathcal J') = \\sum\\limits_j a_j \\cdot a_j'\n"
  "* ``'Canberra'``\n\n"
  "  .. math:: S_C(\\mathcal J, \\mathcal J') = \\sum\\limits_j \\frac{a_j - a_j'}{a_j + a_j'}\n\n"
  "Other similarity funcitons make use of the absolute values :math:`a_j` (:py:attr:`Jet.abs`) and the phase values :math:`\\phi_j` (:py:attr:`Jet.phase`):\n\n"
  "* ``'AbsPhase'``\n\n"
  "  .. math:: S_{\\phi}(\\mathcal J, \\mathcal J') = \\sum\\limits_j a_j \\cdot a_j' \\cdot \\cos(\\phi_j - \\phi_j')\n"
  "* ``'Disparity'``\n\n"
  "  .. math:: S_D(\\mathcal J, \\mathcal J') = \\sum\\limits_j a_j \\cdot a_j' \\cdot \\cos(\\phi_j - \\phi_j' - \\vec k_j^T\\vec d)\n"
  "* ``'PhaseDiff'``\n\n"
  "  .. math:: S_P(\\mathcal J, \\mathcal J') = \\sum\\limits_j \\cos(\\phi_j - \\phi_j' - \\vec k_j^T\\vec d)\n"
  "* ``'PhaseDiffPlusCanberra'``\n\n"
  "  .. math:: S_{P+C}(\\mathcal J, \\mathcal J') = \\sum\\limits_j \\left[ \\frac{a_j - a_j'}{a_j + a_j'} + \\cos(\\phi_j - \\phi_j' - \\vec k_j^T\\vec d) \\right]\n\n"
  "Some of these functions make use of the kernel vector :math:`\\vec k_j` (:py:attr:`Transform.wavelet_frequencies`) and the disparity vector :math:`\\vec d`, which is estimated based on the given two Gabor jets. "
  "This procedure is described in more detail in Appendix B of [Guenther2011]_."
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Creates a Gabor wavelet similarity function of the given type",
    "Currently, several types of Gabor jet similarity functions are implemented. "
    "Please refer to [Guenther2012]_ for details.",
    true
  )
  .add_prototype("type, [transform]", "")
  .add_prototype("hdf5", "")
  .add_parameter("type", "str", "The type of the Gabor jet similarity function; might be one of (``'ScalarProduct'``, ``'Canberra'``, ``'AbsPhase'``, ``'Disparity'``, ``'PhaseDiff'``, ``'PhaseDiffPlusCanberra'``)")
  .add_parameter("transform", ":py:class:`bob.ip.gabor.Transform`", "The Gabor wavelet transform class that was used to generate the Gabor jets; only required for disparity-based similarity functions ('Disparity', 'PhaseDiff', 'PhaseDiffPlusCanberra')")
  .add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for reading to load the parametrization of the Gabor wavelet similarity from")
);

static int PyBobIpGaborSimilarity_init(PyBobIpGaborSimilarityObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist1 = Similarity_doc.kwlist(1);
  char** kwlist2 = Similarity_doc.kwlist(0);

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
    self->cxx.reset(new bob::ip::gabor::Similarity(*hdf5->f));
  } else {
    const char* name = 0;
    PyBobIpGaborTransformObject* gwt = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|O!", kwlist2, &name, &PyBobIpGaborTransform_Type, &gwt)) return -1;
    if (gwt)
      self->cxx.reset(new bob::ip::gabor::Similarity(bob::ip::gabor::Similarity::name_to_type(name), gwt->cxx));
    else
      self->cxx.reset(new bob::ip::gabor::Similarity(bob::ip::gabor::Similarity::name_to_type(name)));
  }
  return 0;
BOB_CATCH_MEMBER("Similarity constructor", -1)
}

static void PyBobIpGaborSimilarity_delete(PyBobIpGaborSimilarityObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}

int PyBobIpGaborSimilarity_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobIpGaborSimilarity_Type));
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

static auto type_doc = bob::extension::VariableDoc(
  "type",
  "str",
  "The type of the Gabor jet similarity function"
);
PyObject* PyBobIpGaborSimilarity_type(PyBobIpGaborSimilarityObject* self, void*){
BOB_TRY
  return Py_BuildValue("s", self->cxx->type().c_str());
BOB_CATCH_MEMBER("type", 0)
}

static auto transform_doc = bob::extension::VariableDoc(
  "transform",
  ":py:class:`bob.ip.gabor.Transform` or ``None``",
  "The Gabor wavelet transform used in the similarity class; can be ``None`` for similarity functions that do not compute disparities"
);
PyObject* PyBobIpGaborSimilarity_transform(PyBobIpGaborSimilarityObject* self, void*){
BOB_TRY
  PyBobIpGaborTransformObject* transform = (PyBobIpGaborTransformObject*)PyBobIpGaborTransform_Type.tp_alloc(&PyBobIpGaborTransform_Type, 0);
  transform->cxx = self->cxx->transform();
  return Py_BuildValue("N", transform);
BOB_CATCH_MEMBER("transform", 0)
}

static auto lastDisparity_doc = bob::extension::VariableDoc(
  "last_disparity",
  "(float, float)",
  "The disparity that was computed during the last call to :py:func:`similarity` or :py:func:`disparity`."
);
PyObject* PyBobIpGaborSimilarity_lastDisparity(PyBobIpGaborSimilarityObject* self, void*){
BOB_TRY
  const auto& disp = self->cxx->disparity();
  return Py_BuildValue("(dd)", disp[0], disp[1]);
BOB_CATCH_MEMBER("last_disparity", 0)
}


static PyGetSetDef PyBobIpGaborSimilarity_getseters[] = {
  {
    type_doc.name(),
    (getter)PyBobIpGaborSimilarity_type,
    0,
    type_doc.doc(),
    0
  },
  {
    transform_doc.name(),
    (getter)PyBobIpGaborSimilarity_transform,
    0,
    transform_doc.doc(),
    0
  },
  {
    lastDisparity_doc.name(),
    (getter)PyBobIpGaborSimilarity_lastDisparity,
    0,
    lastDisparity_doc.doc(),
    0
  },
  {0}  /* Sentinel */
};


/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/

static auto similarity_doc = bob::extension::FunctionDoc(
  "similarity",
  "This function computes the similarity between the two given Gabor jets",
  "Depending on the :py:attr:`type`, different kinds of similarities are computed (see [Guenther2011]_ for details). "
  "Some of them will also compute the disparity from the first to the second Gabor jet, which can be retrieved by :py:attr:`last_disparity`.\n\n"
  ".. note::\n\n  The function `__call__` is a synonym for this function.",
  true
)
.add_prototype("jet1, jet2", "sim")
.add_parameter("jet1, jet2", ":py:class:`bob.ip.gabor.Jet`", "The two Gabor jets that should be compared")
.add_return("sim", "float", "The similarity between the two Gabor jets; more similar Gabor jets will get higher similarity values")
;

static PyObject* PyBobIpGaborSimilarity_similarity(PyBobIpGaborSimilarityObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = similarity_doc.kwlist();

  PyBobIpGaborJetObject* jet1,* jet2;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobIpGaborJet_Type, &jet1, &PyBobIpGaborJet_Type, &jet2)) return 0;

  double sim = self->cxx->similarity(*jet1->cxx, *jet2->cxx);
  return Py_BuildValue("d", sim);
BOB_CATCH_MEMBER("similarity", 0)
}


static auto disparity_doc = bob::extension::FunctionDoc(
  "disparity",
  "This function computes the disparity vector for the given Gabor jets",
  "This function is only available for convenience, it **does not** need to be called before :py:func:`similarity` is called.",
  true
)
.add_prototype("jet1, jet2", "disparity")
.add_parameter("jet1, jet2", ":py:class:`bob.ip.gabor.Jet`", "The two Gabor jets to compute the disparity between")
.add_return("disparity", "(float, float)", "The disparity vector estimated from the given Gabor jets")
;

static PyObject* PyBobIpGaborSimilarity_disparity(PyBobIpGaborSimilarityObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = disparity_doc.kwlist();

  PyBobIpGaborJetObject* jet1,* jet2;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobIpGaborJet_Type, &jet1, &PyBobIpGaborJet_Type, &jet2)) return 0;

  const auto& disp = self->cxx->disparity(*jet1->cxx, *jet2->cxx);
  return Py_BuildValue("(dd)", disp[0], disp[1]);
BOB_CATCH_MEMBER("disparity", 0)
}


static auto shift_phase_doc = bob::extension::FunctionDoc(
  "shift_phase",
  "This function returns a copy of the Gabor jet, for which the Gabor phases are shifted towards the reference Gabor jet",
  "It uses the disparity estimation to compute the avarage disparity between the two Gabor jets. "
  "Afterwards, the phases of the given ``jet`` are adapted such that the disparity to the ``reference`` jet is equaled out.",
  true
)
.add_prototype("jet, reference", "shifted")
.add_parameter("jet", ":py:class:`bob.ip.gabor.Jet`", "The Gabor jets, whose phases should be shifted")
.add_parameter("reference", ":py:class:`bob.ip.gabor.Jet`", "The Gabor jets, towards which the phases should be shifted")
.add_return("shifted", ":py:class:`bob.ip.gabor.Jet`", "A copy of ``jet``, where the phases are shifted towards ``reference``")
;

static PyObject* PyBobIpGaborSimilarity_shift_phase(PyBobIpGaborSimilarityObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = shift_phase_doc.kwlist();

  PyBobIpGaborJetObject* jet,* reference;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobIpGaborJet_Type, &jet, &PyBobIpGaborJet_Type, &reference)) return 0;

  // create new jet
  PyBobIpGaborJetObject* shifted = reinterpret_cast<PyBobIpGaborJetObject*>(PyBobIpGaborJet_Type.tp_alloc(&PyBobIpGaborJet_Type, 0));
  shifted->cxx.reset(new bob::ip::gabor::Jet(jet->cxx->length()));

  // shift it
  self->cxx->shift_phase(*jet->cxx, *reference->cxx, *shifted->cxx);

  // return it
  return Py_BuildValue("N", shifted);
BOB_CATCH_MEMBER("shift_phase", 0)
}


static auto load_doc = bob::extension::FunctionDoc(
  "load",
  "Loads the parametrization of the Gabor jet similarity from the given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file opened for reading")
;

static PyObject* PyBobIpGaborSimilarity_load(PyBobIpGaborSimilarityObject* self, PyObject* args, PyObject* kwargs) {
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
  "Saves the parametrization of this Gabor jet similarity to the given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for writing")
;

static PyObject* PyBobIpGaborSimilarity_save(PyBobIpGaborSimilarityObject* self, PyObject* args, PyObject* kwargs) {
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


static PyMethodDef PyBobIpGaborSimilarity_methods[] = {
  {
    similarity_doc.name(),
    (PyCFunction)PyBobIpGaborSimilarity_similarity,
    METH_VARARGS|METH_KEYWORDS,
    similarity_doc.doc()
  },
  {
    disparity_doc.name(),
    (PyCFunction)PyBobIpGaborSimilarity_disparity,
    METH_VARARGS|METH_KEYWORDS,
    disparity_doc.doc()
  },
  {
    shift_phase_doc.name(),
    (PyCFunction)PyBobIpGaborSimilarity_shift_phase,
    METH_VARARGS|METH_KEYWORDS,
    shift_phase_doc.doc()
  },
  {
    load_doc.name(),
    (PyCFunction)PyBobIpGaborSimilarity_load,
    METH_VARARGS|METH_KEYWORDS,
    load_doc.doc()
  },
  {
    save_doc.name(),
    (PyCFunction)PyBobIpGaborSimilarity_save,
    METH_VARARGS|METH_KEYWORDS,
    save_doc.doc()
  },
  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the Gabor wavelet type struct; will be initialized later
PyTypeObject PyBobIpGaborSimilarity_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobIpGaborSimilarity(PyObject* module)
{

  // initialize the Similarity type struct
  PyBobIpGaborSimilarity_Type.tp_name = Similarity_doc.name();
  PyBobIpGaborSimilarity_Type.tp_basicsize = sizeof(PyBobIpGaborSimilarityObject);
  PyBobIpGaborSimilarity_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobIpGaborSimilarity_Type.tp_doc = Similarity_doc.doc();

  // set the functions
  PyBobIpGaborSimilarity_Type.tp_new = PyType_GenericNew;
  PyBobIpGaborSimilarity_Type.tp_init = reinterpret_cast<initproc>(PyBobIpGaborSimilarity_init);
  PyBobIpGaborSimilarity_Type.tp_dealloc = reinterpret_cast<destructor>(PyBobIpGaborSimilarity_delete);
  PyBobIpGaborSimilarity_Type.tp_methods = PyBobIpGaborSimilarity_methods;
  PyBobIpGaborSimilarity_Type.tp_getset = PyBobIpGaborSimilarity_getseters;
  PyBobIpGaborSimilarity_Type.tp_call = reinterpret_cast<ternaryfunc>(PyBobIpGaborSimilarity_similarity);

  // check that everyting is fine
  if (PyType_Ready(&PyBobIpGaborSimilarity_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobIpGaborSimilarity_Type);
  return PyModule_AddObject(module, "Similarity", (PyObject*)&PyBobIpGaborSimilarity_Type) >= 0;
}
