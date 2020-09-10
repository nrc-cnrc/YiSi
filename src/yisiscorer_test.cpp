/**
 * @file yisiscorer_test.cpp
 * @brief Unit test for yisiscorer.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "cmdlp/options.h"
#include "yisiscorer.h"
#include "util.h"
#include "srl.h"

using namespace std;
using namespace yisi;

int main(const int argc, const char* argv[])
{
   typedef com::masaers::cmdlp::options<yisi_options, phrasesim_options> options_type;

   options_type opt(argc,argv);
   if (! opt) {
      return opt.exit_code();
   }

   yisiscorer_t yisi(opt, opt);

   string reffile("test_ref.en");
   string hypfile("test_hyp.en");
   vector<sent_t*> refsent = read_sent("word", reffile);
   vector<sent_t*> hypsents = read_sent("word", hypfile);
   vector<vector<sent_t*> > refsents;
   for (size_t i=0; i<refsent.size(); i++){
     vector<sent_t*> r;
     r.push_back(refsent[i]);
     refsents.push_back(r);
   }

   auto r1 = yisi.refsrlparse(refsents);
   auto r2 = yisi.hypsrlparse(hypsents);

   for (size_t i=0; i < r1.size(); i++) {
      //cerr << "Building YiSi graph" << endl;
      yisigraph_t m = yisi.align(r1[i],r2[i]);

      cout << "YiSi score is:" << yisi.score(m, "yisi") << endl;
   }
   for (auto it = refsents.begin(); it != refsents.end(); it++) {
     for (auto jt = it->begin(); jt != it->end(); jt++){
       delete *jt;
       *jt = NULL;
     }
   }
   for (auto it = hypsents.begin(); it != hypsents.end(); it++) {
      delete *it;
      *it = NULL;
   }
}
