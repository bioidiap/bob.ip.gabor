/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Thu Jun 12 09:24:47 CEST 2014
 *
 * @brief Bindings for a Gabor graph
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

static auto Graph_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".Graph",
  "A class to extract Gabor jets from a Gabor transformed image (see :py:func:`bob.ip.gabor.Transform.transform`)",
  "The graph structure stores a list of nodes, where Gabor jets will be extracted from the images. "
  "These nodes are stored with **absolute** positions, so please assure that the image is large enough."
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Creates a the Gabor graph",
    "There are three different ways to create a Gabor graph. "
    "The first two ways will generate nodes in regular grid positions, while the third can specify the positions as a list of tuples.",
    true
  )
  .add_prototype("righteye, lefteye, between, along, above, below", "")
  .add_prototype("first, last, step", "")
  .add_prototype("nodes", "")
  .add_prototype("hdf5", "")
  .add_parameter("righteye, lefteye", "(int, int)", "The position of the left and the right eye of the face in the image; the positions are with respect to the person in the image, so normally ``lefteye[1] > righteye[0]``")
  .add_parameter("between", "int", "The number of nodes that should be placed between the eyes (excluding both eye nosed)")
  .add_parameter("along", "int", "The number of nodes that should be placed to the left of the right eye and to the right of the left eye (excluding the eye positions)")
  .add_parameter("above", "int", "The number of nodes that should be placed above the eyes (excluding the eye positions)")
  .add_parameter("below", "int", "The number of nodes that should be placed below the eyes (excluding the eye positions)")
  .add_parameter("first", "(int, int)", "The position of the first (top-left) node that will be placed")
  .add_parameter("last", "(int, int)", "The position of the last (bottom-right) node that will be placed; depending on the ``step`` parameter, the actual bottom-right node might be before ``last``")
  .add_parameter("step", "(int, int)", "The distance between two each nodes (in vertical and horizontal direction)")
  .add_parameter("nodes", "[(int, int)]", "The node positions that should be stored in the graph")
  .add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for reading to load the nodes of the Gabor graph from")
);

static int PyBobIpGaborGraph_init(PyBobIpGaborGraphObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist1 = Graph_doc.kwlist(3);
  char** kwlist2 = Graph_doc.kwlist(2);
  char** kwlist3 = Graph_doc.kwlist(1);
  char** kwlist4 = Graph_doc.kwlist(0);

  // get the number of command line arguments
  Py_ssize_t nargs = (args?PyTuple_Size(args):0) + (kwargs?PyDict_Size(kwargs):0);

  switch (nargs){
    case 1: {
      // two ways to call with one argument
      PyObject* k = Py_BuildValue("s", kwlist1[0]);
      auto k_ = make_safe(k);
      if (
        (kwargs && PyDict_Contains(kwargs, k)) ||
        (args && PyBobIoHDF5File_Check(PyTuple_GetItem(args, 0)))
      ){
        PyBobIoHDF5FileObject* hdf5;
        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", kwlist1, &PyBobIoHDF5File_Converter, &hdf5)) return -1;
        auto hdf5_ = make_safe(hdf5);

        self->cxx.reset(new bob::ip::gabor::Graph(*hdf5->f));
      } else {
        PyListObject* list;
        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist2, &PyList_Type, &list)) return -1;

        std::vector<blitz::TinyVector<int,2>> nodes(PyList_GET_SIZE(list));
        Py_ssize_t i = 0;
        for (auto nit = nodes.begin(); nit != nodes.end(); ++nit, ++i){
          // check that the object inside the list is a two-element int tuple
          if (!PyArg_ParseTuple(PyList_GET_ITEM(list, i), "ii", &((*nit)[0]), &((*nit)[1]))){
            PyErr_Format(PyExc_TypeError, "%s requires only tuples of two integral positions in the nodes list", Py_TYPE(self)->tp_name);
            return -1;
          }
        }
        self->cxx.reset(new bob::ip::gabor::Graph(nodes));
      }
      break;
    }

    case 3:{
      blitz::TinyVector<int,2> first, last, step;
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(ii)(ii)(ii)", kwlist3, &first[0], &first[1], &last[0], &last[1], &step[0], &step[1])) return -1;
      self->cxx.reset(new bob::ip::gabor::Graph(first, last, step));

    }
    break;

    case 6:{
      blitz::TinyVector<int,2> right, left;
      int between, along, above, below;
      if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(ii)(ii)iiii", kwlist4, &right[0], &right[1], &left[0], &left[1], &between, &along, &above, &below)) return -1;
      self->cxx.reset(new bob::ip::gabor::Graph(right, left,  between, along, above, below));
    }
    break;

    default:
      return -1;
  }

  return 0;
BOB_CATCH_MEMBER("Graph constructor", -1)
}

static void PyBobIpGaborGraph_delete(PyBobIpGaborGraphObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}

int PyBobIpGaborGraph_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobIpGaborGraph_Type));
}

static PyObject* PyBobIpGaborGraph_RichCompare(PyBobIpGaborGraphObject* self, PyObject* other, int op) {
BOB_TRY
  if (!PyBobIpGaborGraph_Check(other)) {
    PyErr_Format(PyExc_TypeError, "cannot compare `%s' with `%s'", Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
    return 0;
  }
  auto other_ = reinterpret_cast<PyBobIpGaborGraphObject*>(other);
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
BOB_CATCH_MEMBER("RichCompare", 0);
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

static auto numberOfNodes_doc = bob::extension::VariableDoc(
  "number_of_nodes",
  "int",
  "The number of nodes that this Graph will extract"
);
PyObject* PyBobIpGaborGraph_numberOfNodes(PyBobIpGaborGraphObject* self, void*){
BOB_TRY
  return Py_BuildValue("i", self->cxx->numberOfNodes());
BOB_CATCH_MEMBER("number_of_nodes", 0);
}

static auto nodes_doc = bob::extension::VariableDoc(
  "nodes",
  "[(int, int)]",
  "The list of node positions extracted by this graph",
  ".. warning:: \n"
  "   You can use this variable to reset the nodes in this graph, but only by using the ``=`` operator.\n"
  "   Something like ``graph.nodes[0] = (1,1)`` will **not** have the expected outcome!"
);
PyObject* PyBobIpGaborGraph_getNodes(PyBobIpGaborGraphObject* self, void*){
BOB_TRY
  // get the data
  auto nodes = self->cxx->nodes();
  // populate a list
  PyObject* list = PyList_New(nodes.size());
  for (Py_ssize_t i = 0; i < (Py_ssize_t)nodes.size(); ++i){
    PyList_SET_ITEM(list, i, Py_BuildValue("(ii)", nodes[i][0], nodes[i][1]));
  }
  return list;
BOB_CATCH_MEMBER("nodes", 0);
}

int PyBobIpGaborGraph_setNodes(PyBobIpGaborGraphObject* self, PyObject* value, void*){
BOB_TRY
  if (!PyList_Check(value)){
    PyErr_Format(PyExc_TypeError, "%s requires only tuples of two integral positions in the nodes member", Py_TYPE(self)->tp_name);
  }
  PyListObject* list = reinterpret_cast<PyListObject*>(value);
  std::vector<blitz::TinyVector<int,2>> nodes(PyList_GET_SIZE(list));
  Py_ssize_t i = 0;
  for (auto nit = nodes.begin(); nit != nodes.end(); ++nit, ++i){
    // check that the object inside the list is a two-element int tuple
    if (!PyArg_ParseTuple(PyList_GET_ITEM(list, i), "ii", &((*nit)[0]), &((*nit)[1]))){
      PyErr_Format(PyExc_TypeError, "%s requires only tuples of two integral positions in the nodes member", Py_TYPE(self)->tp_name);
      return -1;
    }
  }
  self->cxx->nodes(nodes);
  return 0;
BOB_CATCH_MEMBER("nodes", 0);
}

static PyGetSetDef PyBobIpGaborGraph_getseters[] = {
  {
    numberOfNodes_doc.name(),
    (getter)PyBobIpGaborGraph_numberOfNodes,
    0,
    numberOfNodes_doc.doc(),
    0
  },
  {
    nodes_doc.name(),
    (getter)PyBobIpGaborGraph_getNodes,
    (setter)PyBobIpGaborGraph_setNodes,
    nodes_doc.doc(),
    0
  },
  {0}  /* Sentinel */
};


/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/

static auto extract_doc = bob::extension::FunctionDoc(
  "extract",
  "This function extracts all Gabor jets from the given trafo image for all nodes of the graph",
  "The trafo image should have been created by a call to :py:func:`bob.ip.gabor.Transform.transform`. "
  "It must be assured that all nodes of the graph are inside the image boundaries of the trafo image.\n\n"
  ".. note::\n\n  The function `__call__` is a synonym for this function.",
  true
)
.add_prototype("trafo_image, jets")
.add_prototype("trafo_image", "jets")
.add_parameter("trafo_image", "array_like (complex, 3D)", "The Gabor wavelet transformed image, e.g., the result of :py:func:`bob.ip.gabor.Transform.transform`")
.add_parameter("jets", "[:py:class:`bob.ip.gabor.Jet`]", "The list of Gabor jets that will be filled during the extraction process; The number of jets must be identical to :py:attr:`number_of_nodes`, and the jets must have the correct :py:attr:`bob.ip.gabor.Jet.length`.")
.add_return("jets", "[:py:class:`bob.ip.gabor.Jet`]", "The list of Gabor jets extracted at the :py:attr:`nodes` from the given ``trafo_image``.")
;

static PyObject* PyBobIpGaborGraph_extract(PyBobIpGaborGraphObject* self, PyObject* args, PyObject* kwargs) {
BOB_TRY
  char** kwlist = extract_doc.kwlist();

  PyBlitzArrayObject* trafo_image;
  PyObject* jets = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&|O!", kwlist, &PyBlitzArray_Converter, &trafo_image, &PyList_Type, &jets)) return 0;

  auto trafo_image_ = make_safe(trafo_image);

  if (trafo_image->ndim != 3 || trafo_image->type_num != NPY_COMPLEX128) {
    PyErr_Format(PyExc_TypeError, "`%s' only accepts 3-dimensional arrays of complex type for `input`", Py_TYPE(self)->tp_name);
    return 0;
  }

  if (jets){
    if ((int)PyList_Size(jets) != self->cxx->numberOfNodes()){
      PyErr_Format(PyExc_RuntimeError, "`%s' requires the `jets` parameter to be a list of bob.ip.gabor.Jet objects of length %d, but it has length %" PY_FORMAT_SIZE_T "d)", Py_TYPE(self)->tp_name, self->cxx->numberOfNodes(), PyList_Size(jets));
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_Size(jets); ++i){
      if (!PyBobIpGaborJet_Check(PyList_GET_ITEM(jets, i))){
        PyErr_Format(PyExc_RuntimeError, "`%s' requires all elements of the `jets` parameter to be of type bob.ip.gabor.Jet, but element %" PY_FORMAT_SIZE_T "d isn't", Py_TYPE(self)->tp_name, i);
        return 0;
      }
    }
    Py_INCREF(jets);
  } else {
    // pre-allocate the Gabor jets
    jets = PyList_New(self->cxx->numberOfNodes());
    int jet_len = trafo_image->shape[0];
    for (Py_ssize_t i = 0; i < PyList_Size(jets); ++i){
      PyBobIpGaborJetObject* jet = reinterpret_cast<PyBobIpGaborJetObject*>(PyBobIpGaborJet_Type.tp_alloc(&PyBobIpGaborJet_Type, 0));
      jet->cxx.reset(new bob::ip::gabor::Jet(jet_len));
      PyList_SET_ITEM(jets, i, Py_BuildValue("N",jet));
    }
  }

  std::vector<boost::shared_ptr<bob::ip::gabor::Jet>> output(PyList_Size(jets));
  for (Py_ssize_t i = 0; i < PyList_Size(jets); ++i){
    output[i] = (reinterpret_cast<PyBobIpGaborJetObject*>(PyList_GET_ITEM(jets,i)))->cxx;
  }

  self->cxx->extract(*PyBlitzArrayCxx_AsBlitz<std::complex<double>,3>(trafo_image), output);
  return jets;
BOB_CATCH_MEMBER("extract", 0)
}


static auto load_doc = bob::extension::FunctionDoc(
  "load",
  "Loads the list of node positions of the Gabor graph from the given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file opened for reading")
;

static PyObject* PyBobIpGaborGraph_load(PyBobIpGaborGraphObject* self, PyObject* args, PyObject* kwargs) {
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
  "Saves the the list of node positions of the Gabor graph to the given HDF5 file",
  0,
  true
)
.add_prototype("hdf5")
.add_parameter("hdf5", ":py:class:`bob.io.base.HDF5File`", "An HDF5 file open for writing")
;

static PyObject* PyBobIpGaborGraph_save(PyBobIpGaborGraphObject* self, PyObject* args, PyObject* kwargs) {
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


static PyMethodDef PyBobIpGaborGraph_methods[] = {
  {
    extract_doc.name(),
    (PyCFunction)PyBobIpGaborGraph_extract,
    METH_VARARGS|METH_KEYWORDS,
    extract_doc.doc()
  },
  {
    load_doc.name(),
    (PyCFunction)PyBobIpGaborGraph_load,
    METH_VARARGS|METH_KEYWORDS,
    load_doc.doc()
  },
  {
    save_doc.name(),
    (PyCFunction)PyBobIpGaborGraph_save,
    METH_VARARGS|METH_KEYWORDS,
    save_doc.doc()
  },
  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the Gabor wavelet type struct; will be initialized later
PyTypeObject PyBobIpGaborGraph_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobIpGaborGraph(PyObject* module)
{

  // initialize the Graph type struct
  PyBobIpGaborGraph_Type.tp_name = Graph_doc.name();
  PyBobIpGaborGraph_Type.tp_basicsize = sizeof(PyBobIpGaborGraphObject);
  PyBobIpGaborGraph_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  PyBobIpGaborGraph_Type.tp_doc = Graph_doc.doc();

  // set the functions
  PyBobIpGaborGraph_Type.tp_new = PyType_GenericNew;
  PyBobIpGaborGraph_Type.tp_init = reinterpret_cast<initproc>(PyBobIpGaborGraph_init);
  PyBobIpGaborGraph_Type.tp_dealloc = reinterpret_cast<destructor>(PyBobIpGaborGraph_delete);
  PyBobIpGaborGraph_Type.tp_methods = PyBobIpGaborGraph_methods;
  PyBobIpGaborGraph_Type.tp_getset = PyBobIpGaborGraph_getseters;
  PyBobIpGaborGraph_Type.tp_call = reinterpret_cast<ternaryfunc>(PyBobIpGaborGraph_extract);
  PyBobIpGaborGraph_Type.tp_richcompare = reinterpret_cast<richcmpfunc>(PyBobIpGaborGraph_RichCompare);

  // check that everyting is fine
  if (PyType_Ready(&PyBobIpGaborGraph_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobIpGaborGraph_Type);
  return PyModule_AddObject(module, "Graph", (PyObject*)&PyBobIpGaborGraph_Type) >= 0;
}
