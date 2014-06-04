/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief Header file for the C++ implementations of the Gabor jet
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */


#ifndef BOB_IP_GABOR_JET_H
#define BOB_IP_GABOR_JET_H

#include <bob/io/HDF5File.h>
#include <bob/sp/FFT2D.h>
#include <bob/core/cast.h>

#include "Transform.h"


namespace bob {

  namespace ip {

    namespace gabor{


      //! \brief The Jet class provides an interface for handling Gabor jets.
      //! It extracts Gabor jets from an trafo image which was the result of a Gabor wavelet transform
      class Jet {

        public:

          //! Extracts the Gabor jet from a trafo image at the given position
          Jet(
            const blitz::Array<std::complex<double>,3>& trafo_image,
            const blitz::TinyVector<int,2>& position,
            bool normalize = true
          );

          //! creates a Gabor jet from a vector of complex
          Jet(
            const blitz::Array<std::complex<double>,1>& data,
            bool normalize = true
          );

          //! Copy constructor
          Jet(const Jet& other);

          //! Constructor from HDF5File
          Jet(bob::io::HDF5File& file);

          //! Assignment operator
          Jet& operator=(const Jet& other);

          //! Equality operator
          bool operator==(const Jet& other) const;

          //! Normalizes this Gabor jet to unit Euclidean length
          double normalize();

          //! The vector of absolute values
          const blitz::Array<double,1> abs() const {return m_jet(0, blitz::Range::all());}

          //! The vector of phase values
          const blitz::Array<double,1> phase() const  {return m_jet(1, blitz::Range::all());}

          //! The vector of absolute and phase values
          const blitz::Array<double,2>& jet() const {return m_jet;}

          //! The vector of complex values
          const blitz::Array<std::complex<double>,1> complex() const;

          //! \brief saves the parameters of this Gabor wavelet family to file
          void save(bob::io::HDF5File& file) const;

          //! \brief reads the parameters of this Gabor wavelet family from file
          void load(bob::io::HDF5File& file);

        private:

          // the Gabor jet, stored as absolute values and phases
          blitz::Array<double, 2> m_jet;
      }; // class Transform

    } // namespace gabor

  } // namespace ip

} // namespace bob


#endif // BOB_IP_GABOR_JET_H
