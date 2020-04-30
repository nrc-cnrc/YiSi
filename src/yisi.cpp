/**
 * @file yisi.cpp
 * @brief Main program for YiSi.
 *
 * @author Jackie Lo with updates by Serge Leger and Darlene Stewart
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "yisi.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace yisi;


extern "C" int eval(eval_options eval_opt, yisi_options yisi_opt, phrasesim_options phrasesim_opt)
{
   if (phrasesim_opt.reflexweight_name_m == "learn" && phrasesim_opt.reflexweight_path_m.empty()) {
      phrasesim_opt.reflexweight_path_m = eval_opt.ref_file_m;
   }
   if (phrasesim_opt.hyplexweight_name_m == "learn" && phrasesim_opt.hyplexweight_path_m.empty()) {
      phrasesim_opt.hyplexweight_path_m = eval_opt.hyp_file_m;
   }
   if (phrasesim_opt.inplexweight_name_m == "learn" && phrasesim_opt.inplexweight_path_m.empty()) {
      phrasesim_opt.inplexweight_path_m = eval_opt.inp_file_m;
   }

   yisiscorer_t yisi(yisi_opt, phrasesim_opt);

   if (eval_opt.sntscore_file_m.empty()) {
      eval_opt.sntscore_file_m = eval_opt.hyp_file_m + ".sntyisi";
   }

   if (eval_opt.docscore_file_m.empty()) {
      eval_opt.docscore_file_m = eval_opt.sntscore_file_m + ".docyisi";
   }

   // Open the output file early to make sure we can open it.
   ofstream sntout;
   open_ofstream(sntout, eval_opt.sntscore_file_m);

   cerr << "Reading hyp sents... ";
   vector<sent_t*> hypsents = read_sent(eval_opt.hyp_type_m, eval_opt.hyp_file_m,
                                        eval_opt.hyp_unit_delim_m, eval_opt.hypidemb_file_m,
					eval_opt.context_config_m);
   cerr << "Done." << endl;

   vector < vector<sent_t*> > refsents;
   if (! eval_opt.ref_file_m.empty()) {
      cerr << "Reading ref sents... ";
      auto reffiles = tokenize(eval_opt.ref_file_m, ':');
      vector<string> refidemb(reffiles.size(), "");
      if (! eval_opt.refidemb_file_m.empty())
         refidemb = tokenize(eval_opt.refidemb_file_m, ':');
      if (refidemb.size() != reffiles.size()) {
         cerr << "ERROR: number of components in refidemb-file (" << refidemb.size()
            << ") does not match the number of components in ref-file ("
            << reffiles.size() << "). Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
      refsents.resize(hypsents.size());
      for (size_t i = 0; i < reffiles.size(); i++) {
         auto rs = read_sent(eval_opt.ref_type_m, reffiles[i],
                             eval_opt.ref_unit_delim_m, refidemb[i], eval_opt.context_config_m);
         if (rs.size() != hypsents.size()) {
            cerr << "ERROR: No. of sentences in ref-file " << i << " (" << rs.size()
               << ") does not match with no. of sentences in hyp-file " << i
               << " (" << hypsents.size() << "). Check your input! Exiting..."
               << endl;
            exit(EXIT_FAILURE);
         }
         for (size_t j = 0; j < rs.size(); j++) {
            refsents[j].push_back(rs[j]);
         }
      }
      cerr << "Done." << endl;
   }

   vector<sent_t*> inpsents;
   if (! eval_opt.inp_file_m.empty()) {
      cerr << "Reading inp sents... ";
      inpsents = read_sent(eval_opt.inp_type_m, eval_opt.inp_file_m,
                           eval_opt.inp_unit_delim_m, eval_opt.inpidemb_file_m, eval_opt.context_config_m);
      if (inpsents.size() != hypsents.size()) {
         cerr << "ERROR: No. of sentences in inp-file (" << inpsents.size()
            << ") does not match with no. of sentences in hyp-file ("
            << hypsents.size() << "). Check your input! Exiting..." << endl;
         exit(EXIT_FAILURE);
      }
      cerr << "Done." << endl;
   }

   cerr << "Creating hyp srlgraphs... ";
   vector<srlgraph_t> hypsrlgraphs = yisi.hypsrlparse(hypsents);
   cerr << "Done." << endl;

   vector < vector<srlgraph_t> > refsrlgraphs(hypsrlgraphs.size());
   if (refsents.size() > 0) {
      cerr << "Creating ref srlgraphs... ";
      for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
         refsrlgraphs[i] = yisi.refsrlparse(refsents[i]);
      }
      cerr << "Done." << endl;
   }

   vector<srlgraph_t> inpsrlgraphs;
   if (inpsents.size() > 0) {
      cerr << "Creating inp srlgraphs... ";
      inpsrlgraphs = yisi.inpsrlparse(inpsents);
      cerr << "Done." << endl;
   }

   double docscore = 0.0;

   for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
      cout << "Evaluating line " << i + 1 << endl;
      yisigraph_t m;
      if (! eval_opt.inp_file_m.empty()) {
         // cerr << "inpsrlgraph:" << endl;
         // inpsrlgraphs[i].print(cout, i);
         // cerr << "hypsrlgraph:" << endl;
         // hypsrlgraphs[i].print(cout, i);
         // cerr << "yisigraph:" << endl;
         m = yisi.align(refsrlgraphs[i], hypsrlgraphs[i], inpsrlgraphs[i]);
         // m.print(cout);
      } else {
         // hypsrlgraphs[i].print(cout, i);
         m = yisi.align(refsrlgraphs[i], hypsrlgraphs[i]);
         // m.print(cout);
      }
      if (eval_opt.mode_m != "features") {
         double s = yisi.score(m);
         sntout << s << endl;
         docscore += s;
      } else {
         auto f = yisi.features(m);
         for (auto it = f.begin(); it != f.end(); it++) {
            sntout << *it << " ";
         }
         sntout << endl;
      }
   }
   sntout.close();

   if (eval_opt.mode_m != "features") {
      ofstream docout;
      open_ofstream(docout, eval_opt.docscore_file_m);
      docscore /= hypsents.size();
      docout << docscore << endl;
      docout.close();
   }

   // Clean up the memory allocated to sentences.
   for (auto it = hypsents.begin(); it != hypsents.end(); it++) {
      delete *it;
      *it = NULL;
   }
   for (auto it = refsents.begin(); it != refsents.end(); it++) {
      for (auto jt = it->begin(); jt != it->end(); jt++) {
         delete *jt;
         *jt = NULL;
      }
   }
   for (auto it = inpsents.begin(); it != inpsents.end(); it++) {
      delete *it;
      *it = NULL;
   }

   return 0;
}

int main(const int argc, const char* argv[])
{
   typedef com::masaers::cmdlp::options<eval_options, yisi_options, phrasesim_options> options_type;

   options_type opt(argc, argv);
   if (!opt) {
      return opt.exit_code();
   }

   if (opt.hyp_file_m.empty()) {
      cerr << "ERROR: Required option '--hyp-file' not set." << endl;
      return EXIT_FAILURE;
   }

   return eval(opt, opt, opt);
}
