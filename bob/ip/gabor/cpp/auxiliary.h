/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief Header file for the C++ implementations of the auxiliary functions
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */


#ifndef BOB_IP_GABOR_AUX_H
#define BOB_IP_GABOR_AUX_H

#include "Similarity.h"

namespace bob{
  namespace ip{
    namespace gabor{

      void average(const std::vector<boost::shared_ptr<Jet>>& jets, boost::shared_ptr<Jet>& average, bool normalize=true);

    }
  }
}

#endif // BOB_IP_GABOR_AUX_H
