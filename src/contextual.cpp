/**
 * @file contextual.cpp
 * @brief interface with contextual LM
 *
 * @author Jackie Lo
 *
 * Class implementation of:
 *    - contextual_t (wrapper class for contextual LM using HuggingFace transformer API)
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2020, Her Majesty in Right of Canada /
 * Copyright 2020, Sa Majeste la Reine du Chef du Canada
 */

#include "contextual.h"
#include "util.h"

#include <iostream>
//#include <fstream>
//#include <iomanip>
//#include <sstream>
//#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace yisi;
using namespace std;

bool contextual_t::is_python_init_m = false;
int contextual_t::obj_cnt_m = 0;

contextual_t::contextual_t(string config_str) {
   auto configs = tokenize(config_str, ':');
   string modelname, layer;
   if (configs.size()<2){
     cerr <<"ERROR: Too few parameters for contextual model. Exiting...";
     exit(EXIT_FAILURE);
   } else {
     modelname = configs[0];
     layer = configs[1];
   }
   
   PyObject *pName, *pModule, *pClass, *pModelObject;
   PyObject *pArgs, *pValue;

   cerr << "Setting up python ..." << endl;

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
   cerr<< "Done." <<endl;
   
   // Initialize the contextual Model
   const char* contextual_module_name = "HuggingFace_wrapper";
   cerr << "Importing " << contextual_module_name <<" ... ";
   pName = PyUnicode_FromString(contextual_module_name);
   if (pName == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: cannot convert string: " << contextual_module_name << ". Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   pModule = PyImport_Import(pName);
   Py_DECREF(pName);
   if (pModule == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Failed to load: " << contextual_module_name << ". Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   pClass = PyObject_GetAttrString(pModule, "Contextual_t");
   // pClass is a new reference 
   if (pModule == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Cannot find class: Contextual_t. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   if (! PyCallable_Check(pClass)) {
      cerr << "Python ERROR: Not Callable: Contextual_t. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   auto check_and_set_pArgs =
        [&](int idx, string type, string cvalue) {
      if (pValue == NULL) {
         PyErr_Print();
         cerr << "Python ERROR: Cannot convert " << type << ": " << cvalue << ". Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
      // pValue reference stolen here: 
      PyTuple_SetItem(pArgs, idx, pValue);
   };

   pArgs = PyTuple_New(2);
   if (pArgs == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Cannot create pArgs tuple of size 2. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }
   
   pValue = PyUnicode_FromString(modelname.c_str());
   check_and_set_pArgs(0, "string", modelname.c_str());
   pValue = PyUnicode_FromString(layer.c_str());
   check_and_set_pArgs(1, "string", layer.c_str());

   cerr << "Done." << endl;

   cerr << "Loading " << modelname << " ... ";

   pModelObject = PyObject_CallObject(pClass, pArgs);
   Py_DECREF(pArgs);
   if (pModelObject == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Cannot instantiate Python class: Contextual_t. Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   cerr << "Done" << endl;

   Py_DECREF(pClass);
   Py_DECREF(pModule);

   p_contextual_model_object_m = pModelObject;

}  // contextual_t

contextual_t::~contextual_t() {
   --obj_cnt_m;
   Py_DECREF(p_contextual_model_object_m);
   p_contextual_model_object_m = NULL;
} // ~contextual_t

contextualfeatures_t contextual_t::get_features(string sent) {
   //cerr << "Calling Contextual_t.get_features ... " << endl;
   contextualfeatures_t result;
   PyObject *pResult = PyObject_CallMethod(p_contextual_model_object_m,
                                           (char *)"get_features",
                                           (char *)"(s)",
                                           sent.c_str());
   if (pResult == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Method call failed: Contextual_t.get_features."
           << " Exiting..." << endl;
      exit(EXIT_FAILURE);
   }

   if (! PySequence_Check(pResult)) {
      cerr << "Python ERROR: Contextual_t.get_features did not return a sequence."
           << " Exiting..." << endl;
      exit(EXIT_FAILURE);
   }
   
   PyObject *pResultF = PySequence_Fast(pResult, "Expected result to be a sequence.");

   PyObject *pLmscore = PySequence_Fast_GET_ITEM(pResultF, 0);
   result.lmscore_m = PyFloat_AsDouble(pLmscore);
   
   PyObject *pUnits = PySequence_Fast_GET_ITEM(pResultF, 1);
   PyObject *pUnitsF = PySequence_Fast(pUnits, "Expected result to be a sequence.");
   Py_ssize_t units_len = PySequence_Fast_GET_SIZE(pUnitsF);
   result.units_m.reserve((size_t)units_len);
   for (auto i=0; i < units_len; i++) {
     PyObject *pUnit = PySequence_Fast_GET_ITEM(pUnitsF, i);
     result.units_m.push_back(string(PyUnicode_AsUTF8(pUnit)));
   }

   PyObject *pEmbeddings = PySequence_Fast_GET_ITEM(pResultF, 2);
   PyObject *pEmbeddingsF = PySequence_Fast(pEmbeddings, "Expected embeddings to be a sequence.");
   Py_ssize_t embeddings_len = PySequence_Fast_GET_SIZE(pEmbeddingsF);
   result.embeddings_m.reserve((size_t)embeddings_len);

   for (auto i=0; i< embeddings_len; i++){
     vector<double> unit_emb;
     PyObject *pEmbedding = PySequence_Fast_GET_ITEM(pEmbeddingsF, i);
     PyObject *pEmbeddingF = PySequence_Fast(pEmbedding, "Expected embedding to be a sequence.");
     Py_ssize_t unit_emb_len = PySequence_Fast_GET_SIZE(pEmbeddingF);
     unit_emb.reserve((size_t)unit_emb_len);
     for (auto j=0; j<unit_emb_len; j++){
       PyObject *pValue = PySequence_Fast_GET_ITEM(pEmbeddingF, j);
       unit_emb.push_back(PyFloat_AsDouble(pValue));
     }
     result.embeddings_m.push_back(unit_emb);
     Py_DECREF(pEmbeddingF);
   }
   Py_DECREF(pEmbeddingsF);
   Py_DECREF(pUnitsF);
   Py_DECREF(pResultF);
   Py_DECREF(pResult);
   return result;
} // get_features
