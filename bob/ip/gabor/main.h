/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Tue Jun  3 17:44:24 CEST 2014
 *
 * @brief Bindings to bob::ip color converters
 */

#ifndef BOB_IP_GABOR_BINDINGS
#define BOB_IP_GABOR_BINDINGS_H

#include <Python.h>

#include <bob.blitz/capi.h>
#include <bob.blitz/cleanup.h>
#include <bob.extension/documentation.h>
#include <bob.io.base/api.h>

#include "cpp/Wavelet.h"
#include "cpp/Transform.h"
#include "cpp/Jet.h"

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


// Gabor jet
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Jet> cxx;
} PyBobIpGaborJetObject;

extern PyTypeObject PyBobIpGaborJetType;

bool init_BobIpGaborJet(PyObject* module);


#endif // BOB_IP_GABOR_BINDINGS
