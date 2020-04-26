#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Tiago de Freitas Pereira <tiago.pereira@idiap.ch>

import bob.ip.gabor
import numpy
import pickle

def test_jet():    
    jet = bob.ip.gabor.Jet(5)
    jet.jet = numpy.arange(10).reshape(2,5).astype("float")
    jet_after_pickle = pickle.loads(pickle.dumps(jet))
    
    assert numpy.allclose(jet.abs, jet_after_pickle.abs, 10e-3)
    assert numpy.allclose(jet.complex, jet_after_pickle.complex, 10e-3)
    assert numpy.allclose(jet.jet, jet_after_pickle.jet, 10e-3)    
