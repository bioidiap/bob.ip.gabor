#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Thu 30 Jan 08:45:49 2014 CET

from setuptools import setup, find_packages, dist
dist.Distribution(dict(setup_requires=['bob.blitz', 'bob.io.base']))
from bob.blitz.extension import Extension
import bob.io.base

import os
package_dir = os.path.dirname(os.path.realpath(__file__))
package_dir = os.path.join(package_dir, 'bob', 'ip', 'gabor')
include_dirs = [package_dir, bob.io.base.get_include()]

packages = ['bob-io >= 1.2.2', 'bob-sp >= 1.2.2', 'bob-ip >= 1.2.2', 'boost']
version = '2.0.0a1'

setup(

    name='bob.ip.gabor',
    version=version,
    description='Line and Box drawing utilities',
    url='http://github.com/bioidiap/bob.ip.gabor',
    license='BSD',
    author='Andre Anjos',
    author_email='andre.anjos@idiap.ch',

    long_description=open('README.rst').read(),

    packages=find_packages(),
    include_package_data=True,

    install_requires=[
      'setuptools',
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
        packages = packages,
        include_dirs = include_dirs,
        version = version,
      ),
      Extension("bob.ip.gabor._library",
        [
          "bob/ip/gabor/cpp/Wavelet.cpp",
          "bob/ip/gabor/cpp/Transform.cpp",
          "bob/ip/gabor/cpp/Jet.cpp",
          "bob/ip/gabor/cpp/Graph.cpp",
          "bob/ip/gabor/cpp/Similarity.cpp",

          "bob/ip/gabor/wavelet.cpp",
          "bob/ip/gabor/transform.cpp",
          "bob/ip/gabor/jet.cpp",
          "bob/ip/gabor/graph.cpp",
          "bob/ip/gabor/similarity.cpp",
          "bob/ip/gabor/main.cpp",
        ],
        packages = packages,
        boost_modules = ['python', 'system'],
        include_dirs = include_dirs,
        version = version,
      ),
    ],

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
