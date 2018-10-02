/**
 * @file lw_count.cpp
 * @brief Unit test for lexweight counts.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "util.h"

#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include <vector>

using namespace std;
using namespace yisi;

// globals

// main

int main(int argc, char* argv[])
{
  string mode(argv[1]);
  if (mode == "count"){
    cerr<<"Collecting lexweight counts from "<< argv[1]<<"...";
    ifstream C(argv[2]);
    vector<vector<string> > tokens;
    if (!C){
      cerr <<"ERROR: fail to open corpus file. Exiting..."<<endl;
      exit(1);
    }
    while (!C.eof()){
      string sent;
      getline(C, sent);
      auto ts = tokenize(sent);
      tokens.push_back(ts);
    }
    auto N = tokens.size();
    map<string, double> lc;
    for (auto it=tokens.begin(); it != tokens.end(); it++){
      map<string, int> sent;
      for (auto jt=it->begin(); jt != it->end(); jt++){
	sent[*jt] = 1;
      }
      for (auto jt=sent.begin(); jt!=sent.end(); jt++){
	if (lc.find(jt->first) != lc.end()){
	  lc[jt->first] += 1.0;
	} else {
	  lc[jt->first] = 1.0;
	}
      }
    }
    ofstream fout(argv[3]);
    fout<<N<<endl;
    for (auto it=lc.begin(); it != lc.end(); it++){
      fout <<it->second <<" "<<it->first <<endl;
    }
    fout.close();
  } else if (mode == "merge"){
    int N=0;
    map<string, double> lw;
    vector<map<string, double> > llw;
    for (int i=2; i<argc-1; i++){
      llw.push_back(map<string, double>());
      cerr <<"reading " << argv[i] << "...";
      int n=0;
      ifstream fin(argv[i]);
      fin >>n;
      N += n;
      while (!fin.eof()){
	string l;
	double c;
	fin >> c >> l;
	cerr<<l<<endl;
	lw[l]=0;
	llw[i-2][l]=c;
      }
      fin.close();
      cerr <<"done." <<endl;
    }
    for (int i=2; i<argc-1; i++){
      cerr <<"summing " << argv[i] << "...";
      for (auto it=llw[i-2].begin(); it!=llw[i-2].end(); it++){
	lw[it->first] += it->second;
      }
      cerr <<"done." <<endl;
    }
    cerr <<"computing idf ...";
    for (auto it=lw.begin(); it!=lw.end(); it++){
      auto cc = it->second;
      it->second = log2(1 + N / cc);
    }
    cerr <<"done."<<endl;
    cerr <<"writing "<<argv[argc-1]<<"...";
    ofstream fout(argv[argc-1]);
    for (auto it=lw.begin(); it != lw.end(); it++){
      fout <<it->second <<" "<<it->first <<endl;
    }
    fout.close();
    cerr <<"done."<<endl;
  }
  return 0;
}

