/**
 * @author Manuel Gunther <mgunter@vast.uccs.edu>
 * @date Tue Oct 20 09:17:56 MDT 2015
 *
 * @brief The C++ implementations of a Gabor jet statistics
 *
 */


#include <bob.ip.gabor/JetStatistics.h>

static double sqr(const double x){return x*x;}

bob::ip::gabor::JetStatistics::JetStatistics(const std::vector<boost::shared_ptr<bob::ip::gabor::Jet>>& jets, boost::shared_ptr<bob::ip::gabor::Transform> gwt)
: m_gwt(gwt)
{

  // compute statistics of Gabor jets
  // ... get the average Gabor jet
  bob::ip::gabor::Jet average(jets);

  int jet_length = average.length();

  // ... the phases of the average serve as the mean for the phases
  m_meanPhase.reference(average.phase());

  // ... the average of the absolute values must be comupted separately
  m_meanAbs.resize(jet_length);
  m_meanAbs = 0.;
  for (int j = jet_length; j--;){
    for (int i = jets.size(); i--;){
      m_meanAbs(j) += jets[i]->abs()(j);
    }
    m_meanAbs(j) /= jets.size();
  }

  // ... get variances
  m_varAbs.resize(jet_length);
  m_varAbs = 0.;
  m_varPhase.resize(jet_length);
  m_varPhase = 0.;
  for (int j = jet_length; j--;){
    for (int i = jets.size(); i--;){
      m_varAbs(j) += sqr(jets[i]->abs()(j) - m_meanAbs(j));
      m_varPhase(j) += sqr(adjust_phase(jets[i]->phase()(j) - m_meanPhase(j)));
    }
    m_varAbs(j) /= jets.size() - 1;
    m_varPhase(j) /= jets.size() - 1;
  }
}

bob::ip::gabor::JetStatistics::JetStatistics(bob::io::base::HDF5File& hdf5){
  m_meanAbs.reference(hdf5.readArray<double,1>("MeanAbs"));
  m_varAbs.reference(hdf5.readArray<double,1>("VarAbs"));
  m_meanPhase.reference(hdf5.readArray<double,1>("MeanPhase"));
  m_varPhase.reference(hdf5.readArray<double,1>("VarPhase"));
  if (hdf5.hasGroup("Transform")){
    hdf5.cd("Transform");
    m_gwt.reset(new bob::ip::gabor::Transform(hdf5));
    hdf5.cd("..");
  }
}

bool bob::ip::gabor::JetStatistics::operator == (const JetStatistics& other) const {
  return
    bob::core::array::isClose(m_meanAbs, other.m_meanAbs) &&
    bob::core::array::isClose(m_meanPhase, other.m_meanPhase) &&
    bob::core::array::isClose(m_varAbs, other.m_varAbs) &&
    bob::core::array::isClose(m_varPhase, other.m_varPhase) &&
    ( (!m_gwt && !other.m_gwt) || (m_gwt && other.m_gwt && *m_gwt == *other.m_gwt) );
}



void bob::ip::gabor::JetStatistics::save(bob::io::base::HDF5File& hdf5, bool saveTransform) const{
  hdf5.setArray("MeanAbs", m_meanAbs);
  hdf5.setArray("VarAbs", m_varAbs);
  hdf5.setArray("MeanPhase", m_meanPhase);
  hdf5.setArray("VarPhase", m_varPhase);
  if (saveTransform && m_gwt){
    hdf5.createGroup("Transform");
    hdf5.cd("Transform");
    m_gwt->save(hdf5);
    hdf5.cd("..");
  }
}

blitz::TinyVector<double,2> bob::ip::gabor::JetStatistics::disparity(const boost::shared_ptr<bob::ip::gabor::Jet> jet) const{
  if (!m_gwt) throw std::runtime_error("The Gabor wavelet transform class has not been set jet");
  if (m_gwt->numberOfWavelets() != jet->length())
    throw std::runtime_error((boost::format("The given Gabor jet is of length %d, but the transform has %d wavelets; forgot to set your custom Transform") % jet->length() % m_gwt->numberOfWavelets()).str());

  // compute confidences and phase differences once
  m_confidences.resize(m_meanAbs.shape());
  m_phaseDifferences.resize(m_meanPhase.shape());
  m_confidences = m_meanAbs * jet->abs();
  m_phaseDifferences = m_meanPhase - jet->phase();

  double gamma_y_y = 0., gamma_y_x = 0., gamma_x_x = 0., phi_y = 0., phi_x = 0.;
  blitz::TinyVector<double,2> disparity(0., 0.);
  auto kernels = m_gwt->waveletFrequencies();

  // iterate through the Gabor jet **backwards** (from highest scale to lowest scale)
  for (int j = jet->length()-1, scale = m_gwt->numberOfScales(); scale--;){
    for (int direction = m_gwt->numberOfDirections(); direction--; --j){
      const double kjy = kernels[j][0], kjx = kernels[j][1];
      const double conf = m_confidences(j), diff = m_phaseDifferences(j), var = m_varPhase(j);
      // totalize Gamma matrix
      gamma_y_y += conf * kjy * kjy / var;
      gamma_y_x += conf * kjy * kjx / var;
      gamma_x_x += conf * kjx * kjx / var;

      // totalize phi vector
      // estimate the number of cycles that we are off (using the current estimation of the disparity
      double n = round((diff - disparity[0] * kjy - disparity[1] * kjx) / (2.*M_PI));
      // totalize corrected phi vector elements
      phi_y += conf * (diff - n * 2. * M_PI) * kjy / var;
      phi_x += conf * (diff - n * 2. * M_PI) * kjx / var;
    }

    // re-calculate disparity as d=\Gamma^{-1}\Phi of the (low frequency) wavelet scales that we used up to now
    double gamma_det = gamma_x_x * gamma_y_y - sqr(gamma_y_x);
    disparity[0] = (gamma_x_x * phi_y - gamma_y_x * phi_x) / gamma_det;
    disparity[1] = (gamma_y_y * phi_x - gamma_y_x * phi_y) / gamma_det;
  }

  return disparity;
}

double bob::ip::gabor::JetStatistics::logLikelihood(const boost::shared_ptr<bob::ip::gabor::Jet> jet, bool estimate_phase, const blitz::TinyVector<double,2>& offset) const{
  double q_phase = 0.;
  double factor = 1.;
  if (estimate_phase){
    // compute the disparity for the given jet
    auto disp = disparity(jet);

    // correct disparity (which was computed from integer location)
    disp[0] -= offset[0] - (int)offset[0];
    disp[1] -= offset[1] - (int)offset[1];

    // .. and the phase part
    auto kernels = m_gwt->waveletFrequencies();
    auto abs = jet->abs(), phase = jet->phase();
    for (int j = jet->length(); j--;){
      q_phase += sqr(adjust_phase(phase(j) + kernels[j][0] * disp[0] + kernels[j][1] * disp[1] - m_meanPhase(j))) / m_varPhase(j) * abs(j) / m_varAbs(j);
    }
//    q_phase *= blitz::sum(m_varPhase);
    factor = 2.;
  }
  // compute quality measure
  // .. absolute part
  blitz::Array<double,1> diff(jet->abs() - m_meanAbs);
  double q_abs = blitz::sum(diff*diff / m_varAbs);
//  double q_abs = blitz::sum(diff*diff / m_varAbs) * blitz::sum(m_varAbs);

  return -(q_abs + q_phase)/(factor*jet->length());
}
