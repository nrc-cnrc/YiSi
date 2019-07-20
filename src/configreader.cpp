/**
 * @file configreader.cpp
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


#include "cmdlp/options.h"
#include "yisi.h"

#include "configreader.h"

using namespace yisi;

// read_config wraps the cmdlp construction for use with the yisi python extension.
extern "C" ConfigReader read_config(std::string path) {
   typedef com::masaers::cmdlp::options< eval_options, yisi_options, phrasesim_options > options_type;
   const char* argv[]{
       "",
       "--config",
       path.c_str(),
   };

   options_type opt(3, argv);
   if (!opt) {
      throw std::runtime_error("error reading config file");
   }

   ConfigReader config;
   config.yisi = opt;
   config.phrasesim = opt;
   config.eval = opt;
   return config;
}
