/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief Header file for the C++ implementations of the Gabor wavelet transform
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */


#ifndef BOB_IP_GABOR_TRANSFORM_H
#define BOB_IP_GABOR_TRANSFORM_H

#include <bob.io.base/HDF5File.h>
#include <bob.sp/FFT2D.h>
#include <bob.core/cast.h>

#include <bob.ip.gabor/Wavelet.h>


namespace bob {

  namespace ip {

    namespace gabor{


      //! \brief The Transform class computes a Gabor wavelet transform of the given image.
      //! It computes either the complete Gabor wavelet transformed image (short: trafo image) with
      //! number_of_scales * number_of_orientations layers, or a Gabor jet image that includes
      //! one Gabor jet (with one vector of absolute values and one vector of phases) for each pixel
      class Transform {

        public:

          //! \brief Constructs a Gabor wavelet transform object.
          //! This class will generate number_of_scales * number_of_orientations Gabor wavelets
          //! using the given sigma, k_max and k_fac values
          //! All parameters have reasonable defaults, as used by default algorithms
          Transform(
            int number_of_scales = 5,
            int number_of_directions = 8,
            double sigma = 2. * M_PI,
            double k_max = M_PI / 2.,
            double k_fac = 1./sqrt(2.),
            double pow_of_k = 0.,
            bool dc_free = true,
            double epsilon = 1e-10
          );

          //! Copy constructor
          Transform(const Transform& other);

          //! Constructor from HDF5File
          Transform(bob::io::base::HDF5File& file);

          //! Assignment operator
          Transform& operator=(const Transform& other);
          //! Equality operator
          bool operator==(const Transform& other) const;

          //! generate the wavelets for the new resolution
          void generateWavelets(int y_resoultion, int x_resolution);

          //! Returns the Gabor wavelet for the given index
          const std::vector<boost::shared_ptr<bob::ip::gabor::Wavelet>>& wavelets() const {return m_wavelets;}

          //! get the number of wavelets (usually, 40) used by this GWT class
          int numberOfWavelets() const{return m_wavelet_frequencies.size();}
          int numberOfDirections() const{return m_number_of_directions;}
          int numberOfScales() const{return m_number_of_scales;}

          //! Returns the vector of central frequencies used by this Gabor wavelet family
          const std::vector<blitz::TinyVector<double,2> >& waveletFrequencies() const {return m_wavelet_frequencies;}

          double sigma() const {return m_sigma;}
          double k_max() const {return m_k_max;}
          double k_fac() const {return m_k_fac;}
          double pow_of_k() const {return m_pow_of_k;}
          bool dc_free() const {return m_dc_free;}

          template <typename T> void transform(
            const blitz::Array<T,2>& gray_image,
            blitz::Array<std::complex<double>,3>& trafo_image
          ){
            transform_inner(bob::core::array::cast<std::complex<double> >(gray_image), trafo_image);
          }

          //! \brief saves the parameters of this Gabor wavelet family to file
          void save(bob::io::base::HDF5File& file) const;

          //! \brief reads the parameters of this Gabor wavelet family from file
          void load(bob::io::base::HDF5File& file);

        private:

          //! performs Gabor wavelet transform and returns vector of complex images
          void transform_inner(
            const blitz::Array<std::complex<double>,2>& gray_image,
            blitz::Array<std::complex<double>,3>& trafo_image
          );

          void computeWaveletFrequencies();

          double m_sigma;
          double m_pow_of_k;
          double m_k_max;
          double m_k_fac;
          bool m_dc_free;

          std::vector<boost::shared_ptr<bob::ip::gabor::Wavelet>> m_wavelets;
          std::vector<blitz::TinyVector<double,2> > m_wavelet_frequencies;

          bob::sp::FFT2D m_fft;
          bob::sp::IFFT2D m_ifft;

          blitz::Array<std::complex<double>,2> m_temp_array, m_temp_array2, m_frequency_image;

          //! The number of scales (levels, frequencies) of this family
          int m_number_of_scales;
          //! The number of directions (orientations) of this family
          int m_number_of_directions;
          //! The lowest absolute value in the wavelet that should be considered as non-zero
          double m_epsilon;
      }; // class Transform

    } // namepsace gabor

  } // namespace ip

} // namespace bob

#endif // BOB_IP_GABOR_TRANSFORM_H
