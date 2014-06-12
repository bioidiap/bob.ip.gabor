from ._library import Jet
import bob.io.base
import numpy

def _name(i):
  return "Jet_%03d"%(i+1)

def save_jets(jets, hdf5):
  """Saves the given list of Gabor jets to the given HDF5 file, which needs to be open for writing."""
  hdf5.set("NumberOfJets", len(jets))
  for i in range(len(jets)):
    hdf5.create_group(_name(i))
    hdf5.cd(_name(i))
    jets[i].save(hdf5)
    hdf5.cd("..")

def load_jets(hdf5):
  """Loads the list of Gabor jets from the given HDF5 file."""
  count = hdf5.read("NumberOfJets")
  jets = []
  for i in range(count):
    hdf5.cd(_name(i))
    jets.append(Jet(hdf5))
    hdf5.cd("..")
  return jets

