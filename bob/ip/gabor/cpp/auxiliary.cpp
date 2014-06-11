/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date Wed Jun  4 17:44:24 CEST 2014
 *
 * @brief Header file for the C++ implementations of the auxiliary functions
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include "auxiliary.h"

void bob::ip::gabor::average(const std::vector<boost::shared_ptr<bob::ip::gabor::Jet>>& jets, boost::shared_ptr<bob::ip::gabor::Jet>& average, bool normalize){
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

  if (!average)
    average.reset(new bob::ip::gabor::Jet(mean, normalize));
  else
    average->init(mean, normalize);
}
