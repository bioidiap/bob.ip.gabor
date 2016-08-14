#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Manuel Guenther <manuel.guenther@idiap.ch>
# Tue Jun 24 09:32:21 CEST 2014

bob_packages = ['bob.core', 'bob.io.base', 'bob.sp']

from setuptools import setup, find_packages, dist
dist.Distribution(dict(setup_requires=['bob.extension', 'bob.blitz'] + bob_packages))
from bob.blitz.extension import Extension, Library, build_ext

from bob.extension.utils import load_requirements
build_requires = load_requirements()

# Define package version
version = open("version.txt").read().rstrip()

packages = ['boost']
boost_modules = ['system']

setup(

    name='bob.ip.gabor',
    version=version,
    description="Gabor wavelet analysis tools for Bob",
    url='http://gitlab.idiap.ch/bob/bob.ip.gabor',
    license='BSD',
    author='Manuel Guenther',
    author_email='manuel.guenther@idiap.ch',

    long_description=open('README.rst').read(),

    packages=find_packages(),
    include_package_data=True,
    zip_safe=False,

    setup_requires = build_requires,
    install_requires = build_requires,



    ext_modules = [
      Extension("bob.ip.gabor.version",
        [
          "bob/ip/gabor/version.cpp",
        ],
        version = version,
        bob_packages = bob_packages,
        packages = packages,
        boost_modules = boost_modules,
      ),

      Library("bob.ip.gabor.bob_ip_gabor",
        [
          "bob/ip/gabor/cpp/Wavelet.cpp",
          "bob/ip/gabor/cpp/Transform.cpp",
          "bob/ip/gabor/cpp/Jet.cpp",
          "bob/ip/gabor/cpp/Graph.cpp",
          "bob/ip/gabor/cpp/Similarity.cpp",
          "bob/ip/gabor/cpp/JetStatistics.cpp",
        ],
        version = version,
        bob_packages = bob_packages,
        packages = packages,
        boost_modules = boost_modules,
      ),

      Extension("bob.ip.gabor._library",
        [
          "bob/ip/gabor/wavelet.cpp",
          "bob/ip/gabor/transform.cpp",
          "bob/ip/gabor/jet.cpp",
          "bob/ip/gabor/graph.cpp",
          "bob/ip/gabor/similarity.cpp",
          "bob/ip/gabor/jet_statistics.cpp",
          "bob/ip/gabor/main.cpp",
        ],
        bob_packages = bob_packages,
        version = version,
        packages = packages,
        boost_modules = boost_modules,
      ),
    ],

    cmdclass = {
      'build_ext': build_ext
    },

    classifiers = [
      'Framework :: Bob',
      'Development Status :: 4 - Beta',
      'Intended Audience :: Developers',
      'License :: OSI Approved :: BSD License',
      'Natural Language :: English',
      'Programming Language :: Python',
      'Programming Language :: Python :: 3',
      'Topic :: Software Development :: Libraries :: Python Modules',
      ],

)
