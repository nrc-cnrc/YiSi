/**
 * @file call_bert_test.cpp
 * @brief Unit test for calling BERT.
 *
 * @author Darlene Stewart
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2019, Her Majesty in Right of Canada /
 * Copyright 2019, Sa Majeste la Reine du Chef du Canada
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;


const char* default_bert_model = "multi_cased_L-12_H-768_A-12";
const char* default_layers = "-4";
const int default_max_seq_length = 512;
const int default_batch_size = 8;
const bool default_do_lower_case = false;


PyObject *call_bert(string file_path) {
   PyObject *pName, *pModule, *pClass, *pModelObject;
   PyObject *pArgs, *pValue, *pResult;

   Py_SetProgramName(Py_DecodeLocale("python3", NULL));

   Py_Initialize();
   cerr << "Python program: " << Py_EncodeLocale(Py_GetProgramName(), NULL) << endl;
   cerr << "Python program full path: " << Py_EncodeLocale(Py_GetProgramFullPath(), NULL) << endl;
   cerr << "Python version: " << Py_GetVersion() << endl;

   const char* bert_module_name = "BERT_wrapper";

   pName = PyUnicode_FromString(bert_module_name);
   if (pName == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: cannot convert string: " << bert_module_name << endl;
      exit(EXIT_FAILURE);
   }

   pModule = PyImport_Import(pName);
   Py_DECREF(pName);
   if (pModule == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Failed to load: " << bert_module_name << endl;
      exit(EXIT_FAILURE);
   }
   cerr << "Imported" << bert_module_name << "." << endl;

   pClass = PyObject_GetAttrString(pModule, "BERT_model");
   /* pClass is a new reference */
   if (pModule == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Cannot find class: BERT_model" << endl;
      exit(EXIT_FAILURE);
   }

   if (! PyCallable_Check(pClass)) {
      cerr << "Python ERROR: Not Callable: BERT_model" << endl;
      exit(EXIT_FAILURE);
   }

   auto check_and_set_pArgs =
        [&](int idx, string type, string cvalue) {
      if (pValue == NULL) {
         PyErr_Print();
         cerr << "Python ERROR: Cannot convert " << type << ": " << cvalue << endl;
         exit(EXIT_FAILURE);
      }
      /* pValue reference stolen here: */
      PyTuple_SetItem(pArgs, idx, pValue);
   };

   pArgs = PyTuple_New(5);
   if (pArgs == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Cannot create pArgs tuple of size 5." << endl;
      exit(EXIT_FAILURE);
   }
   pValue = PyUnicode_FromString(default_bert_model);
   check_and_set_pArgs(0, "string", default_bert_model);
   pValue = PyUnicode_FromString(default_layers);
   check_and_set_pArgs(1, "string", default_layers);
   pValue = PyLong_FromLong(default_max_seq_length);
   check_and_set_pArgs(2, "int", to_string(default_max_seq_length));
   pValue = PyLong_FromLong(default_batch_size);
   check_and_set_pArgs(3, "int", to_string(default_batch_size));
   pValue = PyBool_FromLong(default_do_lower_case);
   check_and_set_pArgs(4, "bool", to_string(default_do_lower_case));
   pModelObject = PyObject_CallObject(pClass, pArgs);
   Py_DECREF(pArgs);
   if (pModelObject == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Cannot instantiate Python class: BERT_model" << endl;
      exit(EXIT_FAILURE);
   }
   cerr << "BERT_model instance created." << endl;
   cerr << "Calling BERT_model.extract_features_2_yisi." << endl;
   pResult = PyObject_CallMethod(pModelObject, (char *)"extract_features_2_yisi", (char *)"(s)", file_path.c_str());
   if (pResult == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Method call failed: BERT_model.extract_features_2_yisi" << endl;
      exit(EXIT_FAILURE);
   }

   Py_DECREF(pModelObject);
   Py_DECREF(pClass);
   Py_DECREF(pModule);
   return pResult;
}

void process_bert_result(PyObject *pResult, string output_path) {
   // Process the returned BERT results here.
   // extract_features_2_yisi returns sents
   // sents is a list of 3-tuples (units, toks, embeddings)
   //   units is a list of strings
   //   toks is a list of strings
   //   embeddings is a list of 2-tuples (tid, unit-emb)
   //     tid is a integer
   //     unit-emb is a list of floats

   ofstream fout(output_path);
   if (! fout) {
      cerr << "ERROR: Failed to open output file (" << output_path << "). Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   if (! PySequence_Check(pResult)) {
      cerr << "Python ERROR: BERT_model.extract_features_2_yisi did not return a sequence." << endl;
      exit(EXIT_FAILURE);
   }

   // NOTE: PySequence_Fast_GET_ITEM returns a borrowed reference, not a new
   // reference, so don't call Py_DECREF when all done with the reference!

   PyObject *pResultF = PySequence_Fast(pResult, "Expected result to be a sequence.");
   Py_ssize_t sents_len = PySequence_Fast_GET_SIZE(pResultF);
   cerr << "BERT_model.extract_features_2_yisi returned " << sents_len << " sentences." << endl;

   for (auto sent_idx = 0; sent_idx < sents_len; ++sent_idx) {
      PyObject *pSentF = PySequence_Fast(PySequence_Fast_GET_ITEM(pResultF, sent_idx),
                                        "Expected sent to be a sequence.");
      if (PySequence_Fast_GET_SIZE(pSentF) != 3) {      // Sanity check
         cerr << "Python ERROR: Invalid tuple length for sentence " << sent_idx << ": "
              << PySequence_Fast_GET_SIZE(pSentF) << endl;
         Py_DECREF(pSentF);
         continue;
      }
      PyObject *pUnits = PySequence_Fast_GET_ITEM(pSentF, 0);
      PyObject *pUnitsF = PySequence_Fast(pUnits, "Expected units to be a sequence.");
      Py_ssize_t units_len = PySequence_Fast_GET_SIZE(pUnitsF);
      PyObject *pToks = PySequence_Fast_GET_ITEM(pSentF, 1);
      PyObject *pToksF = PySequence_Fast(pToks, "Expected toks to be be a sequence.");
      Py_ssize_t toks_len = PySequence_Fast_GET_SIZE(pToksF);
      PyObject *pEmbeddings = PySequence_Fast_GET_ITEM(pSentF, 2);
      PyObject *pEmbeddingsF = PySequence_Fast(pEmbeddings, "Expected embeddings to be a sequence.");
      Py_ssize_t embeddings_len = PySequence_Fast_GET_SIZE(pEmbeddingsF);
      cerr << endl;
      cerr << "Sentence " << sent_idx << " lengths: units=" << units_len
           << " toks=" << toks_len << " embeddings=" << embeddings_len << endl;
      cerr << "  Units:";
      for (auto i = 0; i < units_len; ++i) {
         PyObject *pUnit = PySequence_Fast_GET_ITEM(pUnitsF, i);
         cerr << " " << PyUnicode_AsUTF8(pUnit);
      }
      cerr << endl;
      cerr << "  Toks:";
      for (auto i = 0; i < toks_len; ++i) {
         PyObject *pTok = PySequence_Fast_GET_ITEM(pToksF, i);
         cerr << " " << PyUnicode_AsUTF8(pTok);
      }
      cerr << endl;
      for (auto emb_idx = 0; emb_idx < embeddings_len; ++emb_idx) {
         PyObject *pEmbedding = PySequence_Fast_GET_ITEM(pEmbeddingsF, emb_idx);
         PyObject *pEmbeddingF = PySequence_Fast(pEmbedding, "Expected embedding to be a sequence.");
         if (PySequence_Fast_GET_SIZE(pEmbeddingF) != 2) {      // Sanity check
            cerr << "Python ERROR: Invalid tuple length for embedding " << emb_idx << ": "
                 << PySequence_Fast_GET_SIZE(pEmbeddingF) << endl;
            Py_DECREF(pEmbeddingF);
            continue;
         }
         PyObject *pTid = PySequence_Fast_GET_ITEM(pEmbeddingF, 0);
         PyObject *pUnit_emb = PySequence_Fast_GET_ITEM(pEmbeddingF, 1);
         PyObject *pUnit_embF = PySequence_Fast(pUnit_emb, "Expected unit_emb to be a sequence.");
         Py_ssize_t unit_emb_len = PySequence_Fast_GET_SIZE(pUnit_embF);

         // Output to fout in the .emb file format; also output to cerr with extra detail.
         cerr << "Unit embedding " << emb_idx << ": " << PyLong_AsSize_t(pTid)
              << " len: " << unit_emb_len << ": ";
         fout << emb_idx << "\t" << PyLong_AsSize_t(pTid) << "\t";
         cerr << fixed << setprecision(4);
         fout << fixed << setprecision(6);
         for (auto i = 0; i < min(unit_emb_len, (Py_ssize_t)16); ++i) {
            PyObject *pValue = PySequence_Fast_GET_ITEM(pUnit_embF, i);
            if (i < 8)
               cerr << " " << PyFloat_AsDouble(pValue);
            if (i != 0)
               fout << " ";
            fout << PyFloat_AsDouble(pValue);
         }
         cerr << endl;
         fout << endl;

         Py_DECREF(pUnit_embF);
         Py_DECREF(pEmbeddingF);
      }
      fout << endl;
      Py_DECREF(pSentF);
      Py_DECREF(pUnitsF);
      Py_DECREF(pToksF);
      Py_DECREF(pEmbeddingsF);
   }

   Py_DECREF(pResultF);
   fout.close();
}


int main(const int argc, const char* argv[])
{
   string usage("Usage: call_bert_test <in-file>\nwrites output to <in-file>.emb");

   if (argc < 2 || argc > 2) {
      cerr << "ERROR: call_bert_test requires 1 argument, but got " << argc-1 << endl;
      cerr << usage << endl;
      exit(1);
   }

   if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) {
      cerr << usage << endl;
      exit(0);
   }

   string in_file = argv[1];

   PyObject *pResult = call_bert(in_file);
   process_bert_result(pResult, in_file+".emb");
   Py_DECREF(pResult);

   return 0;
}
