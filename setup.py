#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Manuel Guenther <manuel.guenther@idiap.ch>
# Tue Jun 24 09:32:21 CEST 2014

from setuptools import setup, find_packages, dist
dist.Distribution(dict(setup_requires=['bob.blitz', 'bob.core', 'bob.io.base', 'bob.sp']))
from bob.blitz.extension import Extension, Library, build_ext

import os
package_dir = os.path.dirname(os.path.realpath(__file__))
target_dir = os.path.join(package_dir, 'bob', 'ip', 'gabor')

version = '2.0.0a1'

setup(

    name='bob.ip.gabor',
    version=version,
    description='C++ code and Python bindings for Bob\'s Gabor wavelet analysis tools',
    url='http://github.com/bioidiap/bob.ip.gabor',
    license='BSD',
    author='Manuel Guenther',
    author_email='manuel.guenther@idiap.ch',

    long_description=open('README.rst').read(),

    packages=find_packages(),
    include_package_data=True,

    install_requires=[
      'setuptools',
      'matplotlib',
      'bob.blitz',
      'bob.io.base',
      'bob.io.image',
      'bob.ip.color',
      'bob.sp'
    ],

    namespace_packages=[
      "bob",
      "bob.ip",
    ],

    ext_modules = [
      Extension("bob.ip.gabor.version",
        [
          "bob/ip/gabor/version.cpp",
        ],
        version = version,
        bob_packages = ['bob.core', 'bob.io.base', 'bob.sp'],
      ),

      Library("bob_ip_gabor",
        [
          "bob/ip/gabor/cpp/Wavelet.cpp",
          "bob/ip/gabor/cpp/Transform.cpp",
          "bob/ip/gabor/cpp/Jet.cpp",
          "bob/ip/gabor/cpp/Graph.cpp",
          "bob/ip/gabor/cpp/Similarity.cpp",
        ],
        package_directory = package_dir,
        target_directory = target_dir,
        version = version,
        bob_packages = ['bob.core', 'bob.io.base', 'bob.sp'],
      ),

      Extension("bob.ip.gabor._library",
        [
          "bob/ip/gabor/wavelet.cpp",
          "bob/ip/gabor/transform.cpp",
          "bob/ip/gabor/jet.cpp",
          "bob/ip/gabor/graph.cpp",
          "bob/ip/gabor/similarity.cpp",
          "bob/ip/gabor/main.cpp",
        ],
        bob_packages = ['bob.core', 'bob.io.base', 'bob.sp'],
        version = version,
        libraries = ['bob_ip_gabor'],
      ),
    ],

    cmdclass = {
      'build_ext': build_ext
    },

    classifiers = [
      'Development Status :: 3 - Alpha',
      'Intended Audience :: Developers',
      'License :: OSI Approved :: BSD License',
      'Natural Language :: English',
      'Programming Language :: Python',
      'Programming Language :: Python :: 3',
      'Topic :: Software Development :: Libraries :: Python Modules',
      ],

)
