from ._library import Jet
import bob.io.base
import numpy

def _name(i):
  return "Jet_%03d"%(i+1)

def save_jets(jets, hdf5):
  """save_jets(jets, hdf5) -> None

  Saves the given list of Gabor jets to the given HDF5 file, which needs to be open for writing.

  **Parameters**:

    ``hdf5`` : :py:class:`bob.io.base.HDF5File`
      An HDF5 file open for writing

    ``jets`` : [:py:class:`bob.ip.gabor.Jet`]
      The list of Gabor jets to write to file
  """
  hdf5.set("NumberOfJets", len(jets))
  for i in range(len(jets)):
    hdf5.create_group(_name(i))
    hdf5.cd(_name(i))
    jets[i].save(hdf5)
    hdf5.cd("..")

def load_jets(hdf5):
  """load_jets(hdf5) -> jets

  Loads a list of Gabor jets from the given HDF5 file, which needs to be open for reading.

  **Parameters**:

    ``hdf5`` : :py:class:`bob.io.base.HDF5File`
      An HDF5 file open for reading

  **Returns**:

    ``jets`` : [:py:class:`bob.ip.gabor.Jet`]
      The list of Gabor jets read from file
  """
  count = hdf5.read("NumberOfJets")
  jets = []
  for i in range(count):
    hdf5.cd(_name(i))
    jets.append(Jet(hdf5))
    hdf5.cd("..")
  return jets

