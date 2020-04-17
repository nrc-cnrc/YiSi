/**
 * @file bert.cpp
 * @brief BERT model
 *
 * @author Darlene Stewart
 *
 * Class implementation of:
 *    - bert_t (wrapper class for BERT tools)
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2020, Her Majesty in Right of Canada /
 * Copyright 2020, Sa Majeste la Reine du Chef du Canada
 */

#include "bert.h"
#include "util.h"

#include <iostream>
#include <fstream>
#include <iomanip>
//#include <sstream>
//#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace yisi;
using namespace std;

bool bert_t::is_python_init_m = false;
int bert_t::obj_cnt_m = 0;

const char* bert_bert_default_bert_model = "multi_cased_L-12_H-768_A-12";
const char* bert_bert_default_layers = "-4";
const int bert_bert_default_max_seq_length = 512;
const int bert_bert_default_batch_size = 8;
const bool bert_bert_default_do_lower_case = false;

bert_t::bert_t(string config_path) {
   PyObject *pName, *pModule, *pClass, *pModelObject;
   PyObject *pArgs, *pValue;

   cerr << "Setting up BERT ..." << endl;

   // Read the BERT config file.
//   ifstream fin(config_path.c_str());
//   if (!fin) {
//      cerr << "ERROR: Failed to open BERT config file (" << config_path << "). Exiting..." << endl;
//      exit(1);
//   }
//
//   while (!fin.eof()) {
//      string line;
//      getline(fin, line);
//      istringstream iss(line);
//      string cfgn, cfgv;
//      getline(iss, cfgn, '=');
//      getline(iss, cfgv);
////      if (cfgn == "yisi_home") {
////         yisi_home = cfgv;
////      } else if (cfgn == "lang") {
////         lang = cfgv;
////      }
//   }

   // Initialize the Python Interpreter, if not already done.
   if (! is_python_init_m) {
      Py_SetProgramName(Py_DecodeLocale("python3", NULL));
      Py_Initialize();
      cerr << "Python program: " << Py_EncodeLocale(Py_GetProgramName(), NULL) << endl;
      cerr << "Python program full path: " << Py_EncodeLocale(Py_GetProgramFullPath(), NULL) << endl;
      cerr << "Python version: " << Py_GetVersion() << endl;
      is_python_init_m = true;
   }
   ++obj_cnt_m;

   // Initialize the Bert Model
   const char* bert_module_name = "BERT_wrapper";

   pName = PyUnicode_FromString(bert_module_name);
   if (pName == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: cannot convert string: " << bert_module_name << ". Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   pModule = PyImport_Import(pName);
   Py_DECREF(pName);
   if (pModule == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Failed to load: " << bert_module_name << ". Exiting..." << endl;
      exit(EXIT_FAILURE);
   }
   cerr << "Imported" << bert_module_name << "." << endl;

   pClass = PyObject_GetAttrString(pModule, "BERT_model");
   /* pClass is a new reference */
   if (pModule == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Cannot find class: BERT_model. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   if (! PyCallable_Check(pClass)) {
      cerr << "Python ERROR: Not Callable: BERT_model. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   auto check_and_set_pArgs =
        [&](int idx, string type, string cvalue) {
      if (pValue == NULL) {
         PyErr_Print();
         cerr << "Python ERROR: Cannot convert " << type << ": " << cvalue << ". Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
      /* pValue reference stolen here: */
      PyTuple_SetItem(pArgs, idx, pValue);
   };

   pArgs = PyTuple_New(5);
   if (pArgs == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Cannot create pArgs tuple of size 5. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }
   pValue = PyUnicode_FromString(bert_bert_default_bert_model);
   check_and_set_pArgs(0, "string", bert_bert_default_bert_model);
   pValue = PyUnicode_FromString(bert_bert_default_layers);
   check_and_set_pArgs(1, "string", bert_bert_default_layers);
   pValue = PyLong_FromLong(bert_bert_default_max_seq_length);
   check_and_set_pArgs(2, "int", to_string(bert_bert_default_max_seq_length));
   pValue = PyLong_FromLong(bert_bert_default_batch_size);
   check_and_set_pArgs(3, "int", to_string(bert_bert_default_batch_size));
   pValue = PyBool_FromLong(bert_bert_default_do_lower_case);
   check_and_set_pArgs(4, "bool", to_string(bert_bert_default_do_lower_case));
   pModelObject = PyObject_CallObject(pClass, pArgs);
   Py_DECREF(pArgs);
   if (pModelObject == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Cannot instantiate Python class: BERT_model. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }
   cerr << "BERT_model instance created." << endl;

   Py_DECREF(pClass);
   Py_DECREF(pModule);

   p_bert_model_object_m = pModelObject;
   cerr << "Done setting up BERT." << endl;
}  // bert_t

bert_t::~bert_t() {
   --obj_cnt_m;
   Py_DECREF(p_bert_model_object_m);
   p_bert_model_object_m = NULL;
} // ~bert_t

size_t bert_t::apply_model(string text_path) {
   cerr << "Calling BERT_model.extract_features_2_yisi." << endl;
   PyObject *pResult = PyObject_CallMethod(p_bert_model_object_m,
                                           (char *)"extract_features_2_yisi",
                                           (char *)"(s)",
                                           text_path.c_str());
   if (pResult == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Method call failed: BERT_model.extract_features_2_yisi."
           << " Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   if (! PySequence_Check(pResult)) {
      cerr << "Python ERROR: BERT_model.extract_features_2_yisi did not return a sequence."
           << " Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   PyObject *pResultF = PySequence_Fast(pResult, "Expected result to be a sequence.");

   Py_DECREF(pResult);

   p_results_m.push_back(pResultF);
   size_t results_id = p_results_m.size();
   cerr << "BERT_model.extract_features_2_yisi returned "
        << get_size(results_id) << " sentences." << endl;

   return results_id;
} // apply_model

PyObject *bert_t::get_results(size_t results_id) {
   // Private helper function for bert_t::get_size and bert_t::get_sentence.
   // Returns a BORROWED REF to a PySequence_Fast object.
   // DO NOT Call Py_DECREF when finished with the REF.
   // Note: results_id is 1-based.
   if (results_id == 0 || results_id > p_results_m.size()) {
      cerr << "ERROR: unknown results id: " << results_id << ". Exiting..." << endl;
      exit(EXIT_FAILURE);
   }
   return p_results_m[results_id-1];
} // get_results

size_t bert_t::get_size(size_t results_id) {
   PyObject *pResultF = get_results(results_id);

   Py_ssize_t sents_len = PySequence_Fast_GET_SIZE(pResultF);

   return sents_len;
} // get_size

PyObject *bert_t::get_sentence(size_t results_id, size_t sent_idx) {
   // Private helper function for bert_t::get_units and bert_t::get_embeddings.
   // Returns a NEW REF to a PySequence_Fast object.
   // Call Py_DECREF when finished with the REF.
   PyObject *pResultF = get_results(results_id);

   PyObject *pSentF = PySequence_Fast(PySequence_Fast_GET_ITEM(pResultF, sent_idx),
                                     "Expected sent to be a sequence.");
   if (PySequence_Fast_GET_SIZE(pSentF) != 3) {      // Sanity check
      cerr << "Python ERROR: Invalid tuple length for sentence " << sent_idx << ": "
           << PySequence_Fast_GET_SIZE(pSentF) << endl;
      Py_DECREF(pSentF);
      pSentF = NULL;
   }

   return pSentF;
} // get_sentence

vector<string> bert_t::get_units(size_t results_id, size_t sent_idx, bool verbose) {
   vector<string> units;

   // NOTE: PySequence_Fast_GET_ITEM returns a borrowed reference, not a new
   // reference, so don't call Py_DECREF when all done with the reference!

   PyObject *pSentF = get_sentence(results_id, sent_idx);
   if (! pSentF)
      return units;

   PyObject *pUnits = PySequence_Fast_GET_ITEM(pSentF, 0);
   PyObject *pUnitsF = PySequence_Fast(pUnits, "Expected units to be a sequence.");
   Py_ssize_t units_len = PySequence_Fast_GET_SIZE(pUnitsF);
   if (verbose)
      cerr << "Sentence " << sent_idx << " units length=" << units_len;

   units.reserve((size_t)units_len);

   for (auto i = 0; i < units_len; ++i) {
      PyObject *pUnit = PySequence_Fast_GET_ITEM(pUnitsF, i);
      units.push_back(string(PyUnicode_AsUTF8(pUnit)));
      if (verbose)
         cerr << " " << units.back();
   }
   if (verbose)
      cerr << endl;

   Py_DECREF(pSentF);
   Py_DECREF(pUnitsF);

   return units;
} // get_units

vector<vector<double>> bert_t::get_embeddings(size_t results_id, size_t sent_idx, bool verbose) {
   vector<vector<double>> embeddings;

   // NOTE: PySequence_Fast_GET_ITEM returns a borrowed reference, not a new
   // reference, so don't call Py_DECREF when all done with the reference!

   PyObject *pSentF = get_sentence(results_id, sent_idx);
   if (! pSentF)
      return embeddings;

   PyObject *pEmbeddings = PySequence_Fast_GET_ITEM(pSentF, 2);
   PyObject *pEmbeddingsF = PySequence_Fast(pEmbeddings, "Expected embeddings to be a sequence.");
   Py_ssize_t embeddings_len = PySequence_Fast_GET_SIZE(pEmbeddingsF);
   if (verbose)
      cerr << "Sentence " << sent_idx << " embeddings length=" << embeddings_len << endl;

   embeddings.reserve((size_t)embeddings_len);

   for (auto emb_idx = 0; emb_idx < embeddings_len; ++emb_idx) {
      vector<double> unit_emb;
      PyObject *pEmbedding = PySequence_Fast_GET_ITEM(pEmbeddingsF, emb_idx);
      PyObject *pEmbeddingF = PySequence_Fast(pEmbedding, "Expected embedding to be a sequence.");
      if (PySequence_Fast_GET_SIZE(pEmbeddingF) != 2) {      // Sanity check
         cerr << "Python ERROR: Invalid tuple length for embedding " << emb_idx << ": "
              << PySequence_Fast_GET_SIZE(pEmbeddingF) << endl;
         Py_DECREF(pEmbeddingF);
         continue;
      }
      PyObject *pUnit_emb = PySequence_Fast_GET_ITEM(pEmbeddingF, 1);
      PyObject *pUnit_embF = PySequence_Fast(pUnit_emb, "Expected unit_emb to be a sequence.");
      Py_ssize_t unit_emb_len = PySequence_Fast_GET_SIZE(pUnit_embF);

      if (verbose) {
         cerr << "Unit embedding " << emb_idx << " length=" << unit_emb_len << ": ";
         cerr << fixed << setprecision(4);
      }
      unit_emb.reserve((size_t)unit_emb_len);
      for (auto i = 0; i < unit_emb_len; ++i) {
         PyObject *pValue = PySequence_Fast_GET_ITEM(pUnit_embF, i);
         unit_emb.push_back(PyFloat_AsDouble(pValue));
         if (verbose)
            if (i < 8) cerr << " " << unit_emb.back();
      }
      if (verbose)
         cerr << endl;

      embeddings.push_back(unit_emb);

      Py_DECREF(pUnit_embF);
      Py_DECREF(pEmbeddingF);
   }

   Py_DECREF(pSentF);
   Py_DECREF(pEmbeddingsF);

   return embeddings;
} // get_embeddings
