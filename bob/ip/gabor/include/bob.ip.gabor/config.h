/**
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 * @date   Tue Nov 18 17:54:06 CET 2014
 *
 * @brief General directives for all modules in bob.ip.gabor
 */

#ifndef BOB_IP_GABOR_CONFIG_H
#define BOB_IP_GABOR_CONFIG_H

/* Macros that define versions and important names */
#define BOB_IP_GABOR_API_VERSION 0x0200

#ifdef BOB_IMPORT_VERSION

  /***************************************
  * Here we define some functions that should be used to build version dictionaries in the version.cpp file
  * There will be a compiler warning, when these functions are not used, so use them!
  ***************************************/

  #include <Python.h>
  #include <boost/preprocessor/stringize.hpp>

  /**
   * bob.ip.gabor c/c++ api version
   */
  static PyObject* bob_ip_gabor_version() {
    return Py_BuildValue("{ss}", "api", BOOST_PP_STRINGIZE(BOB_IP_GABOR_API_VERSION));
  }

#endif // BOB_IMPORT_VERSION

#endif /* BOB_IP_GABOR_CONFIG_H */
