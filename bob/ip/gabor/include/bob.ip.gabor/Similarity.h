/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Thu Jun  5 17:47:55 CEST 2014
 *
 * @brief Header file for the C++ implementations of the Gabor jet similarities
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */


#ifndef BOB_IP_GABOR_SIMILARITY_H
#define BOB_IP_GABOR_SIMILARITY_H

#include <bob.io.base/HDF5File.h>
#include <bob.sp/FFT2D.h>
#include <bob.core/cast.h>

#include <bob.ip.gabor/Jet.h>

namespace bob {
  namespace ip {
    namespace gabor{
      //! Class to compute Gabor jet similarities
      class Similarity{
        public:

          //! This enum defines different types of Gabor jet similarity functions.
          //! The first functions are based on absolute values of Gabor jets,
          //! while the latter also use the Gabor phases
          typedef enum {
            SCALAR_PRODUCT = 1,
            CANBERRA = 3,
            ABS_PHASE = 8,
            DISPARITY = 16,
            PHASE_DIFF = 22,
            PHASE_DIFF_PLUS_CANBERRA = 30
          } SimilarityType;

          static const std::string& type_to_name(SimilarityType type);

          static SimilarityType name_to_type(const std::string& type);

          //! Constructor for the Gabor jet similarity
          Similarity(SimilarityType type, boost::shared_ptr<Transform> gwt = boost::shared_ptr<Transform>());

          //! \brief reads the parameters of this Gabor jet similarity from file
          Similarity(bob::io::base::HDF5File& file);

          //! The similarity between two Gabor jets, including absolute values and phases
          double similarity(const Jet& jet1, const Jet& jet2) const;

          //! returns the disparity vector estimated from the given jets
          blitz::TinyVector<double,2> disparity(const Jet& jet1, const Jet& jet2) const;

          //! returns the disparity vector estimated during the last call of similarity; only valid for disparity types
          blitz::TinyVector<double,2> disparity() const {return m_disparity;}

          //! returns the Gabor wavelet transform that is attached to this class
          boost::shared_ptr<Transform> transform() const {return m_gwt;}

          //! shifts the phases from jet towards the reference and stored the result in shifted
          void shift_phase(const Jet& jet, const Jet& reference, Jet& shifted) const;

          //! \brief saves the parameters of this Gabor jet similarity to file
          void save(bob::io::base::HDF5File& file) const;

          //! \brief reads the parameters of this Gabor jet similarity from file
          void load(bob::io::base::HDF5File& file);

          const std::string& type () const {return type_to_name(m_type);}

        private:
          // members for all similarity functions
          SimilarityType m_type;

          // members required by disparity functions
          boost::shared_ptr<Transform> m_gwt;

          // initializes the internal memory to be used for disparity-like Gabor jet similarities
          void init();
          // computes confidences from the given Gabor jets
          void compute_confidences(const Jet& jet1, const Jet& jet2) const;
          // computes the disparity using the m_confidences and m_phase_differences values
          void compute_disparity() const;

          mutable blitz::TinyVector<double,2> m_disparity;

          mutable blitz::Array<double,1> m_confidences;
          mutable blitz::Array<double,1> m_phase_differences;

      }; // class Similarity
    } // namespace gabor
  } // namespace ip
} // namespace bob


#endif // BOB_IP_GABOR_SIMILARITY_H
