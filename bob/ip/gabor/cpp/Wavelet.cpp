/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief C++ implementations of the Gabor wavelet transform
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include <bob.ip.gabor/Wavelet.h>

static inline double sqr(double x){return x*x;}

/**
 * Generates a Gabor wavelet.
 * @param resolution The resolution of the image to generate
 * @param k  The frequency vector (i.e. the center of the Gaussian in frequency domain)
 * @param sigma  The standard deviation (i.e. the width of the Gabor wavelet)
 * @param pow_of_k  The power of \f$ k^x \f$ used as a prefactor of the Gabor wavelet
 * @param dc_free   Make the Gabor wavelet DC-free?
 * @param epsilon   The epsilon value below which the wavelet value is considered as zero
 */
bob::ip::gabor::Wavelet::Wavelet(
  const blitz::TinyVector<int,2>& resolution,
  const blitz::TinyVector<double,2>& k,
  const double sigma,
  const double pow_of_k,
  const bool dc_free,
  const double epsilon
)
: m_y_resolution(resolution[0]),
  m_x_resolution(resolution[1])
{
  // check that the parametrization makes sense
  if (m_y_resolution <= 0 || m_y_resolution <= 0 || sigma <= 0){
    throw std::runtime_error("The parametrization of the Gabor wavelet does not make any sense.");
  }

  // create Gabor wavelet with given parameters
  int start_x = - m_x_resolution / 2, start_y = - m_y_resolution / 2;
  // take care of odd resolutions in the end points
  int end_x = m_x_resolution / 2 + m_x_resolution % 2, end_y = m_y_resolution / 2 + m_y_resolution % 2;

  double k_x_factor = 2. * M_PI / m_x_resolution, k_y_factor = 2. * M_PI / m_y_resolution;
  double kx = k[1], ky = k[0];

  // iterate over all pixels of the images
  for (int y = start_y; y < end_y; ++y){

    // convert relative pixel coordinate into frequency coordinate
    double omega_y = y * k_y_factor;

    for (int x = start_x; x < end_x; ++x){

      // convert relative pixel coordinate into frequency coordinate
      double omega_x = x * k_x_factor;

      // compute value of frequency wavelet function
      double omega_minus_k_squared = sqr(omega_x - kx) + sqr(omega_y - ky);
      double sigma_square = sqr(sigma);
      double k_square = sqr(kx) + sqr(ky);
      // assign wavelet value
      double wavelet_value = exp(- sigma_square * omega_minus_k_squared / (2. * k_square));

      if (dc_free){
        double omega_square = sqr(omega_x) + sqr(omega_y);

        wavelet_value -= exp(-sigma_square * (omega_square + k_square) / (2. * k_square));
      } // if ! dc_free

      // prefactor the wavelet value with k^(pow_of_k); the default prefactor 1 might not be the best.
      wavelet_value *= std::pow(k_square, pow_of_k / 2.);

      if (std::abs(wavelet_value) > epsilon){
        m_wavelet_pixel.push_back(std::make_pair(
          blitz::TinyVector<int,2>((y + m_y_resolution) % m_y_resolution, (x + m_x_resolution) % m_x_resolution),
          wavelet_value
        ));
      }
    } // for x
  } // for y
}

bob::ip::gabor::Wavelet::Wavelet(
  const bob::ip::gabor::Wavelet& other
)
: m_wavelet_pixel(other.m_wavelet_pixel.size()),
  m_y_resolution(other.m_y_resolution),
  m_x_resolution(other.m_x_resolution)
{
  std::copy(other.m_wavelet_pixel.begin(), other.m_wavelet_pixel.end(), m_wavelet_pixel.begin());
}

bob::ip::gabor::Wavelet&
bob::ip::gabor::Wavelet::operator =
(
  const bob::ip::gabor::Wavelet& other
)
{
  const_cast<int&>(m_y_resolution) = other.m_y_resolution;
  const_cast<int&>(m_x_resolution) = other.m_x_resolution;
  m_wavelet_pixel.resize(other.m_wavelet_pixel.size());
  std::copy(other.m_wavelet_pixel.begin(), other.m_wavelet_pixel.end(), m_wavelet_pixel.begin());
  return *this;
}

bool
bob::ip::gabor::Wavelet::operator ==
(
  const bob::ip::gabor::Wavelet& other
) const
{
  if (m_x_resolution != other.m_x_resolution || m_y_resolution != other.m_y_resolution)
    return false;
  if (m_wavelet_pixel.size() != other.m_wavelet_pixel.size())
    return false;

  auto it1 = m_wavelet_pixel.begin(), it2 = other.m_wavelet_pixel.begin(), it1end = m_wavelet_pixel.end();
  for (; it1 != it1end; ++it1, ++it2)
    if (it1->first[0] != it2->first[0] || it1->first[1] != it2->first[1] || std::abs(it1->second - it2->second) > 1e-8)
      return false;

  // identical.
  return true;
}

/**
 * Performs the convolution of the given image with this Gabor wavelet.
 * Please note that both the inpus as well as the output image are in frequency domain.
 * @param frequency_domain_image
 * @param transformed_frequency_domain_image
 */
void bob::ip::gabor::Wavelet::transform(
  const blitz::Array<std::complex<double>,2>& frequency_domain_image,
  blitz::Array<std::complex<double>,2>& transformed_frequency_domain_image
) const
{
  // assert same size
  bob::core::array::assertSameShape(frequency_domain_image, transformed_frequency_domain_image);
  // clear resulting image first
  transformed_frequency_domain_image = std::complex<double>(0);
  // iterate through the wavelet pixels and do the multiplication
  auto it = m_wavelet_pixel.begin(), it_end = m_wavelet_pixel.end();
  for (; it < it_end; ++it){
    transformed_frequency_domain_image(it->first) = frequency_domain_image(it->first) * it->second;
  }
}

/**
 * Generates and returns the image for the current wavelet.
 * @return The wavelet image in frequency domain.
 */
blitz::Array<double,2> bob::ip::gabor::Wavelet::waveletImage() const{
  blitz::Array<double,2> image(m_y_resolution, m_x_resolution);
  image = 0;
  // iterate through the wavelet pixels
  auto it = m_wavelet_pixel.begin(), it_end = m_wavelet_pixel.end();
  for (; it < it_end; ++it){
    image(it->first) = it->second;
  }
  return image;
}

