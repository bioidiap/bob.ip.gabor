/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Tue Jun  3 17:44:24 CEST 2014
 *
 * @brief Bindings to bob::ip color converters
 */

#ifndef BOB_IP_GABOR_MAIN_H
#define BOB_IP_GABOR_MAIN_H

#include <bob.blitz/capi.h>
#include <bob.blitz/cleanup.h>
#include <bob.extension/documentation.h>
#include <bob.io.base/api.h>

#include <bob.ip.gabor/Wavelet.h>
#include <bob.ip.gabor/Transform.h>
#include <bob.ip.gabor/Jet.h>
#include <bob.ip.gabor/Graph.h>
#include <bob.ip.gabor/Similarity.h>

// Gabor wavelet
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Wavelet> cxx;
} PyBobIpGaborWaveletObject;

extern PyTypeObject PyBobIpGaborWaveletType;

bool init_BobIpGaborWavelet(PyObject* module);

int PyBobIpGaborWavelet_Check(PyObject* o);


// Gabor wavelet transform
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Transform> cxx;
} PyBobIpGaborWaveletTransformObject;

extern PyTypeObject PyBobIpGaborWaveletTransformType;

bool init_BobIpGaborWaveletTransform(PyObject* module);

int PyBobIpGaborWaveletTransform_Check(PyObject* o);


// Gabor jet
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Jet> cxx;
} PyBobIpGaborJetObject;

extern PyTypeObject PyBobIpGaborJetType;

bool init_BobIpGaborJet(PyObject* module);

int PyBobIpGaborJet_Check(PyObject* o);


// Gabor graph
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Graph> cxx;
} PyBobIpGaborGraphObject;

extern PyTypeObject PyBobIpGaborGraphType;

bool init_BobIpGaborGraph(PyObject* module);

int PyBobIpGaborGraph_Check(PyObject* o);


// Gabor jet similarity
typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::ip::gabor::Similarity> cxx;
} PyBobIpGaborJetSimilarityObject;

extern PyTypeObject PyBobIpGaborJetSimilarityType;

bool init_BobIpGaborJetSimilarity(PyObject* module);

int PyBobIpGaborJetSimilarity_Check(PyObject* o);


#endif // BOB_IP_GABOR_MAIN_H
