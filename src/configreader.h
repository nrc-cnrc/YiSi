/**
 * @file configreader.h
 * @brief Reads a configuration file into a simple structure
 *
 * @author Serge Leger
 *
 * Class definition
 *    - ConfigReader
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2019, Her Majesty in Right of Canada /
 * Copyright 2019, Sa Majeste la Reine du Chef du Canada
 */

#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include "phrasesim.h"
#include "yisiscorer.h"

namespace yisi {
  // ConfigReader wraps the yisi_options and phrasesim_options structures for use with Cython.
  struct ConfigReader {
    yisi::eval_options eval;
    yisi::yisi_options yisi;
    yisi::phrasesim_options phrasesim;
  };

  // read_config reads a cmdlp configuration file into a ConfigReader
  extern "C" ConfigReader read_config(std::string path);

}  // namespace yisi

#endif
