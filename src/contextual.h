/**
 * @file contextual.h
 * @brief interface with contextual LM
 *
 * @author Jackie Lo
 *
 * Class definition of:
 *    - contextual_t (wrapper class for contextual LM using HuggingFace transformer API)
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2020, Her Majesty in Right of Canada /
 * Copyright 2020, Sa Majeste la Reine du Chef du Canada
 */

#ifndef CONTEXTUAL_H
#define CONTEXTUAL_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>
#include <vector>

namespace yisi {
  struct contextualfeatures_t{
    double lmscore_m;
    std::vector<std::string> units_m;
    std::vector<std::vector <double> > embeddings_m;
  };
  class contextual_t {
    public:
      contextual_t() {}
      /// Contructor with config info provided
      contextual_t(std::string config_str);
      ~contextual_t();
      /// Get contextualized features of a sentence
      /// @return contextual feature object
      contextualfeatures_t get_features(std::string sent);
      contextualfeatures_t get_proj_features(std::string sent);
    private:
      /// Has Python been initialized?
      static bool is_python_init_m;
      /// Number of contextual_t instances created.
      static int obj_cnt_m;
      /// Python Contextual model object
      PyObject *p_contextual_model_object_m;
  };

} // yisi

#endif
