/**
 * @author Manuel Gunther <mgunter@vast.uccs.edu>
 * @date Tue Oct 20 09:17:56 MDT 2015
 *
 * @brief Header file for the C++ implementations of a Gabor jet statistics
 *
 */


#ifndef BOB_IP_GABOR_JET_STATISTICS_H
#define BOB_IP_GABOR_JET_STATISTICS_H

#include <bob.io.base/HDF5File.h>
#include <bob.ip.gabor/Jet.h>
#include <math.h>

namespace bob { namespace ip { namespace gabor {

class JetStatistics {
  public:
    JetStatistics(const std::vector<boost::shared_ptr<bob::ip::gabor::Jet>>& jets, boost::shared_ptr<bob::ip::gabor::Transform> gwt = boost::shared_ptr<bob::ip::gabor::Transform>());
    JetStatistics(bob::io::base::HDF5File& hdf5);

    //! Equality operator
    bool operator==(const JetStatistics& other) const;

    // getter functions
    const blitz::Array<double,1>& meanAbs() const {return m_meanAbs;}
    const blitz::Array<double,1>& varAbs() const {return m_varAbs;}
    const blitz::Array<double,1>& meanPhase() const {return m_meanPhase;}
    const blitz::Array<double,1>& varPhase() const {return m_varPhase;}
    // gets the utilized Transform class
    boost::shared_ptr<bob::ip::gabor::Transform> gwt() {return m_gwt;}
    // sets the utilized Transform class
    void gwt(boost::shared_ptr<bob::ip::gabor::Transform> gwt) {m_gwt = gwt;}

    // helper function to get the phase shifted between -pi and pi
    static double adjust_phase(const double phase){
      return phase - (2.*M_PI)*round(phase / (2.*M_PI));
    }

    // saves this configuration to file
    void save(bob::io::base::HDF5File& hdf5, bool saveTransform = true) const;

    // computes the estimated disparity of the given jet towards the mean and variance given in these statistics
    blitz::TinyVector<double, 2> disparity(const boost::shared_ptr<bob::ip::gabor::Jet> jet) const;

    // computes the log likelihood that the given jet fits to these statistics; always negative
    double logLikelihood(const boost::shared_ptr<bob::ip::gabor::Jet> jet, bool estimate_phase = true, const blitz::TinyVector<double,2>& offset=blitz::TinyVector<double,2>(0.,0.)) const;

  protected:
    // means and variances of absolute and phase values of the jets
    blitz::Array<double,1> m_meanAbs, m_meanPhase, m_varAbs, m_varPhase;
    // the transform, with which the jets were extracted
    boost::shared_ptr<bob::ip::gabor::Transform> m_gwt;

  private:
    // cached sotrage to speed up computation
    mutable blitz::Array<double,1> m_confidences, m_phaseDifferences;
};

} } } // namespaces

#endif // BOB_IP_GABOR_JET_STATISTICS_H
