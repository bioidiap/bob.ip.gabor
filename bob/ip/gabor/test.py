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


def test_jet():
  gwt = bob.ip.gabor.Transform()

  # load test image
  image = bob.ip.color.rgb_to_gray(bob.io.base.load(bob.io.base.test_utils.datafile("testimage.jpg", 'bob.ip.gabor')))

  trafo_image = gwt(image)

  def get():
    jet = bob.ip.gabor.Jet(trafo_image=trafo_image, position=(-1,-1), normalize=True)
  nose.tools.assert_raises(RuntimeError, get)
  def get2():
    jet = bob.ip.gabor.Jet(trafo_image=trafo_image)
  nose.tools.assert_raises(TypeError, get2)
  def cmplx():
    jet = bob.ip.gabor.Jet(complex=trafo_image, position=(5,5), normalize=True)
  nose.tools.assert_raises(TypeError, cmplx)

  # extract two Gabor jets
  jet1 = bob.ip.gabor.Jet(trafo_image=trafo_image, position=(5,5), normalize=True)
  jet2 = bob.ip.gabor.Jet(complex=trafo_image[:,5,5], normalize=False)

  assert len(jet1.abs) == gwt.number_of_wavelets
  assert abs(jet1.normalize() - 1.) < 1e-8
  assert not numpy.allclose(jet1.abs, jet2.abs)
  assert numpy.allclose(jet1.phase, jet2.phase)


if __name__ == '__main__':
  test_jet()


