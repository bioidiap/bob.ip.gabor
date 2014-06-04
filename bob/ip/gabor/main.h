#ifndef BOB_IP_GABOR_BINDINGS
#define BOB_IP_GABOR_BINDINGS_H

#include <Python.h>

#include <bob.blitz/capi.h>
#include <bob.blitz/cleanup.h>
#include <bob.extension/documentation.h>
#include <bob.io.base/api.h>

#include "cpp/Wavelet.h"
#include "cpp/Transform.h"

// Gabor wavelet
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Wavelet> cxx;
} PyBobIpGaborWaveletObject;

extern PyTypeObject PyBobIpGaborWaveletType;

bool init_BobIpGaborWavelet(PyObject* module);


// Gabor wavelet transform
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Transform> cxx;
} PyBobIpGaborWaveletTransformObject;

extern PyTypeObject PyBobIpGaborWaveletTransformType;

bool init_BobIpGaborWaveletTransform(PyObject* module);


#endif // BOB_IP_GABOR_BINDINGS
