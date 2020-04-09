/**
 * @file bert.h
 * @brief BERT model
 *
 * @author Darlene Stewart
 *
 * Class definition of:
 *    - bert_t (wrapper class for BERT tools)
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2020, Her Majesty in Right of Canada /
 * Copyright 2020, Sa Majeste la Reine du Chef du Canada
 */

#ifndef BERT_H
#define BERT_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>
#include <vector>

namespace yisi {

   class bert_t {
   public:
      bert_t() {}
      /// Contructor with config info provided
      bert_t(std::string config_path);
      ~bert_t();
      /// Apply a Bert model to a text file.
      /// @return Results ID (1-based) identifying the results set to query methods.
      size_t apply_model(std::string text_path);
      /// Return the size of a results set, i.e. the number of sentences.
      size_t get_size(size_t results_id);
      /// Return the subword units for a specific sentence.
      std::vector<std::string> get_units(size_t results_id, size_t sent_idx, bool verbose=false);
      /// Return the embeddings for a specific sentence.
      std::vector<std::vector<double>> get_embeddings(size_t results_id, size_t sent_idx, bool verbose=false);
   private:
      /// Helper method used by the get_size and get_sentence query methods.
      PyObject *get_results(size_t results_id);
      /// Helper method used by the get_units and get_embeddings query methods.
      PyObject *get_sentence(size_t results_id, size_t sent_idx);
      /// Has Python been initialized?
      static bool is_python_init_m;
      /// Number of bert_t instances created.
      static int obj_cnt_m;
      /// Python Bert model object
      PyObject *p_bert_model_object_m;
      /// Vector of all sets of results for all files for which apply_model was called.
      std::vector<PyObject *> p_results_m;
   };

} // yisi

#endif
