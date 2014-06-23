.. vim: set fileencoding=utf-8 :
.. Andre Anjos <andre.anjos@idiap.ch>
.. Thu 30 Jan 08:46:53 2014 CET

.. image:: http://travis-ci.org/bioidiap/bob.ip.gabor.svg?branch=master
   :target: https://travis-ci.org/bioidiap/bob.ip.gabor
.. image:: http://img.shields.io/badge/docs-latest-orange.png
   :target: https://www.idiap.ch/software/bob/docs/latest/bioidiap/bob.ip.gabor/master/index.html
.. image:: https://coveralls.io/repos/bioidiap/bob.ip.gabor/badge.png
   :target: https://coveralls.io/r/bioidiap/bob.ip.gabor
.. image:: http://img.shields.io/github/tag/bioidiap/bob.ip.gabor.png
   :target: https://github.com/bioidiap/bob.ip.gabor
.. image:: http://img.shields.io/pypi/v/bob.ip.gabor.png
   :target: https://pypi.python.org/pypi/bob.ip.gabor
.. image:: http://img.shields.io/pypi/dm/bob.ip.gabor.png
   :target: https://pypi.python.org/pypi/bob.ip.gabor

===============================================
 C++ code and Python bindings for bob.ip.gabor
===============================================

This package contains a set of C++ code and their according Python bindings for Bob's Image Processing tools concerning Gabor wavelets and the Gabor wavelet transform.

Installation
------------

Install it through normal means, via PyPI or use ``zc.buildout`` to bootstrap the package and run test units.

Documentation
-------------

The latest version of the documentation can be found `here <https://www.idiap.ch/software/bob/docs/latest/bioidiap/bob.ip.gabor/master/index.html>`_.

Otherwise, you can generate the documentation for this package yourself, after installation, using Sphinx::

  $ sphinx-build -b html doc sphinx

This shall place in the directory ``sphinx``, the current version for the documentation of the package.

Testing
-------

You can run a set of tests using the nose test runner::

  $ nosetests -sv

.. warning::

   If Bob <= 1.2.1 is installed on your python path, nose will automatically load the old version of the insulate plugin available in Bob, which will trigger the loading of incompatible shared libraries (from Bob itself), in to your working binary.
   This will cause a stack corruption.
   Either remove the centrally installed version of Bob, or build your own version of Python in which Bob <= 1.2.1 is not installed.

You can run our documentation tests using sphinx itself::

  $ sphinx-build -b doctest doc sphinx

You can test overall test coverage with::

  $ nosetests --with-coverage --cover-package=bob.ip.gabor

The ``coverage`` egg must be installed for this to work properly.

Development
-----------

To develop this package, install using ``zc.buildout``, using the buildout configuration found on the root of the package::

  $ python bootstrap.py
  ...
  $ ./bin/buildout

Tweak the options in ``buildout.cfg`` to disable/enable verbosity and debug builds.
