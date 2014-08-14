/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief Header file for the C++ implementations of the Gabor wavelet
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */


#ifndef BOB_IP_GABOR_WAVELET_H
#define BOB_IP_GABOR_WAVELET_H

#include <bob.core/assert.h>
#include <vector>

namespace bob {

  namespace ip {

    namespace gabor{

      //! \brief This class represents a single Gabor wavelet in frequency domain.
      class Wavelet {

        public:

          //! Generate a Gabor wavelet in frequency domain
          Wavelet(
            const blitz::TinyVector<int,2>& resolution,
            const blitz::TinyVector<double,2>& wavelet_frequency,
            const double sigma = 2. * M_PI,
            const double pow_of_k = 0.,
            const bool dc_free = true,
            const double epsilon = 1e-10
          );

          //! Copy constructor
          Wavelet(const Wavelet& other);

          //! Assignment operator
          Wavelet& operator=(const Wavelet& other);
          //! Equality operator
          bool operator==(const Wavelet& other) const;

          //! Get the image represenation of the Gabor wavelet in frequency domain
          blitz::Array<double,2> waveletImage() const;

          //! Gabor transforms the given image
          void transform(
            const blitz::Array<std::complex<double>,2>& frequency_domain_image,
            blitz::Array<std::complex<double>,2>& transformed_frequency_domain_image
          ) const;

        private:
          // the Gabor wavelet, stored as pairs of indices and values
          std::vector<std::pair<blitz::TinyVector<int,2>, double> > m_wavelet_pixel;

        public:
          // the resolution of the current Gabor wavelet
          const int m_y_resolution, m_x_resolution;

      }; // class Wavelet
    } // namespace gabor
  } // namespace ip
} // namespace bob

#endif // BOB_IP_GABOR_WAVELET_H
