/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun 11 17:20:43 CEST 2014
 *
 * @brief Header file for the C++ implementations of a Graph of Gabor jets
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */


#ifndef BOB_IP_GABOR_GRAPH_H
#define BOB_IP_GABOR_GRAPH_H

#include <bob.io.base/HDF5File.h>
#include <bob.sp/FFT2D.h>
#include <bob.core/cast.h>

#include <bob.ip.gabor/Jet.h>


namespace bob {

  namespace ip {

    namespace gabor{

      //! An interface for handling graphs of Gabor jets
      class Graph{

        public:

          //! creates a face grid graph using two reference positions, namely, the eyes
          Graph(
            blitz::TinyVector<int,2> righteye,
            blitz::TinyVector<int,2> lefteye,
            int between,
            int along,
            int above,
            int below
          );

          //! creates a regular grid graph with specified first and last position, and the step size between two nodes
          Graph(
            blitz::TinyVector<int,2> first,
            blitz::TinyVector<int,2> last,
            blitz::TinyVector<int,2> step
          );

          //! creates a graph with the specified node positions
          Graph(
            const std::vector<blitz::TinyVector<int,2>>& nodes
          );

          //! creates a graph from file
          Graph(
            bob::io::base::HDF5File& hdf5
          );

          //! Copy constructor
          Graph(const Graph& other);

          //! Assignment operator
          Graph& operator =(const Graph& other);

          //! Equality operator
          bool operator ==(const Graph& other) const;

          //! returns the number of nodes of this graph
          int numberOfNodes() const {return m_nodes.size();}

          //! sets the node positions
          void nodes(const std::vector<blitz::TinyVector<int,2>>& nodes) {m_nodes = nodes;}

          //! Returns the generated node positions (in the usual order (y,x))
          const std::vector<blitz::TinyVector<int,2>>& nodes() const {return m_nodes;}

          //! extracts the Gabor jets of the graph from the jet image
          //! the vector must have the same size as the numberOfNodes()
          void extract(
            const blitz::Array<std::complex<double>,3> trafo_image,
            std::vector<boost::shared_ptr<Jet>>& jets,
            bool normalize = true
          ) const;

          //! saves this graph to file
          void save(bob::io::base::HDF5File& file) const;

          //! loads this graph from file
          void load(bob::io::base::HDF5File& file);

        private:
          void checkNodes(int height, int width) const;

          // The node positions of the graph
          std::vector<blitz::TinyVector<int,2>> m_nodes;

      }; // class Graph
    } // namespace gabor

  } // namespace ip

} // namespace bob


#endif // BOB_IP_GABOR_GRAPH_H
