/**
 * @file contextual_test.cpp
 * @brief Unit test for contextual_t class.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2020, Her Majesty in Right of Canada /
 * Copyright 2020, Sa Majeste la Reine du Chef du Canada
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "contextual.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

int main(const int argc, const char* argv[])
{
  string usage("Usage: contextual_test <config string> < in-file\nwrites output to stdout");

  if (argc < 2 || argc > 2) {
    cerr << "ERROR: contextual_test requires 1 argument, but got " << argc-1 << endl;
    cerr << usage << endl;
    exit(1);
  }

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) {
    cerr << usage << endl;
    exit(0);
  }

  yisi::contextual_t *contextual_p = new yisi::contextual_t(string(argv[1]));
  string line;
  while (std::getline(cin, line)){
    auto result = contextual_p->get_features(line);
    cout <<result.lmscore_m<<endl;
    auto units = result.units_m;
    for (auto it = units.begin(); it != units.end(); it++){
      cout << *it << " ";
    }
    cout << endl;
  }

  return 0;
}
