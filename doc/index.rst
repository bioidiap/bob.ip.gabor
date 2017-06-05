.. vim: set fileencoding=utf-8 :
.. Andre Anjos <andre.anjos@idiap.ch>
.. Mon  4 Nov 20:58:04 2013 CET
..
.. Copyright (C) 2011-2013 Idiap Research Institute, Martigny, Switzerland

.. _bob.ip.gabor:

==============================
 Bob's Gabor wavelet routines
==============================

.. todolist::

This module contains the C++ source code and the Python bindings for classes that handle the Gabor wavelet transform and Gabor jet similarities.

If you use parts of this code in a scientific publication, please cite:

.. code-block:: tex

  @inproceedings{guenther2012disparity,
    title = {Face Recognition with Disparity Corrected {G}abor Phase Differences},
    author = {G\"unther, Manuel and Haufe, Dennis and W\"urtz, Rolf P.},
    editor = {Villa, Alessandro E. P. and Duch, W{\l}odzis{\l}aw and \'Erdi, P\'eter and Masulli, Francesco and Palm, G\"unther},
    booktitle = {Artificial Neural Networks and Machine Learning},
    series = {Lecture Notes in Computer Science},
    volume = {7552},
    year = {2012},
    month = sep,
    pages = {411-418},
    publisher = {Springer},
    pdf = {http://publications.idiap.ch/downloads/papers/2012/Gunther_ICANN2012_2012.pdf}
  }


Documentation
-------------

.. toctree::
   :maxdepth: 2

   guide
   py_api
   c_cpp_api


References
----------

.. [Wiskott1997]  *Laurenz Wiskott, Jean-Marc Fellous, Norbert Krueger, Christoph Von Der Malsburg*. **Face Recognition By Elastic Bunch Graph Matching,** IEEE Transactions on Pattern Analysis and Machine Intelligence, 1997.
.. [Guenther2011] *Manuel Guenther*. **Statistical Gabor graph based techniques for the detection, recognition, classification and visualization of human faces,** PhD thesis, Technical University of Ilmenau, June 2011.
.. [Guenther2012] *Manuel Guenther, Denis Haufe, Rolf P. Wuertz*. **Face recognition with disparity corrected Gabor phase differences,** Artificial Neural Networks and Machine Learning, pp. 411-418, September 2012.


Indices and tables
------------------

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

.. include:: links.rst
