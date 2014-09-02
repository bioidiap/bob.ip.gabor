/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 18:15:00 CEST 2014
 *
 * @brief Header file for the C++ implementations of the Gabor jet
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */


#include <bob.ip.gabor/Jet.h>

#include <numeric>

bob::ip::gabor::Jet::Jet(
  int length
):
  m_jet(2, length)
{
  m_jet = 0.;
}

bob::ip::gabor::Jet::Jet(
  const blitz::Array<std::complex<double>,3>& trafo_image,
  const blitz::TinyVector<int,2>& position,
  bool normalize
):
  m_jet(2, trafo_image.extent(0))
{
  if (position[0] < 0 || position[0] >= trafo_image.extent(1) ||
      position[1] < 0 || position[1] >= trafo_image.extent(2)
  ){
    throw std::runtime_error((boost::format("Jet: position (%d, %d) to extract Gabor jet out of range [0, %d[, [0, %d[") % position[0] % position[1] % trafo_image.extent(1) % trafo_image.extent(2)).str());
  }

  blitz::Array<std::complex<double>,1> data = trafo_image(blitz::Range::all(), position[0], position[1]);
  m_jet(0, blitz::Range::all()) = blitz::abs(data);
  m_jet(1, blitz::Range::all()) = blitz::arg(data);

  if (normalize)
    this->normalize();
}

bob::ip::gabor::Jet::Jet(
  const blitz::Array<std::complex<double>,1>& data,
  bool normalize
):
  m_jet(2, data.extent(0))
{
  m_jet(0, blitz::Range::all()) = blitz::abs(data);
  m_jet(1, blitz::Range::all()) = blitz::arg(data);

  if (normalize)
    this->normalize();
}


bob::ip::gabor::Jet::Jet(
  const std::vector<boost::shared_ptr<bob::ip::gabor::Jet>>& jets,
  bool normalize
)
{
  average(jets, normalize);
}


bob::ip::gabor::Jet::Jet(
  bob::io::base::HDF5File& f
)
{
  load(f);
}


bob::ip::gabor::Jet::Jet(
  const Jet& other
):
  m_jet(other.m_jet.shape())
{
  m_jet = other.m_jet;
}

bob::ip::gabor::Jet& bob::ip::gabor::Jet::operator = (
  const Jet& other
){
  m_jet.resize(other.m_jet.shape());
  m_jet = other.m_jet;
  return *this;
}

void bob::ip::gabor::Jet::init(
  const blitz::Array<std::complex<double>,1>& data,
  bool normalize
){
  m_jet.resize(2, data.extent(0));
  m_jet(0, blitz::Range::all()) = blitz::abs(data);
  m_jet(1, blitz::Range::all()) = blitz::arg(data);

  if (normalize)
    this->normalize();
}

void bob::ip::gabor::Jet::extract(
  const blitz::Array<std::complex<double>,3>& trafo_image,
  const blitz::TinyVector<int,2>& position,
  bool normalize
){
  if (position[0] < 0 || position[0] >= trafo_image.extent(1) ||
      position[1] < 0 || position[1] >= trafo_image.extent(2)
  ){
    throw std::runtime_error((boost::format("Jet: position (%d, %d) to extract Gabor jet out of range [0, %d[, [0, %d[") % position[0] % position[1] % trafo_image.extent(1) % trafo_image.extent(2)).str());
  }

  blitz::Array<std::complex<double>,1> data = trafo_image(blitz::Range::all(), position[0], position[1]);
  init(data, normalize);
}


bool bob::ip::gabor::Jet::operator == (
  const Jet& other
) const {
  return bob::core::array::isClose(m_jet, other.m_jet);
}

const blitz::Array<std::complex<double>,1> bob::ip::gabor::Jet::complex() const {
  return blitz::Array<std::complex<double>,1>(blitz::polar(this->abs(), this->phase()));
}


void bob::ip::gabor::Jet::average(const std::vector<boost::shared_ptr<bob::ip::gabor::Jet>>& jets, bool normalize){
  if (jets.empty()){
    throw std::runtime_error("At least one Gabor jet is required to compute the average from.");
  }
  // initialize with 0
  blitz::Array<std::complex<double>,1> mean(jets[0]->jet().extent(1));
  mean = 0.;

  for (auto it = jets.begin(); it != jets.end(); ++it){
    mean += (*it)->complex();
  }
  mean /= (double)jets.size();

  // set the complex values, and normalize if wanted
  init(mean, normalize);
}

void bob::ip::gabor::Jet::save(bob::io::base::HDF5File& f) const{
  f.setArray("Jet", m_jet);
}

void bob::ip::gabor::Jet::load(bob::io::base::HDF5File& f){
  m_jet.reference(f.readArray<double,2>("Jet"));
}

double bob::ip::gabor::Jet::normalize(){
  blitz::Array<double,1> abs_jet = this->abs();
  double norm = std::inner_product(abs_jet.begin(), abs_jet.end(), abs_jet.begin(), 0.);
  // normalize the absolute parts of the jets
  if (std::abs(norm - 1.) > 1e-8)
    abs_jet /= sqrt(norm);
  return norm;
}

