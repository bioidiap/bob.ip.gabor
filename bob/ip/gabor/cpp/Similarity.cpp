/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Thu Jun  5 17:47:55 CEST 2014
 *
 * @brief The C++ implementations of the Gabor jet similarities and disparity computation
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include <bob.ip.gabor/Similarity.h>


static const std::map<bob::ip::gabor::Similarity::SimilarityType, std::string> type_map = {
  {bob::ip::gabor::Similarity::SCALAR_PRODUCT, "ScalarProduct"},
  {bob::ip::gabor::Similarity::CANBERRA, "Canberra"},
  {bob::ip::gabor::Similarity::ABS_PHASE, "AbsPhase"},
  {bob::ip::gabor::Similarity::DISPARITY, "Disparity"},
  {bob::ip::gabor::Similarity::PHASE_DIFF, "PhaseDiff"},
  {bob::ip::gabor::Similarity::PHASE_DIFF_PLUS_CANBERRA, "PhaseDiffPlusCanberra"}
};

const std::string& bob::ip::gabor::Similarity::type_to_name(bob::ip::gabor::Similarity::SimilarityType type){
  return type_map.find(type)->second;
}

bob::ip::gabor::Similarity::SimilarityType bob::ip::gabor::Similarity::name_to_type(const std::string& type){
  for (auto it = type_map.begin(); it != type_map.end(); ++it)
    if (it->second == type)
      return it->first;
  throw std::runtime_error("The given similarity name '" + type + "' does not name an appropriate similarity function type.");
}

bob::ip::gabor::Similarity::Similarity(SimilarityType type, boost::shared_ptr<Transform> gwt)
:
  m_type(type),
  m_gwt(gwt),
  m_disparity(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN())
{
  // initialize, when required
  if (m_type >= DISPARITY){
    if (!m_gwt)
      throw std::runtime_error("The given similarity function type '" + type_to_name(m_type) + "' required to specify the Gabor wavelet transform!");
    init();
  }
}

bob::ip::gabor::Similarity::Similarity(bob::io::base::HDF5File& file)
{
  // load configuration from file
  load(file);
}

static double sqr(double x){return x*x;}

void bob::ip::gabor::Similarity::init(){
  m_confidences.resize(m_gwt->numberOfWavelets());
  m_confidences = 0.;
  m_phase_differences.resize(m_gwt->numberOfWavelets());
  m_phase_differences = 0.;
}

double bob::ip::gabor::Similarity::similarity(const Jet& jet1, const Jet& jet2) const{
  // compute the disparity, if required
  if (m_type < DISPARITY){
    switch (m_type){
      case SCALAR_PRODUCT:
        // normalized scalar product (we assume normalized Gabor jets here!)
        return blitz::dot(jet1.abs(), jet2.abs());
      case CANBERRA:{
        // Canberra similarity
        double sim = 0.;
        const auto& a1 = jet1.abs(),& a2 = jet2.abs();
        int size = jet1.length();
        for (int j = 0; j < size; ++j){
          sim += 1. - std::abs(a1(j) - a2(j)) / (a1(j) + a2(j));
        }
        return sim / size;
      }
      case ABS_PHASE:{
        // similarity with absloute values and cosine of phase differences
        double sim = 0.;
        const auto& a1 = jet1.abs(),& a2 = jet2.abs();
        const auto& p1 = jet1.phase(),& p2 = jet2.phase();
        int size = jet1.length();
        for (int j = 0; j < size; ++j){
          sim += a1(j) * a2(j) * cos(p1(j) - p2(j));
        }
        return sim;
      }
      default:
        throw std::runtime_error("This should not have happened. Please assure that newly generated Gabor jet similarity functions are actually implemented!");
    }

  } else {
    // here only the disparity-related functions should be computed
    // compute disparity
    disparity(jet1, jet2);

    const std::vector<blitz::TinyVector<double,2> >& kernels = m_gwt->waveletFrequencies();

    switch (m_type){
      case DISPARITY:{
        // compute the similarity using the estimated disparity
        double sum = 0.;
        for (int j = 0; j < m_confidences.extent(0); ++j){
          sum += m_confidences(j) * cos(m_phase_differences(j) - m_disparity[0] * kernels[j][0] - m_disparity[1] * kernels[j][1]);
        }
        return sum;
      } // DISPARITY

      case PHASE_DIFF:{
        // compute the similarity using the estimated disparity
        double sum = 0.;
        for (int j = 0; j < m_phase_differences.extent(0); ++j){
          sum += cos(m_phase_differences(j) - m_disparity[0] * kernels[j][0] - m_disparity[1] * kernels[j][1]);
        }
        return sum / jet1.length();
      } // PHASE_DIFF

      case PHASE_DIFF_PLUS_CANBERRA:{
        // compute the similarity using the estimated disparity
        double sum = 0.;
        const auto& a1 = jet1.abs(),& a2 = jet2.abs();
        for (int j = 0; j < m_phase_differences.extent(0); ++j){
          // add disparity term
          sum += cos(m_phase_differences(j) - m_disparity[0] * kernels[j][0] - m_disparity[1] * kernels[j][1]);
          // add Canberra term
          sum += 1. - std::abs(a1(j) - a2(j)) / (a1(j) + a2(j));
        }
        return sum / (2. * jet1.length());
      }

      default:
        // this should never happen
        throw std::runtime_error("This should not have happened. Please check the implementation of the similarity() functions.");
    }
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////  Disparity estimation  /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
blitz::TinyVector<double,2> bob::ip::gabor::Similarity::disparity(const Jet& jet1, const Jet& jet2) const{

  // Here, only the disparity based similarity functions are executed
  bob::core::array::assertCZeroBaseContiguous(jet1.jet());
  bob::core::array::assertCZeroBaseContiguous(jet2.jet());
  bob::core::array::assertSameShape(jet1.jet(),jet2.jet());

  // compute confidence vectors
  compute_confidences(jet1, jet2);

  // now, compute the disparity
  compute_disparity();

  // return the disparity
  return m_disparity;
}

static double adjustPhase(double phase){
  return phase - (2.*M_PI)*round(phase / (2.*M_PI));
}

void bob::ip::gabor::Similarity::shift_phase(const Jet& jet, const Jet& reference, Jet& shifted) const{
  bob::core::array::assertSameShape(jet.jet(),reference.jet());
  bob::core::array::assertSameShape(jet.jet(),shifted.jet());

  // compute disparity between jet and reference jet
  disparity(jet, reference);

  // compute phase shift for each jet entry based on disparity vector
  const std::vector<blitz::TinyVector<double,2>>& kernels = m_gwt->waveletFrequencies();
  auto& data = shifted.jet();
  // copy data from original jet
  data = jet.jet();
  // shift phases according to the computed disparity
  for (int j = 0; j < m_phase_differences.extent(0); ++j){
    data(1,j) = adjustPhase(data(1,j) - m_disparity[0] * kernels[j][0] - m_disparity[1] * kernels[j][1]);
  }
}

void bob::ip::gabor::Similarity::compute_confidences(const Jet& jet1, const Jet& jet2) const{
  if (m_type < DISPARITY){
    throw std::runtime_error("The disparity computation is not supported for similarity type " + type());
  }
  if (jet1.length() != m_confidences.extent(0)){
    throw std::runtime_error((boost::format("The size of the Gabor jet (%d) and the number of wavelets in the Gabor wavelet transform (%d) differ!") % jet1.length() % m_confidences.extent(0)).str());
  }
  // first, fill confidence and phase difference vectors
  const auto& a1 = jet1.abs(),& a2 = jet2.abs(),& p1 = jet1.phase(),& p2 = jet2.phase();
  for (int j = 0; j < m_confidences.extent(0); ++j){
    m_confidences(j) = a1(j) * a2(j);
    m_phase_differences(j) = adjustPhase(p1(j) - p2(j));
  }
}

void bob::ip::gabor::Similarity::compute_disparity() const{
  // approximate the disparity from the phase differences
  double gamma_x_x = 0., gamma_x_y = 0., gamma_y_y = 0., phi_x = 0., phi_y = 0.;
  // initialize the disparity with 0
  m_disparity = 0.;

  const std::vector<blitz::TinyVector<double,2> >& kernels = m_gwt->waveletFrequencies();
  // iterate backwards through the vector to start with the lowest frequency wavelets
  for (int j = m_confidences.extent(0)-1, level = m_gwt->numberOfScales()-1; level >= 0; --level){
    for (int direction = m_gwt->numberOfDirections()-1; direction >= 0; --direction, --j){
      double
          kjx = kernels[j][1],
          kjy = kernels[j][0],
          conf = m_confidences(j),
          diff = m_phase_differences(j);

      // totalize gamma matrix
      gamma_x_x += kjx * kjx * conf;
      gamma_x_y += kjx * kjy * conf;
      gamma_y_y += kjy * kjy * conf;

      // totalize phi vector
      // estimate the number of cycles that we are off
      double nL = round((diff - m_disparity[1] * kjx - m_disparity[0] * kjy) / (2.*M_PI));
      // totalize corrected phi vector elements
      phi_x += (diff - nL * 2. * M_PI) * conf * kjx;
      phi_y += (diff - nL * 2. * M_PI) * conf * kjy;
    } // for direction

    // re-calculate disparity as d=\Gamma^{-1}\Phi of the (low frequency) wavelet scales that we used up to now
    double gamma_det = gamma_x_x * gamma_y_y - sqr(gamma_x_y);
    m_disparity[1] = (gamma_y_y * phi_x - gamma_x_y * phi_y) / gamma_det;
    m_disparity[0] = (gamma_x_x * phi_y - gamma_x_y * phi_x) / gamma_det;
  } // for level
}


void bob::ip::gabor::Similarity::save(bob::io::base::HDF5File& file) const{

  file.set("Type", type_to_name(m_type));
  if (m_type >= DISPARITY){
    file.createGroup("Transform");
    file.cd("Transform");
    m_gwt->save(file);
    file.cd("..");
  }
}


void bob::ip::gabor::Similarity::load(bob::io::base::HDF5File& file){
  // read value
  m_type = name_to_type(file.read<std::string>("Type"));

  if (m_type >= DISPARITY){
    file.cd("Transform");
    m_gwt.reset(new Transform(file));
    file.cd("..");

    init();
  }
}

