/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief C++ implementations of the Gabor wavelet transform
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include <bob.ip.gabor/Transform.h>

/**
 * Initializes a discrete family of Gabor wavelets
 * @param number_of_scales     The number of scales (frequencies) to generate
 * @param number_of_directions The number of directions (orientations) to generate
 * @param sigma                The width (standard deviation) of the Gabor wavelet
 * @param k_max                The highest frequency to generate (maximum: PI)
 * @param k_fac                The logarithmical factor between two scales of Gabor wavelets; should be below one
 * @param pow_of_k             The power of k for the prefactor
 * @param dc_free              Make the Gabor wavelet DC-free?
 */
bob::ip::gabor::Transform::Transform(
  int number_of_scales,
  int number_of_directions,
  double sigma,
  double k_max,
  double k_fac,
  double pow_of_k,
  bool dc_free,
  double epsilon
)
: m_sigma(sigma),
  m_pow_of_k(pow_of_k),
  m_k_max(k_max),
  m_k_fac(k_fac),
  m_dc_free(dc_free),
  m_wavelets(),
  m_wavelet_frequencies(),
  m_fft(),
  m_ifft(),
  m_number_of_scales(number_of_scales),
  m_number_of_directions(number_of_directions),
  m_epsilon(epsilon)
{
  computeWaveletFrequencies();
}


bob::ip::gabor::Transform::Transform(
  const bob::ip::gabor::Transform & other
)
: m_sigma(other.m_sigma),
  m_pow_of_k(other.m_pow_of_k),
  m_k_max(other.m_k_max),
  m_k_fac(other.m_k_fac),
  m_dc_free(other.m_dc_free),
  m_wavelets(),
  m_wavelet_frequencies(),
  m_fft(),
  m_ifft(),
  m_number_of_scales(other.m_number_of_scales),
  m_number_of_directions(other.m_number_of_directions),
  m_epsilon(other.m_epsilon)
{
  computeWaveletFrequencies();
}

bob::ip::gabor::Transform::Transform(
  bob::io::base::HDF5File& file
)
{
  load(file);
}


bob::ip::gabor::Transform&
bob::ip::gabor::Transform::operator =
(
  const bob::ip::gabor::Transform & other
)
{
  m_sigma = other.m_sigma;
  m_pow_of_k = other.m_pow_of_k;
  m_k_max = other.m_k_max;
  m_k_fac = other.m_k_fac;
  m_dc_free = other.m_dc_free;
  m_fft = bob::sp::FFT2D();
  m_ifft = bob::sp::IFFT2D();
  m_number_of_scales = other.m_number_of_scales;
  m_number_of_directions = other.m_number_of_directions;
  m_epsilon = other.m_epsilon;

  computeWaveletFrequencies();

  return *this;
}

bool
bob::ip::gabor::Transform::operator ==
(
  const bob::ip::gabor::Transform & other
) const
{
#define aeq(x1,x2) std::abs(x1-x2) < 1e-8

  return aeq(m_sigma, other.m_sigma) &&
         aeq(m_pow_of_k,other.m_pow_of_k) &&
         aeq(m_k_max, other.m_k_max) &&
         aeq(m_k_fac, other.m_k_fac) &&
         m_dc_free == other.m_dc_free &&
         m_number_of_scales == other.m_number_of_scales &&
         m_number_of_directions == other.m_number_of_directions &&
         aeq(m_epsilon, other.m_epsilon);

#undef aeq
}


/**
 * Private function that computes the frequency vectors of the Gabor wavelets
 */
void bob::ip::gabor::Transform::computeWaveletFrequencies(){
  // reserve enough space
  m_wavelet_frequencies.clear();
  m_wavelet_frequencies.reserve(m_number_of_scales * m_number_of_directions);
  // initialize highest frequency
  double k_abs = m_k_max;
  // iterate over the scales
  for (int s = 0; s < m_number_of_scales; ++s){

    // iterate over the directions
    for (int d = 0; d < m_number_of_directions; ++d )
    {
      double angle = M_PI * d / m_number_of_directions;
      // compute center of kernel in frequency domain in Cartesian coordinates
      m_wavelet_frequencies.push_back(
        blitz::TinyVector<double,2>(k_abs * sin(angle), k_abs * cos(angle)));
    } // for d

    // move to the next frequency scale
    k_abs *= m_k_fac;
  } // for s
}

/**
 * Generates the wavelets for the given image resolution.
 * This function dose not need to be called explicitly to be able to perform the GWT.
 * @param resolution  The resolution of the image to generate the kernels for
 */
void bob::ip::gabor::Transform::generateWavelets(
  int height,
  int width
)
{
  blitz::TinyVector<int,2> resolution(height, width);
  if (height != (int)m_fft.getHeight() || width != (int)m_fft.getWidth() ){
    // new kernels need to be generated
    m_wavelets.resize(m_wavelet_frequencies.size());

    for (int j = 0; j < (int)m_wavelet_frequencies.size(); ++j){
      m_wavelets[j].reset(new bob::ip::gabor::Wavelet(resolution, m_wavelet_frequencies[j], m_sigma, m_pow_of_k, m_dc_free, m_epsilon));
    }

    // reset fft sizes
    m_fft.setShape(height, width);
    m_ifft.setShape(height, width);
    m_temp_array.resize(blitz::shape(height,width));
    m_temp_array2.resize(m_temp_array.shape());
    m_frequency_image.resize(m_temp_array.shape());
  }
}

/**
 * Computes the Gabor wavelet transformation for the given image (in spatial domain)
 * @param gray_image  The source image in spatial domain
 * @param trafo_image The convolution result, in spatial domain
 */
void bob::ip::gabor::Transform::transform_inner(
  const blitz::Array<std::complex<double>,2>& gray_image,
  blitz::Array<std::complex<double>,3>& trafo_image
)
{
  // check that the shape is correct
  bob::core::array::assertSameShape(trafo_image, blitz::shape(m_wavelet_frequencies.size(), gray_image.extent(0), gray_image.extent(1)));

  // first, check if we need to reset the kernels
  generateWavelets(gray_image.extent(0), gray_image.extent(1));

  // perform Fourier transformation to image
  m_fft(gray_image, m_frequency_image);

  // now, let each kernel compute the transformation result
  for (int j = 0; j < (int)m_wavelets.size(); ++j){
    // compute Gabor wavelet transform in frequency domain
    m_wavelets[j]->transform(m_frequency_image, m_temp_array);
    // get a reference to the current layer of the trafo image
    blitz::Array<std::complex<double>,2> layer(trafo_image(j, blitz::Range::all(), blitz::Range::all()));
    // perform ifft on the trafo image layer
    m_ifft(m_temp_array, layer);
  } // for j
}


void bob::ip::gabor::Transform::save(bob::io::base::HDF5File& file) const{
  file.set("Sigma", m_sigma);
  file.set("PowOfK", m_pow_of_k);
  file.set("KMax", m_k_max);
  file.set("KFac", m_k_fac);
  file.set("DCfree", m_dc_free);
  file.set("NumberOfScales", m_number_of_scales);
  file.set("NumberOfDirections", m_number_of_directions);
  file.set("Epsilon", m_epsilon);
}

void bob::ip::gabor::Transform::load(bob::io::base::HDF5File& file){
  m_sigma = file.read<double>("Sigma");
  m_pow_of_k = file.read<double>("PowOfK");
  m_k_max = file.read<double>("KMax");
  m_k_fac = file.read<double>("KFac");
  m_dc_free = file.read<bool>("DCfree");
  m_number_of_scales = file.read<int>("NumberOfScales");
  m_number_of_directions = file.read<int>("NumberOfDirections");
  m_epsilon = file.read<double>("Epsilon");

  computeWaveletFrequencies();
}

