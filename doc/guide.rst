.. vim: set fileencoding=utf-8 :
.. Manuel Guenther <manuek.guether@idiap.ch>
.. Fri Jun 13 09:53:03 CEST 2014

=================
 Exemplary Usage
=================

.. testsetup:: *

  from __future__ import print_function
  import numpy
  import math
  import bob.sp
  import bob.io.base
  import bob.io.base.test_utils
  import bob.ip.gabor


In this documentation, exemplary use cases for the Gabor wavelet transform (and related) classes are detailed.


Gabor wavelets
--------------

The :py:class:`bob.ip.gabor.Wavelet` class can be used to create Gabor wavelets.
The parametrization is according to what is explained in detail in [Guenther2011]_.

The Gabor wavelets by default are in frequency domain.
To create a Gabor wavelet of size ``(128, 128)`` with the vertical orientation and frequency :math:`\frac{\pi}{8}`, you call:

.. doctest::

   >>> wavelet = bob.ip.gabor.Wavelet(resolution = (128, 128), frequency = (math.pi/2, 0))

To compute the wavelet transform, an image of the same resolution in frequency domain must be present.
To generate an image in frequency domain, one can use the :py:func:`bob.sp.fft` function:

.. doctest::

   >>> test_image = numpy.zeros((128,128), numpy.uint8)
   >>> test_image[32:96, 32:96] = 255
   >>> freq_image = bob.sp.fft(test_image.astype(numpy.complex128))

Now, the wavelet transform with the given wavelet can be applied:

.. doctest::

   >>> transformed_freq_image = wavelet.transform(freq_image)

To get the transformed image in spatial domain, it needs to be transformed back, e.g., using :py:func:`bob.sp.ifft`:

.. doctest::

   >>> transformed_image = bob.sp.ifft(transformed_freq_image)

Please note that the resulting image is complex valued.
You can look at its real and its absolute components:

.. doctest::

   >>> real_image = numpy.real(transformed_image)
   >>> abs_image = numpy.abs(transformed_image)

A full working example (including the plotting) is given below:

.. plot:: plot/wavelet.py
   :include-source: True


Gabor wavelet transform
-----------------------

The :py:class:`bob.ip.gabor.Transform` class is performs a Gabor wavelet transform using a discrete family of Gabor wavelets.
The family of Gabor wavelets is composed of wavelets in different scales and orientations.
By default, the family consists of Gabor wavelets in 5 scales and 8 orientations:

.. doctest::

   >>> gwt = bob.ip.gabor.Transform()
   >>> gwt.number_of_scales
   5
   >>> gwt.number_of_directions
   8
   >>> gwt.number_of_wavelets
   40

When transforming an image, all Gabor wavelets will be applied to it.
In opposition to the :py:class:`bob.ip.gabor.Wavelet` class (see above), the parameters to the :py:class:`bob.ip.gabor.Transform` expects all input and output images to be in spatial domain:

.. doctest::

   >>> trafo_image = gwt.transform(test_image)
   >>> trafo_image.shape
   (40, 128, 128)
   >>> trafo_image.dtype
   dtype('complex128')

The result is a 40 layer image that contains the complex-valued results of the transform.
A few of these results (together with the according wavelets) can be seen here:

.. plot:: plot/transform.py
   :include-source: True

The first row displays the Gabor wavelet family in frequency domain can be obtained by (cf. Figure 2.2 of [Guenther2011]_).


Gabor jet and their similarities
--------------------------------

A Gabor jet  :py:class:`bob.ip.gabor.Jet` is the collection of the (complex valued) responses of all Gabor wavelets of the family **at a certain point in the image**.
The Gabor jet is a local texture descriptor, that can be used for various applications.
To extract the texture from the right eye landmark from a facial image, one can simply call:

.. doctest::

  >>> image = bob.io.base.load(bob.io.base.test_utils.datafile("testimage.hdf5", 'bob.ip.gabor'))
  >>> gwt = bob.ip.gabor.Transform()
  >>> trafo_image = gwt(image)
  >>> eye_jet = bob.ip.gabor.Jet(trafo_image, (177, 131))
  >>> len(eye_jet)
  40

One of these applications is to locate the texture in a given image.
E.g., one might locate the position of the eye by scanning over the whole image.
At each position in the image, the similarity between the reference Gabor jet and the Gabor jet at this location is computed using a :py:class:`bob.ip.gabor.Similarity`.
For this computation, both traditional [Wiskott1997]_ and innovative [Guenther2012]_ similarity functions can be used.
A more detailed description of implemented Gabor jet similarity functions can be obtained in the documentation of :py:class:`bob.ip.gabor.Similarity`.
In fact, since the texture descriptor is stable against small shifts, only every 4th pixel will be extracted, and the original offset position is not included:

.. plot:: plot/similarity.py
   :include-source: True

As can be clearly seen, **both** eye regions have high similarities with both similarity functions (remember, we are searching with only the right eye Gabor jet).
The difference is in the other regions of the face.
While the traditional cosine similarity (aka. ``'ScalarProduct'``) has high similarity values all over the image, the novel ``'Disparity'`` similarity highlights specifically the eye regions.

The disparity similarity function has even another use case.
It can estimate the disparity (difference in spatial position) between two Gabor jets, as long as they stem from a similar region:

.. doctest::

   >>> disp_sim = bob.ip.gabor.Similarity("Disparity", gwt)
   >>> pos = (231, 173)
   >>> dist = (5, 6)
   >>> jet1 = bob.ip.gabor.Jet(trafo_image, pos)
   >>> jet2 = bob.ip.gabor.Jet(trafo_image, (pos[0] - dist[0], pos[1] - dist[1]))
   >>> print ("%1.3f, %1.3f" % tuple(disp_sim.disparity(jet1, jet2)))
   4.816, 5.683

In this small example we have used Gabor jets from the same image to compute the disparity.
However, it has also been shown in [Guenther2011]_ that also a Gabor jet from one image can be found in another image, even of a different person.
Hence, this function can be used to localize landmarks.

.. note::
  The area, where useful disparities can be computed, can be increased by using a different parametrization of the Gabor wavelet transform, e.g., by increasing the ``number_of_scales`` in the :py:class:`bob.ip.gabor.Transform` constructor.


Gabor graphs
------------

Finally, graphs of Gabor jets can be used to identify a person.
The :py:class:`bob.ip.gabor.Graph` class is designed to extract Gabor jets at regular grid positions in the image.
Particularly for face recognition, the grid graph can be aligned to eye positions, but also a regular grid can be created by specifying the ``first`` and the ``last`` node, as well as the ``step`` width:

.. doctest::

   >>> graph = bob.ip.gabor.Graph(first=(100,100), last=(image.shape[0]-100, image.shape[1]-100), step = (20, 20))
   >>> graph.number_of_nodes
   136
   >>> graph.nodes[0]
   (100, 100)
   >>> graph.nodes[135]
   (420, 240)

This graph can be used to extract Gabor jets from a Gabor transformed image:

.. doctest::

   >>> jets = graph.extract(trafo_image)
   >>> len(jets)
   136

When graphs are extracted from two facial images, the average similarity of the Gabor jets can be used to define, whether two images contain the the same identities.
A complete example on the AT&T database can be found in the `bob.example.faceverify <http://pypi.python.org/pypi/bob.example.faceverify>`_ package.

