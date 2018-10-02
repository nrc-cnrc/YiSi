/**
 * @file lexsim_test.cpp
 * @brief Unit test for lexsim.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "lexsim.h"

#include <iostream>

using namespace std;
using namespace yisi;

// globals

// main

int main(int argc, char* argv[])
{
  string lsname=argv[1];
  string lspath;
  if (argc > 2){
    lspath=argv[2];
  }
  lexsim_t l(lsname, lspath);
  string s1, s2;
  while (true){
    cout <<"Input two strings:"<<endl;
    cin >> s1;
    cin >> s2;

    cout <<lsname <<" similarity of `"<< s1 << "' and `"<<s2 <<"': "<< l.get_sim(s1, s2, yisi::REF_MODE)<<endl;
  }

  /*
  cout <<"Constructing ibm"<<endl;
  lexsim_t ibm("ibm", ibm_path.c_str());
  cout<<"IBM score of `olive' and `olivo':"<< ibm("olive", "olivo")<<endl;
  */

  /*
  cout <<"Constructing exact"<<endl;
  lexsim_t ex("exact");
  cout <<"Exact similarity of `France' and `Italy':" <<ex("France", "Italy")<<endl;
  cout <<"Exact similarity of `France' and `France':" <<ex("France", "France")<<endl;
  */

  return 0;
}

