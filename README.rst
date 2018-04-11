.. vim: set fileencoding=utf-8 :
.. Sun 14 Aug 2016 20:35:41 CEST

.. image:: http://img.shields.io/badge/docs-stable-yellow.svg
   :target: https://www.idiap.ch/software/bob/docs/bob/bob.ip.gabor/stable/index.html
.. image:: http://img.shields.io/badge/docs-latest-orange.svg
   :target: https://www.idiap.ch/software/bob/docs/bob/bob.ip.gabor/master/index.html
.. image:: https://gitlab.idiap.ch/bob/bob.ip.gabor/badges/master/build.svg
   :target: https://gitlab.idiap.ch/bob/bob.ip.gabor/commits/master
.. image:: https://gitlab.idiap.ch/bob/bob.ip.gabor/badges/master/coverage.svg
   :target: https://gitlab.idiap.ch/bob/bob.ip.gabor/commits/master
.. image:: https://img.shields.io/badge/gitlab-project-0000c0.svg
   :target: https://gitlab.idiap.ch/bob/bob.ip.gabor
.. image:: http://img.shields.io/pypi/v/bob.ip.gabor.svg
   :target: https://pypi.python.org/pypi/bob.ip.gabor


====================================================================
 Tools for Gabor Wavelets, Transform, Jet Extraction and Similarity
====================================================================

This package is part of the signal-processing and machine learning toolbox
Bob_. It contains a set of C++ code and Python bindings for Bob's Image
Processing tools concerning Gabor wavelets, the Gabor wavelet transform, Gabor
jet extraction in a grid graph structure and Gabor jet similarity functions
including a Gabor jet disparity estimation.

If you use parts of this code in a scientific publication, please cite::

  @inproceedings{guenther2012disparity,
    title = {Face Recognition with Disparity Corrected {G}abor Phase Differences},
    author = {G{\"{u}}nther, Manuel and Haufe, Dennis and W{\"{u}}rtz, Rolf P.},
    editor = {Villa, Alessandro E. P. and Duch, W{\l}odzis{\l}aw and {\'{E}}rdi, P{\'{e}}ter and Masulli, Francesco and Palm, G{\"{u}}nther},
    booktitle = {Artificial Neural Networks and Machine Learning},
    series = {Lecture Notes in Computer Science},
    volume = {7552},
    year = {2012},
    month = sep,
    pages = {411-418},
    publisher = {Springer},
    pdf = {http://publications.idiap.ch/downloads/papers/2012/Gunther_ICANN2012_2012.pdf}
  }



Installation
------------

Complete Bob's `installation`_ instructions. Then, to install this package,
run::

  $ conda install bob.ip.gabor


Contact
-------

For questions or reporting issues to this software package, contact our
development `mailing list`_.


.. Place your references here:
.. _bob: https://www.idiap.ch/software/bob
.. _installation: https://www.idiap.ch/software/bob/install
.. _mailing list: https://www.idiap.ch/software/bob/discuss