/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun 11 17:20:43 CEST 2014
 *
 * @brief The C++ implementations of a Graph of Gabor jets
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include <bob.ip.gabor/Graph.h>

/**
 * Generates grid graphs which will be placed according to the given eye positions
 * @param lefteye  Position of the left eye
 * @param righteye Position of the right eye
 * @param between  Number of nodes to place between the eyes (excluding the eye nodes themselves)
 * @param along    Number of nodes to place left and right of the eye nodes (excluding the eye nodes themselves)
 * @param above    Number of nodes to place above the eyes (excluding the eye nodes themselves)
 * @param below    Number of nodes to place below the eyes (excluding the eye nodes themselves)
 */
bob::ip::gabor::Graph::Graph(
  blitz::TinyVector<int,2> righteye,
  blitz::TinyVector<int,2> lefteye,
  int between,
  int along,
  int above,
  int below
)
{
  // shortcuts for eye positions
  int rex = righteye[1], rey = righteye[0];
  int lex = lefteye[1], ley = lefteye[0];
  // compute grid parameters
  double stepx = double(lex - rex) / double(between+1);
  double stepy = double(ley - rey) / double(between+1);
  double xstart = rex - along*stepx + above*stepy;
  double ystart = rey - along*stepy - above*stepx;
  int xcount = between + 2 * (along+1);
  int ycount = above + below + 1;

  // create grid positions
  m_nodes.resize(xcount*ycount);
  for (int y = 0, i = 0; y < ycount; ++y){
    for (int x = 0; x < xcount; ++x, ++i){
      // y position
      m_nodes[i][0] = round(ystart + y * stepx + x * stepy);
      // x position
      m_nodes[i][1] = round(xstart + x * stepx - y * stepy);
    }
  }
}

/**
 * Generates a grid graph starting at the given first index, ending at (or before) the given last index, and advancing the given step size.
 * @param first  First node to be placed (top-left)
 * @param last   Last node to be placed (bottom-right). Depending on the step size, this node might not be reached.
 * @param step   The step size (in pixel) between two nodes
 */
bob::ip::gabor::Graph::Graph(
  blitz::TinyVector<int,2> first,
  blitz::TinyVector<int,2> last,
  blitz::TinyVector<int,2> step
)
{
  int ycount = (last[0] - first[0]) / step[0] + 1;
  int xcount = (last[1] - first[1]) / step[1] + 1;

  // create grid positions
  m_nodes.resize(xcount*ycount);
  for (int y = 0, i = 0; y < ycount; ++y){
    for (int x = 0; x < xcount; ++x, ++i){
      // y position
      m_nodes[i][0] = first[0] + y * step[0];
      // x position
      m_nodes[i][1] = first[1] + x * step[1];
    }
  }
}

bob::ip::gabor::Graph::Graph(
  const std::vector<blitz::TinyVector<int,2>>& nodes
):m_nodes(nodes)
{
}

bob::ip::gabor::Graph::Graph(
  bob::io::base::HDF5File& hdf5
){
  load(hdf5);
}


/**
 * Generates this machine as a copy of the other one
 *
 * @param other  The machine to copy
 */
bob::ip::gabor::Graph::Graph(
  const Graph& other
)
{
  m_nodes = other.m_nodes;
}


/**
 * Makes this machine a deep copy of the given one
 *
 * @param other  The machine to copy
 * @return  A reference to *this
 */
bob::ip::gabor::Graph& bob::ip::gabor::Graph::operator =(
  const Graph& other
)
{
  m_nodes = other.m_nodes;
  return *this;
}

/**
 * Checks if the parameterization of both machines is identical.
 *
 * @param other  The machine to test for equality to this
 * @return true if the node positions of both machines are identical, otherwise false
 */
bool bob::ip::gabor::Graph::operator ==(
  const Graph& other
) const
{
  if (other.numberOfNodes() != numberOfNodes())
    return false;
  for (auto it1 = m_nodes.begin(), it2 = other.m_nodes.begin(); it1 != m_nodes.end(); ++it1, ++it2){
    if ((*it1)[0] != (*it2)[0] || (*it1)[1] != (*it2)[1])
      return false;
  }
  return true;
}


void bob::ip::gabor::Graph::checkNodes(int height, int width) const{
  for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it){
    if ((*it)[0] < 0 || (*it)[0] >= height ||
        (*it)[1] < 0 || (*it)[1] >= width)
      throw std::runtime_error((boost::format("The position (%i,%i) is out of the image boundaries %i x %i") % (*it)[0] % (*it)[1] % height % width).str());
  }
}

/**
 * Extracts the Gabor jets at the node positions
 * @param jet_image  The Gabor jet image to extract the Gabor jets from
 * @param graph_jets The graph that will be filled
 */
void bob::ip::gabor::Graph::extract(
  const blitz::Array<std::complex<double>,3> trafo_image,
  std::vector<boost::shared_ptr<Jet>>& jets,
  bool normalize
) const {
  // check the positions
  checkNodes(trafo_image.shape()[1], trafo_image.shape()[2]);
  // assure the size of the Jet vector
  jets.resize(numberOfNodes());
  // extract Gabor jets
  auto jit = jets.begin();
  auto nit = m_nodes.begin();
  for (; nit != m_nodes.end(); ++jit, ++nit){
    if (*jit){
      // Gabor jet is existent, avoid re-creation
      (*jit)->extract(trafo_image, *nit, normalize);
    } else {
      // Gabor jet is not existent, create it
      jit->reset(new Jet(trafo_image, *nit, normalize));
    }
  }
}

void bob::ip::gabor::Graph::save(bob::io::base::HDF5File& file) const{
  blitz::Array<int,2> n(m_nodes.size(), 2);
  int i = 0;
  for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it, ++i)
    n(i, blitz::Range::all()) = *it;
  file.setArray("NodePositions", n);
}

void bob::ip::gabor::Graph::load(bob::io::base::HDF5File& file){
  blitz::Array<int,2> n(file.readArray<int,2>("NodePositions"));
  m_nodes.resize(n.extent(0));
  int i = 0;
  for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it, ++i)
    *it = n(i, blitz::Range::all());
}

