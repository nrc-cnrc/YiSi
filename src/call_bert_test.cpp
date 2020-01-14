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
#include <string>
#include <vector>

//#include "util.h"

using namespace std;
//using namespace yisi;


const char* default_bert_model = "multi_cased_L-12_H-768_A-12";
const char* default_layers = "-4";
const int default_max_seq_length = 512;
const int default_batch_size = 8;
const bool default_do_lower_case = false;


void call_bert(string file_path) {
   PyObject *pName, *pModule, *pClass, *pModelObject;
   PyObject *pArgs, *pValue;

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
   pValue = PyObject_CallMethod(pModelObject, (char *)"extract_features_2_yisi", (char *)"(s)", file_path.c_str());
   if (pValue == NULL) {
      PyErr_Print();
      cerr << "Python ERROR: Method call failed: BERT_model.extract_features_2_yisi" << endl;
      exit(EXIT_FAILURE);
   }

   // Process the return value here.

   Py_DECREF(pValue);
   Py_DECREF(pModelObject);
   Py_DECREF(pClass);
   Py_DECREF(pModule);
}


int main(const int argc, const char* argv[])
{
   if ( argc < 2 || argc > 2) {
      cerr << "ERROR: call_bert_test requires 1 argument, but got" << argc-1 << endl;
      cerr << "Usage: call_bert_test inp_file" << endl;
      exit (1);
   }

   string inp_file = argv[1];

   call_bert(inp_file);

   return 0;
}
