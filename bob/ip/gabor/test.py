#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Manuel Guenther <manuel.guenther@idiap.ch>
# Wed Jun  4 21:22:35 CEST 2014
#
# Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland

"""Tests Gabor wavelet functionality
"""

import numpy
import nose.tools
import math, cmath
import tempfile

import bob.io.base
import bob.io.image
import bob.sp
import bob.ip.gabor
import bob.ip.color

import bob.io.base.test_utils

regenerate_references = False

def test_wavelet():
  # check that the wavelet in frequency domain is just a Gaussian moved to
  k = [math.pi/2.] * 2
  sigma = 2. * math.pi
  size = 32
  offset = 8

  # get a wavelet
  wavelet = bob.ip.gabor.Wavelet(resolution = (size,size), frequency = k, sigma = sigma, epsilon = 1e-5, dc_free = False)

  # compute the Gaussian that it should be
  gaussian = numpy.ndarray((size,size), numpy.float64)
  sigma_gaussian = sigma / math.sqrt(k[0]**2 + k[1]**2)
  for y in range(size):
    wy = (y-offset) * 2. * math.pi / size
    for x in range(size):
      wx = (x-offset) * 2. * math.pi / size
      gaussian[y,x] = math.exp(-(wy**2 + wx**2) * sigma_gaussian**2 / 2.)
  # check that they are similar (not identical, though)
  assert numpy.allclose(wavelet.wavelet, gaussian, atol=1e-4, rtol=1e-4)

  # the point-response to the wavelet should be the wavelet itself
  offset = size//2
  image = numpy.zeros((size,size), numpy.complex128)
  image[offset,offset] = 1.
  ai = bob.sp.fft(image)
  tf = wavelet.transform(ai)
  convoluted = bob.sp.ifft(tf)
  # check that the convoluted image is wavelet-like
  spat_wavelet = bob.sp.ifft(wavelet.wavelet.astype(numpy.complex128))
  for y in range(size):
    for x in range(size):
      diff = cmath.phase(spat_wavelet[y,x]) - cmath.phase(convoluted[(y+offset)%size, (x+offset)%size])
      assert abs(diff) < 1e-6 or abs(diff - 2*math.pi) < 1e-6 or abs(diff + 2*math.pi) < 1e-6

  # check that the __call__ function works as well
  tf2 = wavelet(ai)
  assert numpy.allclose(tf, tf2)


def test_transform():
  # check that the Transform class is doing something useful
  d = 8
  s = 5
  sigma = 2. * math.pi
  k_max = math.pi/2.
  k_fac = math.sqrt(2.)
  gwt = bob.ip.gabor.Transform(number_of_directions=d, number_of_scales=s, sigma=sigma, k_max=k_max, k_fac=k_fac, power_of_k=0., dc_free=True, epsilon=1e-10)

  assert gwt.number_of_wavelets == s*d
  assert gwt.number_of_scales == s
  assert gwt.number_of_directions == d
  assert gwt.sigma == sigma
  assert gwt.k_max == k_max
  assert gwt.k_fac == k_fac
  assert gwt.power_of_k == 0.
  assert gwt.dc_free == True

  # at the beginning, the k-vectors should be created, but no wavelets
  assert len(gwt.wavelet_frequencies) == s*d
  assert len(gwt.wavelets) == 0
  # check some of the frequencies
  f = gwt.wavelet_frequencies
  assert abs(f[0][0]) < 1e-8
  assert abs(f[0][1] - k_max) < 1e-8
  assert abs(f[d//2][0] - k_max) < 1e-8
  assert abs(f[d//2][1]) < 1e-8

  assert abs(f[2*d][0]) < 1e-8
  assert abs(f[2*d][1] - k_max * k_fac**2) < 1e-8
  assert abs(f[2*d+d//2][0] - k_max * k_fac**2) < 1e-8
  assert abs(f[2*d+d//2][1]) < 1e-8

  # now, create wavelets for resolution
  size = 64
  gwt.generate_wavelets(height=size, width=size)
  wavelets = gwt.wavelets
  assert len(wavelets) == s*d

  # check that all wavelets are generated correctly
  for i,k in enumerate(f):
    # create wavelet
    w = bob.ip.gabor.Wavelet((size, size), k, sigma, 0., True, 1e-10)
    assert numpy.allclose(w.wavelet, wavelets[i].wavelet)

  # load test image
  image = bob.ip.color.rgb_to_gray(bob.io.base.load(bob.io.base.test_utils.datafile("testimage.jpg", 'bob.ip.gabor')))

  trafo_image = gwt(image)
  assert trafo_image.shape[0] == gwt.number_of_wavelets
  assert trafo_image.shape[1:3] == image.shape
  assert trafo_image.dtype == numpy.complex128

  # load the GWT from HDF5
  gwt_file = bob.io.base.test_utils.datafile("gwt.hdf5", 'bob.ip.gabor')
  if regenerate_references:
    gwt.save(bob.io.base.HDF5File(gwt_file, 'w'))
  reference_gwt = bob.ip.gabor.Transform(bob.io.base.HDF5File(gwt_file))
  assert gwt == reference_gwt
  reference_trafo_image = numpy.ndarray(trafo_image.shape, numpy.complex128)
  reference_gwt(image, reference_trafo_image)
  assert numpy.allclose(trafo_image, reference_trafo_image)



def test_jet():
  gwt = bob.ip.gabor.Transform()

  # load test image
  image = bob.ip.color.rgb_to_gray(bob.io.base.load(bob.io.base.test_utils.datafile("testimage.jpg", 'bob.ip.gabor')))

  trafo_image = gwt(image)


  # get an empty jet of size 5
  jet = bob.ip.gabor.Jet(5)
  assert numpy.all(jet.jet == numpy.zeros((2,5)))

  def get():
    jet = bob.ip.gabor.Jet(trafo_image=trafo_image, position=(-1,-1), normalize=True)
  nose.tools.assert_raises(RuntimeError, get)
  def get2():
    jet = bob.ip.gabor.Jet(trafo_image=trafo_image)
  nose.tools.assert_raises(RuntimeError, get2)
  def cmplx():
    jet = bob.ip.gabor.Jet(complex=trafo_image, position=(5,5), normalize=True)
  nose.tools.assert_raises(TypeError, cmplx)

  # extract two Gabor jets
  jet1 = bob.ip.gabor.Jet(trafo_image=trafo_image, position=(5,5), normalize=True)
  jet2 = bob.ip.gabor.Jet(complex=trafo_image[:,5,5], normalize=False)
  jet3 = bob.ip.gabor.Jet()
  jet3.init(complex=trafo_image[:,5,5], normalize=True)
  jet4 = bob.ip.gabor.Jet()
  jet4.extract(trafo_image=trafo_image, position=(5,5), normalize=True)

  assert len(jet1.abs) == gwt.number_of_wavelets
  assert abs(jet1.normalize() - 1.) < 1e-8
  assert not numpy.allclose(jet1.abs, jet2.abs)
  assert numpy.allclose(jet1.phase, jet2.phase)
  assert jet3.length == gwt.number_of_wavelets
  assert jet4.length == gwt.number_of_wavelets
  assert numpy.allclose(jet1.jet, jet3.jet)
  assert numpy.allclose(jet1.jet, jet4.jet)

  # check the first (10,10) pixel of the Gabor wavelet transform to be the same as last time
  jet_file = bob.io.base.test_utils.datafile("testjet.hdf5", 'bob.ip.gabor')
  if regenerate_references:
    jet2.save(bob.io.base.HDF5File(jet_file, 'w'))
  reference_jet = bob.ip.gabor.Jet(bob.io.base.HDF5File(jet_file))
  assert numpy.allclose(jet2.jet, reference_jet.jet)

  # test jet averaging
  d = numpy.conjugate(jet1.complex)
  conjugated = bob.ip.gabor.Jet(d)
  averaged = bob.ip.gabor.Jet([jet1, conjugated], True)
  # assert that the phases are either 0 or PI
  for p in averaged.phase:
    assert abs(p) < 1e-8 or abs(abs(p)-math.pi) < 1e-8



def test_graph():
  # create grid graph
  graph = bob.ip.gabor.Graph(first=(10,10), last=(105,60), step=(20,10))
  assert graph.number_of_nodes == 30
  assert graph.nodes[0] == (10,10)
  assert graph.nodes[-1] == (90,60)
  # set graph nodes
  graph.nodes = [(0,0), (1,1)]
  assert graph.number_of_nodes == 2
  assert graph.nodes[0] == (0,0)
  assert graph.nodes[1] == (1,1)

  # create graph
  graph = bob.ip.gabor.Graph((177,148), (191,142), between=3, above=1, along=1, below=4)
  assert graph.number_of_nodes == 42
  assert (177,148) in graph.nodes
  assert (191,142) in graph.nodes

  # test IO
  graph_file = bob.io.base.test_utils.datafile("testgraph.hdf5", 'bob.ip.gabor')
  if regenerate_references:
    graph.save(bob.io.base.HDF5File(graph_file, 'w'))
  reference_graph = bob.ip.gabor.Graph(bob.io.base.HDF5File(graph_file))
  assert graph == reference_graph

  # test extraction from image
  image = bob.ip.color.rgb_to_gray(bob.io.base.load(bob.io.base.test_utils.datafile("testimage.jpg", 'bob.ip.gabor')))
  gwt = bob.ip.gabor.Transform()
  trafo_image = gwt(image)

  def empty():
    graph.extract(trafo_image, [])
  nose.tools.assert_raises(RuntimeError, empty)

  def wrong():
    graph.extract(trafo_image, [(1,2)]*graph.number_of_nodes)
  nose.tools.assert_raises(RuntimeError, wrong)

  jets = [bob.ip.gabor.Jet() for i in range(graph.number_of_nodes)]
  graph.extract(trafo_image, jets)
  jets_file = bob.io.base.test_utils.datafile("testjets.hdf5", 'bob.ip.gabor')
  if regenerate_references:
    bob.ip.gabor.save_jets(jets, bob.io.base.HDF5File(jets_file, 'w'))
  reference_jets = bob.ip.gabor.load_jets(bob.io.base.HDF5File(jets_file))

  assert len(jets) == len(reference_jets)
  for i in range(len(jets)):
    assert numpy.allclose(jets[i].jet, reference_jets[i].jet)



def test_similarity():
  # here we need the same GWT parameters as used to generate the Gabor jet!
  gwt = bob.ip.gabor.Transform()
  jet = bob.ip.gabor.Jet(bob.io.base.HDF5File(bob.io.base.test_utils.datafile("testjet.hdf5", 'bob.ip.gabor')))
  jet.normalize()

  for i, type in enumerate(('ScalarProduct', 'Canberra', 'Disparity', 'PhaseDiff', 'PhaseDiffPlusCanberra')):
    sim = bob.ip.gabor.Similarity(type=type, transform=gwt)
    assert (sim.similarity(jet,jet) - 1.) < 1e-8
    disp = sim.last_disparity
    if i < 2:
      assert math.isnan(disp[0])
      assert math.isnan(disp[1])
    else:
      assert abs(disp[0]) < 1e-8
      assert abs(disp[1]) < 1e-8
      assert (sim.disparity(jet,jet) == disp)

  # load similarity from file
  sim_file = bob.io.base.test_utils.datafile("testsim.hdf5", 'bob.ip.gabor')
  if regenerate_references:
    sim.save(bob.io.base.HDF5File(sim_file, 'w'))
  reference_sim = bob.ip.gabor.Similarity(bob.io.base.HDF5File(sim_file))
  assert reference_sim.transform == gwt


if __name__ == '__main__':
  test_graph()


